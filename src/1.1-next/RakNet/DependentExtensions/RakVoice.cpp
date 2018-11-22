#include "RakVoice.h"
#include "speex/speex.h"
#include "speex/speex_preprocess.h"
#include "BitStream.h"
#include "PacketPriority.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakPeerInterface.h"
#include <malloc.h>
#include "GetTime.h"

#ifdef _DEBUG
#include <stdio.h>
#endif

//#define _TEST_LOOPBACK
//#define PRINT_DEBUG_INFO

#define SAMPLESIZE 2

#ifdef PRINT_DEBUG_INFO
#include <stdio.h>
#endif

int VoiceChannelComp( const SystemAddress &key, VoiceChannel * const &data )
{
	if (key < data->systemAddress)
		return -1;
	if (key == data->systemAddress)
		return 0;
	return 1;
}

RakVoice::RakVoice()
{
	rakPeer=0;
	bufferedOutput=0;
	defaultEncoderComplexity=2;
	defaultVADState=false;
	defaultDENOISEState=false;
	defaultVBRState=false;
}
RakVoice::~RakVoice()
{
	Deinit();
}
void RakVoice::Init(unsigned short sampleRate, unsigned bufferSizeBytes)
{
	// Record the parameters
	assert(sampleRate==8000 || sampleRate==16000 || sampleRate==32000);
	this->sampleRate=sampleRate;
	this->bufferSizeBytes=bufferSizeBytes;
	bufferedOutputCount=bufferSizeBytes/SAMPLESIZE;
	bufferedOutput = new float[bufferedOutputCount];
	unsigned i;
	for (i=0; i < bufferedOutputCount; i++)
		bufferedOutput[i]=0.0f;
	zeroBufferedOutput=false;

#ifdef _TEST_LOOPBACK
	Packet p;
	RakNet::BitStream out;
	out.Write((unsigned char)ID_RAKVOICE_OPEN_CHANNEL_REQUEST);
	out.Write(sampleRate);
	p.data=out.GetData();
	p.systemAddress=UNASSIGNED_SYSTEM_ADDRESS;
	p.length=out.GetNumberOfBytesUsed();
	OpenChannel(rakPeer, &p);
#endif
}
void RakVoice::Deinit(void)
{
	delete [] bufferedOutput;
	CloseAllChannels();
}
void RakVoice::RequestVoiceChannel(SystemAddress recipient)
{
	// Send a reliable ordered message to the other system to open a voice channel
	RakNet::BitStream out;
	out.Write((unsigned char)ID_RAKVOICE_OPEN_CHANNEL_REQUEST);
	out.Write(sampleRate);
	rakPeer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED,0,recipient,false);	
}
void RakVoice::CloseVoiceChannel(SystemAddress recipient)
{
	FreeChannelMemory(recipient);
	
	// Send a message to the remote system telling them to close the channel
	RakNet::BitStream out;
	out.Write((unsigned char)ID_RAKVOICE_CLOSE_CHANNEL);
	rakPeer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED,0,recipient,false);	
}
void RakVoice::CloseAllChannels(void)
{
	RakNet::BitStream out;
	out.Write((unsigned char)ID_RAKVOICE_CLOSE_CHANNEL);

	// Free the memory for all channels
	unsigned index;
	for (index=0; index < voiceChannels.Size(); index++)
	{
		rakPeer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED,0,voiceChannels[index]->systemAddress,false);	
		FreeChannelMemory(index);
	}
	voiceChannels.Clear();
}
bool RakVoice::SendFrame(SystemAddress recipient, void *inputBuffer)
{
	bool objectExists;
	unsigned index;
	VoiceChannel *channel;

	index = voiceChannels.GetIndexFromKey(recipient, &objectExists);
	if (objectExists)
	{
		unsigned totalBufferSize;
		unsigned remainingBufferSize;

		channel=voiceChannels[index];

		totalBufferSize=bufferSizeBytes * FRAME_OUTGOING_BUFFER_COUNT;
		if (channel->outgoingWriteIndex >= channel->outgoingReadIndex)
			remainingBufferSize=totalBufferSize-(channel->outgoingWriteIndex-channel->outgoingReadIndex);
		else
			remainingBufferSize=channel->outgoingReadIndex-channel->outgoingWriteIndex;

#ifdef _DEBUG
		assert(remainingBufferSize>0 && remainingBufferSize <= totalBufferSize);
	//	printf("SendFrame: buff=%i writeIndex=%i readIndex=%i\n",remainingBufferSize, channel->outgoingWriteIndex, channel->outgoingReadIndex);

		//printf("Writing %i bytes to write offset %i. %i %i.\n", bufferSizeBytes, channel->outgoingWriteIndex, *((char*)inputBuffer+channel->outgoingWriteIndex), *((char*)inputBuffer+channel->outgoingWriteIndex+bufferSizeBytes-1));
#endif

		// Copy encoded sound to the outgoing buffer for that channel.  This has to be fast, since this function is likely to be called from a locked buffer
		// I allocated the buffer to be a size multiple of bufferSizeBytes so don't have to watch for overflow on this line
		memcpy(channel->outgoingBuffer + channel->outgoingWriteIndex, inputBuffer, bufferSizeBytes );

#ifdef _DEBUG
		assert(channel->outgoingWriteIndex+bufferSizeBytes <= totalBufferSize);
#endif


		// Increment the write index, wrapping if needed.
		channel->outgoingWriteIndex+=bufferSizeBytes;
#ifdef _DEBUG
		// Verify that the write is aligned to the size of outgoingBuffer
		assert(channel->outgoingWriteIndex <= totalBufferSize);
#endif
		if (channel->outgoingWriteIndex==totalBufferSize)
			channel->outgoingWriteIndex=0;

		if (bufferSizeBytes >= remainingBufferSize) // Would go past the current read position
		{
#ifdef _DEBUG
			// This is actually a warning - it means that FRAME_OUTGOING_BUFFER_COUNT wasn't big enough and old data is being overwritten
			assert(0);
#endif
			// Force the read index up one block
			channel->outgoingReadIndex=(channel->outgoingReadIndex+channel->speexOutgoingFrameSampleCount * SAMPLESIZE)%totalBufferSize;			
		}

    	return true;
	}
	
	return false;
}
void RakVoice::ReceiveFrame(void *outputBuffer)
{
	short *out = (short*)outputBuffer;
	unsigned i;
	// Convert the floats to final 16-bits output
	for (i=0; i < bufferSizeBytes / SAMPLESIZE; i++)
	{
		if (bufferedOutput[i]>32767.0f)
			out[i]=32767;
		else if (bufferedOutput[i]<-32768.0f)
			out[i]=-32768;
		else
			out[i]=(short)bufferedOutput[i];
	}

	// Done with this block.  Zero all the values in Update
	zeroBufferedOutput=true;
}


int RakVoice::GetSampleRate(void) const
{
	return sampleRate;
}

int RakVoice::GetBufferSizeBytes(void) const
{
	return bufferSizeBytes;
}

bool RakVoice::IsInitialized(void) const
{
	// Use bufferedOutput to tell if the object was not initialized
	return (bufferedOutput!=0);
}

RakPeerInterface* RakVoice::GetRakPeerInterface(void) const
{
	return rakPeer;
}





void RakVoice::OnAttach(RakPeerInterface *peer)
{
	rakPeer=peer;
}
void RakVoice::OnShutdown(RakPeerInterface *peer)
{
	CloseAllChannels();
}

void RakVoice::Update(RakPeerInterface *peer)
{
	unsigned i,j, bytesAvailable, speexFramesAvailable, speexBlockSize;
	unsigned bytesWaitingToReturn;
	int bytesWritten;
	VoiceChannel *channel;
	char *inputBuffer;
	char tempOutput[2048];
	// 1 byte for ID, and 2 bytes(short) for Message number
	static const int headerSize=sizeof(unsigned char) + sizeof(unsigned short);
	// First byte is ID for RakNet
	tempOutput[0]=ID_RAKVOICE_DATA;
	
	RakNetTime currentTime = RakNet::GetTime();

	// Size of VoiceChannel::incomingBuffer and VoiceChannel::outgoingBuffer arrays
	unsigned totalBufferSize=bufferSizeBytes * FRAME_OUTGOING_BUFFER_COUNT;
	
	// Allow all channels to write, and set the output to zero in preparation
	if (zeroBufferedOutput)
	{
		for (i=0; i < bufferedOutputCount; i++)
			bufferedOutput[i]=0.0f;
		for (i=0; i < voiceChannels.Size(); i++)
			voiceChannels[i]->copiedOutgoingBufferToBufferedOutput=false;
		zeroBufferedOutput=false;
	}

	// For each channel
	for (i=0; i < voiceChannels.Size(); i++)
	{
		channel=voiceChannels[i];

		if (currentTime - channel->lastSend > 50) // Throttle to 20 sends a second
		{
			// Circular buffer so I have to do this to count how many bytes are available
			if (channel->outgoingWriteIndex>=channel->outgoingReadIndex)
				bytesAvailable=channel->outgoingWriteIndex-channel->outgoingReadIndex;
			else
				bytesAvailable=channel->outgoingWriteIndex + (totalBufferSize-channel->outgoingReadIndex);

			// Speex returns how many frames it encodes per block.  Each frame is of byte length sampleSize.
			speexBlockSize = channel->speexOutgoingFrameSampleCount * SAMPLESIZE;

#ifdef PRINT_DEBUG_INFO
			static int lastPrint=0;
			if (i==0 && currentTime-lastPrint > 2000)
			{
				lastPrint=currentTime;
				unsigned bytesWaitingToReturn;
				if (channel->incomingReadIndex <= channel->incomingWriteIndex)
					bytesWaitingToReturn=channel->incomingWriteIndex-channel->incomingReadIndex;
				else
					bytesWaitingToReturn=totalBufferSize-channel->incomingReadIndex+channel->incomingWriteIndex;

				printf("%i bytes to send. incomingMessageNumber=%i. bytesWaitingToReturn=%i.\n", bytesAvailable, channel->incomingMessageNumber, bytesWaitingToReturn );
			}
#endif

#ifdef _TEST_LOOPBACK
			/*
			if (bufferSizeBytes<bytesAvailable)
			{
				printf("Update: bytesAvailable=%i writeIndex=%i readIndex=%i\n",bytesAvailable, channel->outgoingWriteIndex, channel->outgoingReadIndex);
				memcpy(channel->incomingBuffer + channel->incomingWriteIndex, channel->outgoingBuffer+channel->outgoingReadIndex, bufferSizeBytes);
				channel->incomingWriteIndex=(channel->incomingWriteIndex+bufferSizeBytes) % totalBufferSize;
				channel->outgoingReadIndex=(channel->outgoingReadIndex+bufferSizeBytes) % totalBufferSize;
			}
			return;
			*/
#endif

			// Find out how many frames we can read out of the buffer for speex to encode and send these out.
			speexFramesAvailable = bytesAvailable / speexBlockSize;

			// Encode all available frames and send them unreliable sequenced
			if (speexFramesAvailable > 0)
			{
				SpeexBits speexBits;
				speex_bits_init(&speexBits);
				while (speexFramesAvailable-- > 0)
				{
					speex_bits_reset(&speexBits);

					// If the input data would wrap around the buffer, copy it to another buffer first
					if (channel->outgoingReadIndex + speexBlockSize >= totalBufferSize)
					{
#ifdef _DEBUG
						assert(speexBlockSize < 2048-1);
#endif
						unsigned t;
						for (t=0; t < speexBlockSize; t++)
							tempOutput[t+headerSize]=channel->outgoingBuffer[t%totalBufferSize];
						inputBuffer=tempOutput+headerSize;
					}
					else
						inputBuffer=channel->outgoingBuffer+channel->outgoingReadIndex;

#ifdef _DEBUG
					/*
					printf("In: ");
					if (shortSampleType)
					{
						short *blah = (short*) inputBuffer;
						for (int p=0; p < 5; p++)
						{
							printf("%.i ", blah[p]);
						}
					}
					else
					{
						float *blah = (float*) inputBuffer;
						for (int p=0; p < 5; p++)
						{
							printf("%.3f ", blah[p]);
						}
					}

					printf("\n");
*/
#endif
					int is_speech=1;

					// Run preprocessor if required
					if (defaultDENOISEState||defaultVADState){
						is_speech=speex_preprocess((SpeexPreprocessState*)channel->pre_state,(spx_int16_t*) inputBuffer, NULL );
					}

					if ((is_speech)||(!defaultVADState)){
						is_speech = speex_encode_int(channel->enc_state, (spx_int16_t*) inputBuffer, &speexBits);
					}

					channel->outgoingReadIndex=(channel->outgoingReadIndex+speexBlockSize)%totalBufferSize;

					// If no speech detected, don't send this frame
					if ((!is_speech)&&(defaultVADState)){
						continue;
					}

#ifdef _DEBUG
//					printf("Update: bytesAvailable=%i writeIndex=%i readIndex=%i\n",bytesAvailable, channel->outgoingWriteIndex, channel->outgoingReadIndex);
#endif

					bytesWritten = speex_bits_write(&speexBits, tempOutput+headerSize, 2048-headerSize);
#ifdef _DEBUG
					// If this assert hits then you need to increase the size of the temp buffer, but this is really a bug because
					// voice packets should never be bigger than a few hundred bytes.
					assert(bytesWritten!=2048-headerSize);
#endif

//					static int bytesSent=0;
//					bytesSent+= bytesWritten+headerSize;
//					printf("bytesSent=%i\n", bytesSent);

#ifdef PRINT_DEBUG_INFO
static int voicePacketsSent=0;
printf("%i ", voicePacketsSent++);
#endif

					// at +1, because the first byte in the buffer has the ID for RakNet.
					memcpy(tempOutput+1, &channel->outgoingMessageNumber, sizeof(unsigned short));
					channel->outgoingMessageNumber++;
					rakPeer->Send(tempOutput, bytesWritten+headerSize, HIGH_PRIORITY, UNRELIABLE,0,channel->systemAddress,false);

#ifdef _TEST_LOOPBACK
					Packet p;
					p.length=bytesWritten+1;
					p.data=(unsigned char*)tempOutput;
					p.systemAddress=channel->systemAddress;
					OnVoiceData(peer,&p);
#endif
				}

				speex_bits_destroy(&speexBits);
				channel->lastSend=currentTime;
			}
		}

		// As sound buffer blocks fill up, I add their values to RakVoice::bufferedOutput .  Then when the user calls ReceiveFrame they get that value, already
		// processed.  This is necessary because that function needs to run as fast as possible so I remove all processing there that I can.  Otherwise the sound
		// plays back distorted and popping
		if (channel->copiedOutgoingBufferToBufferedOutput==false)
		{
			if (channel->incomingReadIndex <= channel->incomingWriteIndex)
				bytesWaitingToReturn=channel->incomingWriteIndex-channel->incomingReadIndex;
			else
				bytesWaitingToReturn=totalBufferSize-channel->incomingReadIndex+channel->incomingWriteIndex;

			if (bytesWaitingToReturn==0)
			{
				channel->bufferOutput=true;
			}
			else if (channel->bufferOutput==false || bytesWaitingToReturn > bufferSizeBytes*2)
			{
				// Block running this again until the user calls ReceiveFrame since every call to ReceiveFrame only gets zero or one output blocks from
				// each channel
				channel->copiedOutgoingBufferToBufferedOutput=true;

				// Stop buffering output.  We won't start buffering again until there isn't enough data to read.
				channel->bufferOutput=false;

				// Cap to the size of the output buffer.  But we do write less if less is available, with the rest silence
				if (bytesWaitingToReturn > bufferSizeBytes)
				{
					bytesWaitingToReturn=bufferSizeBytes;
				}
				else
				{
					// Align the write index so when we increment the partial block read (which is always aligned) it computes out to 0 bytes waiting
					channel->incomingWriteIndex=channel->incomingReadIndex+bufferSizeBytes;
					if (channel->incomingWriteIndex==totalBufferSize)
						channel->incomingWriteIndex=0;
				}

				short *in = (short *) (channel->incomingBuffer+channel->incomingReadIndex);
				for (j=0; j < bytesWaitingToReturn / SAMPLESIZE; j++)
				{
					// Write short to float so if the range goes over the range of a float we can still add and subtract the correct final value.
					// It will be clamped at the end
					bufferedOutput[j]+=in[j%(totalBufferSize/SAMPLESIZE)];
				}

				// Update the read index.  Always update by bufferSizeBytes, not bytesWaitingToReturn.
				// if bytesWaitingToReturn < bufferSizeBytes then the rest is silence since this means the buffer ran out or we stopped sending.
				channel->incomingReadIndex+=bufferSizeBytes;
				if (channel->incomingReadIndex==totalBufferSize)
					channel->incomingReadIndex=0;

			//	printf("%f %f\n", channel->incomingReadIndex/(float)bufferSizeBytes, channel->incomingWriteIndex/(float)bufferSizeBytes);
			}
		}
	}
}
PluginReceiveResult RakVoice::OnReceive(RakPeerInterface *peer, Packet *packet)
{
	assert(packet);
	assert(peer);

	switch (packet->data[0]) 
	{
	case ID_RAKVOICE_OPEN_CHANNEL_REQUEST:
		OnOpenChannelRequest(peer, packet);
	break;
	case ID_RAKVOICE_OPEN_CHANNEL_REPLY:
		OnOpenChannelReply(peer, packet);
		break;
	case ID_RAKVOICE_CLOSE_CHANNEL:
	case ID_CONNECTION_LOST:
	case ID_DISCONNECTION_NOTIFICATION:
		FreeChannelMemory(packet->systemAddress);
	break;
	case ID_RAKVOICE_DATA:
		OnVoiceData(peer, packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	}

	return RR_CONTINUE_PROCESSING;
}
void RakVoice::OnCloseConnection(RakPeerInterface *peer, SystemAddress systemAddress)
{
	CloseVoiceChannel(systemAddress);
}

void RakVoice::OnOpenChannelRequest(RakPeerInterface *peer, Packet *packet)
{
	OpenChannel(peer,packet);

	RakNet::BitStream out;
	out.Write((unsigned char)ID_RAKVOICE_OPEN_CHANNEL_REPLY);
	out.Write(sampleRate);
	rakPeer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED,0,packet->systemAddress,false);	
}
void RakVoice::OnOpenChannelReply(RakPeerInterface *peer, Packet *packet)
{
	OpenChannel(peer, packet);
}
void RakVoice::OpenChannel(RakPeerInterface *peer, Packet *packet)
{
	RakNet::BitStream in(packet->data, packet->length, false);
	in.IgnoreBits(8);

	FreeChannelMemory(packet->systemAddress);

	VoiceChannel *channel=new VoiceChannel;
	channel->systemAddress=packet->systemAddress;

	if (in.Read(channel->remoteSampleRate)==false || (channel->remoteSampleRate!=8000 && channel->remoteSampleRate!=16000 && channel->remoteSampleRate!=32000))
	{
#ifdef _DEBUG
		assert(0);
#endif
		delete channel;
		return;
	}

	if (sampleRate==8000)
		channel->enc_state=speex_encoder_init(&speex_nb_mode);
	else if (sampleRate==16000)
		channel->enc_state=speex_encoder_init(&speex_wb_mode);
	else // 32000
		channel->enc_state=speex_encoder_init(&speex_uwb_mode);

	if (channel->remoteSampleRate==8000)
		channel->dec_state=speex_decoder_init(&speex_nb_mode);
	else if (channel->remoteSampleRate==16000)
		channel->dec_state=speex_decoder_init(&speex_wb_mode);
	else // 32000
		channel->dec_state=speex_decoder_init(&speex_uwb_mode);

	// make sure encoder and decoder are created
	assert((channel->enc_state)&&(channel->dec_state));

	int ret;
	ret=speex_encoder_ctl(channel->enc_state, SPEEX_GET_FRAME_SIZE, &channel->speexOutgoingFrameSampleCount);
	assert(ret==0);
	channel->outgoingBuffer = new char [bufferSizeBytes * FRAME_OUTGOING_BUFFER_COUNT];
	channel->outgoingReadIndex=0;
	channel->outgoingWriteIndex=0;
	channel->bufferOutput=true;
	channel->outgoingMessageNumber=0;
	channel->copiedOutgoingBufferToBufferedOutput=false;

	ret=speex_decoder_ctl(channel->dec_state, SPEEX_GET_FRAME_SIZE, &channel->speexIncomingFrameSampleCount);
	assert(ret==0);
	channel->incomingBuffer = new char [bufferSizeBytes * FRAME_INCOMING_BUFFER_COUNT];
	channel->incomingReadIndex=0;
	channel->incomingWriteIndex=0;
	channel->lastSend=0;
	channel->incomingMessageNumber=0;

	// Initialize preprocessor
	channel->pre_state = speex_preprocess_state_init(channel->speexOutgoingFrameSampleCount, sampleRate);
	assert(channel->pre_state);

	// Set encoder default parameters
	SetEncoderParameter(channel->enc_state, SPEEX_SET_VBR, (defaultVBRState) ? 1 : 0 );
	SetEncoderParameter(channel->enc_state, SPEEX_SET_COMPLEXITY, defaultEncoderComplexity);
	// Set preprocessor default parameters
	SetPreprocessorParameter(channel->pre_state, SPEEX_PREPROCESS_SET_DENOISE, (defaultDENOISEState) ? 1 : 2);
	SetPreprocessorParameter(channel->pre_state, SPEEX_PREPROCESS_SET_VAD, (defaultVADState) ? 1 : 2);

	voiceChannels.Insert(packet->systemAddress, channel, true);
}


void RakVoice::SetEncoderParameter(void* enc_state, int vartype, int val)
{
	if (enc_state){ 
		// Set parameter for just one encoder
		int ret = speex_encoder_ctl(enc_state, vartype, &val);
		assert(ret==0);		
	} else {
		// Set parameter for all encoders
		for (unsigned int index=0; index < voiceChannels.Size(); index++)
		{
			int ret = speex_encoder_ctl(voiceChannels[index]->enc_state, vartype, &val);
			assert(ret==0);
		}
	}
}

void RakVoice::SetPreprocessorParameter(void* pre_state, int vartype, int val)
{
	if (pre_state){
		// Set parameter for just one preprocessor
		int ret = speex_preprocess_ctl((SpeexPreprocessState*)pre_state, vartype, &val);
		assert(ret==0);
	} else {
		// Set parameter for all decoders
		for (unsigned int index=0; index < voiceChannels.Size(); index++)
		{
			int ret = speex_preprocess_ctl((SpeexPreprocessState*)voiceChannels[index]->pre_state, vartype, &val);
			assert(ret==0);
		}
	}
}

void RakVoice::SetEncoderComplexity(int complexity)
{
	assert((complexity>=0)&&(complexity<=10));
	SetEncoderParameter(NULL, SPEEX_SET_COMPLEXITY, complexity);
	defaultEncoderComplexity = complexity;
}
void RakVoice::SetVAD(bool enable)
{
	SetPreprocessorParameter(NULL, SPEEX_PREPROCESS_SET_VAD, (enable)? 1 : 2);
	defaultVADState = enable;
}
void RakVoice::SetNoiseFilter(bool enable)
{
	SetPreprocessorParameter(NULL, SPEEX_PREPROCESS_SET_DENOISE, (enable) ? 1 : 2);
	defaultDENOISEState = enable;
}
void RakVoice::SetVBR(bool enable)
{
	SetEncoderParameter(NULL, SPEEX_SET_VBR, (enable) ? 1 : 0);
	defaultVBRState = enable;
}

int RakVoice::GetEncoderComplexity(void)
{
	return defaultEncoderComplexity;
}
bool RakVoice::IsVADActive(void)
{
	return defaultVADState;
}
bool RakVoice::IsNoiseFilterActive()
{
	return defaultDENOISEState;
}
bool RakVoice::IsVBRActive()
{
	return defaultVBRState;
}


void RakVoice::FreeChannelMemory(SystemAddress recipient)
{
	bool objectExists;
	unsigned index;
	index = voiceChannels.GetIndexFromKey(recipient, &objectExists);

	// Free the memory for this channel
	if (objectExists)
	{
		FreeChannelMemory(index);
	}
}
void RakVoice::FreeChannelMemory(unsigned index)
{
	VoiceChannel *channel;
	channel=voiceChannels[index];
	speex_encoder_destroy(channel->enc_state);
	speex_decoder_destroy(channel->dec_state);
	speex_preprocess_state_destroy((SpeexPreprocessState*)channel->pre_state);
	delete [] channel->incomingBuffer;
	delete [] channel->outgoingBuffer;
	delete channel;
	voiceChannels.RemoveAtIndex(index);
}
void RakVoice::OnVoiceData(RakPeerInterface *peer, Packet *packet)
{
	bool objectExists;
	unsigned index;
	unsigned short packetMessageNumber, messagesSkipped;
	VoiceChannel *channel;
	char tempOutput[2048];
	unsigned int i;
	// 1 byte for ID, 2 bytes(short) for message number
	static const int headerSize=sizeof(unsigned char) + sizeof(unsigned short);

	index = voiceChannels.GetIndexFromKey(packet->systemAddress, &objectExists);
	if (objectExists)
	{
		SpeexBits speexBits;
		speex_bits_init(&speexBits);
		channel=voiceChannels[index];
		memcpy(&packetMessageNumber, packet->data+1, sizeof(unsigned short));

		// Intentional overflow
		messagesSkipped=packetMessageNumber-channel->incomingMessageNumber;
		if (messagesSkipped > ((unsigned short)-1)/2)
		{
#ifdef PRINT_DEBUG_INFO
			printf("--- UNDERFLOW ---\n");
#endif
			// Underflow, just ignore it
			return;
		}
#ifdef PRINT_DEBUG_INFO
		if (messagesSkipped>0)
			printf("%i messages skipped\n", messagesSkipped);
#endif
		// Don't do more than 100 ms of messages skipped.  Discard the rest.
		int maxSkip = (int)(100.0f / (float) sampleRate);
		for (i=0; i < (unsigned) messagesSkipped && i < (unsigned) maxSkip; i++)
		{
			speex_decode_int(channel->dec_state, 0, (spx_int16_t*)tempOutput);

			// Write to buffer a 'message skipped' interpolation
			WriteOutputToChannel(channel, tempOutput);
		}
	
		channel->incomingMessageNumber=packetMessageNumber+1;

		// Write to incomingBuffer the decoded data
		speex_bits_read_from(&speexBits, (char*)(packet->data+headerSize), packet->length-headerSize);
		speex_decode_int(channel->dec_state, &speexBits, (spx_int16_t*)tempOutput);

#ifdef _DEBUG
		{
			/*
			printf("Out: ");
			if (channel->remoteIsShortSampleType)
			{
				short *blah = (short*) tempOutput;
				for (int p=0; p < 5; p++)
				{
					printf("%.i ", blah[p]);
				}
			}
			else
			{
				float *blah = (float*) tempOutput;
				for (int p=0; p < 5; p++)
				{
					printf("%.3f ", blah[p]);
				}
			}
			
			printf("\n");
			*/
		}
#endif

		// Write to buffer
		WriteOutputToChannel(channel, tempOutput);

		speex_bits_destroy(&speexBits);
	}
}
void RakVoice::WriteOutputToChannel(VoiceChannel *channel, char *dataToWrite)
{
	unsigned totalBufferSize;
	unsigned remainingBufferSize;
	unsigned speexBlockSize;

	totalBufferSize=bufferSizeBytes * FRAME_INCOMING_BUFFER_COUNT;
	if (channel->incomingWriteIndex >= channel->incomingReadIndex)
		remainingBufferSize=totalBufferSize-(channel->incomingWriteIndex-channel->incomingReadIndex);
	else
		remainingBufferSize=channel->incomingReadIndex-channel->incomingWriteIndex;

	// Speex returns how many frames it encodes per block.  Each frame is of byte length sampleSize.
	speexBlockSize = channel->speexIncomingFrameSampleCount * SAMPLESIZE;

	if (channel->incomingWriteIndex+speexBlockSize <= totalBufferSize)
	{
		memcpy(channel->incomingBuffer + channel->incomingWriteIndex, dataToWrite, speexBlockSize);
	}
	else
	{
		memcpy(channel->incomingBuffer + channel->incomingWriteIndex, dataToWrite, totalBufferSize-channel->incomingWriteIndex);
		memcpy(channel->incomingBuffer, dataToWrite, speexBlockSize-(totalBufferSize-channel->incomingWriteIndex));
	}
    channel->incomingWriteIndex=(channel->incomingWriteIndex+speexBlockSize) % totalBufferSize;


#ifdef _DEBUG
	//printf("WriteOutputToChannel: buff=%i writeIndex=%i readIndex=%i\n",remainingBufferSize, channel->incomingWriteIndex, channel->incomingReadIndex);
#endif

	if (bufferSizeBytes >= remainingBufferSize) // Would go past the current read position
	{
#ifdef _DEBUG
		// This is actually a warning - it means that FRAME_INCOMING_BUFFER_COUNT wasn't big enough and old data is being overwritten
		assert(0);
#endif
		// Force the read index up one block
		channel->incomingReadIndex+=bufferSizeBytes;
		if (channel->incomingReadIndex==totalBufferSize)
			channel->incomingReadIndex=0;
	}
}

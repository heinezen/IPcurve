/// \file
/// \brief Voice compression and transmission interface
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __RAK_VOICE_H
#define __RAK_VOICE_H

class RakPeerInterface;
#include "RakNetTypes.h"
#include "PluginInterface.h"
#include "DS_OrderedList.h"

// How many frames large to make the circular buffers in the VoiceChannel structure
#define FRAME_OUTGOING_BUFFER_COUNT 100
#define FRAME_INCOMING_BUFFER_COUNT 100

/// \internal
struct VoiceChannel
{
	SystemAddress systemAddress;
	void *enc_state;
	void *dec_state;
	void *pre_state;
	unsigned short remoteSampleRate;
	
	// Circular buffer of unencoded sound data read from the user.
	char *outgoingBuffer;
	// Each frame sent to speex requires this many samples, of whatever size you are using.
	int speexOutgoingFrameSampleCount;
	// Index in is bytes.
	// Write index points to the next byte to write to, which must be free.
	unsigned outgoingReadIndex, outgoingWriteIndex;
	bool bufferOutput;
	bool copiedOutgoingBufferToBufferedOutput;
	unsigned short outgoingMessageNumber;

	// Circular buffer of unencoded sound data to be passed to the user.  Each element in the buffer is of size bufferSizeBytes bytes.
	char *incomingBuffer;
	int speexIncomingFrameSampleCount;
	unsigned incomingReadIndex, incomingWriteIndex;	// Index in bytes
	unsigned short incomingMessageNumber;  // The ID_VOICE message number we expect to get.  Used to drop out of order and detect how many missing packets in a sequence

	RakNetTime lastSend;
};
int VoiceChannelComp( const SystemAddress &key, VoiceChannel * const &data );

/// \brief Encodes, decodes, and transmits voice data.
/// Voice compression and transmission interface
/// \addtogroup Plugins
class RakVoice : public PluginInterface
{
public:
	RakVoice();
	~RakVoice();

	// --------------------------------------------------------------------------------------------
	// User functions
	// --------------------------------------------------------------------------------------------

	/// \brief Starts RakVoice
	/// \param[in] speexSampleRate 8000, 16000, or 32000
	/// \param[in] bufferSizeBytes How many bytes long inputBuffer and outputBuffer are in SendFrame and ReceiveFrame are.  Should be your sample size * the number of samples to encode at once.
	void Init(unsigned short speexSampleRate, unsigned bufferSizeBytes);

	/// \brief Changes encoder complexity
	/// Specifying higher values might help when encoding non-speech sounds.
	/// \param[in] complexity 0 to 10. The higher the value, the more CPU it needs. Recommended values are from 2 to 4.
	void SetEncoderComplexity(int complexity);

	/// \brief Enables or disables VAD (Voice Activity Detection)
	/// Enabling VAD can help reduce the amount of data transmitted, by automatically disabling outgoing data, when no voice is detected.
	/// \pre Only applies to encoder.
	/// \param[in] enable true to enable, false to disable
	void SetVAD(bool enable);

	/// \brief Enables or disables the noise filter
	/// \pre Only applies to encoder.
	/// \param[in] enable true to enable, false to disable.
	void SetNoiseFilter(bool enable);

	/// \brief Enables or disables VBR
	/// \pre Only applies to encoder.
	/// \param[in] enable true to enable VBR, false to disable
	void SetVBR(bool enable);

	/// \brief Returns the complexity of the encoder
	/// \pre Only applies to encoder.
	/// \return a value from 0 to 10.
	int GetEncoderComplexity(void);

	/// \brief Returns current state of VAD.
	/// \pre Only applies to encoder.
	/// \return true if VAD is enable, false otherwise
	bool IsVADActive(void);

	/// \brief Returns the current state of the noise filter
	/// \pre Only applies to encoder.
	/// \return true if the noise filter is active, false otherwise.
	bool IsNoiseFilterActive();

	/// \brief Returns the current state of VBR
	/// \pre Only applies to encoder.
	/// \return true if VBR is active, false otherwise.
	bool IsVBRActive();

	/// Shuts down RakVoice
	void Deinit(void);
	
	/// \brief Opens a channel to another connected system
	/// You will get ID_RAKVOICE_OPEN_CHANNEL_REPLY on success
	/// \param[in] recipient Which system to open a channel to
	void RequestVoiceChannel(SystemAddress recipient);

	/// \brief Closes an existing voice channel.
	/// Other system will get ID_RAKVOICE_CLOSE_CHANNEL
	/// \param[in] recipient Which system to close a channel with
	void CloseVoiceChannel(SystemAddress recipient);

	/// \brief Closes all existing voice channels
	/// Other systems will get ID_RAKVOICE_CLOSE_CHANNEL
	void CloseAllChannels(void);

	/// \brief Sends voice data to a system on an open channel
	/// \pre \a recipient must refer to a system with an open channel via RequestVoiceChannel
	/// \param[in] recipient The system to send voice data to
	/// \param[in] inputBuffer The voice data.  The size of inputBuffer should be what was specified as bufferSizeBytes in Init
	bool SendFrame(SystemAddress recipient, void *inputBuffer);

	/// \brief Gets decoded voice data, from one or more remote senders
	/// \param[out] outputBuffer The voice data.  The size of outputBuffer should be what was specified as bufferSizeBytes in Init
	void ReceiveFrame(void *outputBuffer);

	/// Returns the value sample rate, as passed to Init
	/// \return the sample rate
	int GetSampleRate(void) const;

	/// Returns the buffer size in bytes, as passed to Init
	/// \return buffer size in bytes
	int GetBufferSizeBytes(void) const;

	/// Returns true or false, indicating if the object has been initialized
	/// \return true if initialized, false otherwise.
	bool IsInitialized(void) const;

	/// Returns the RakPeerInterface that the object is attached to.
	/// \return the respective RakPeerInterface, or NULL not attached.
	RakPeerInterface* GetRakPeerInterface(void) const;

	// --------------------------------------------------------------------------------------------
	// Message handling functions
	// --------------------------------------------------------------------------------------------
	virtual void OnAttach(RakPeerInterface *peer);
	virtual void OnShutdown(RakPeerInterface *peer);
	virtual void Update(RakPeerInterface *peer);
	virtual PluginReceiveResult OnReceive(RakPeerInterface *peer, Packet *packet);
	virtual void OnCloseConnection(RakPeerInterface *peer, SystemAddress systemAddress);
protected:
	void OnOpenChannelRequest(RakPeerInterface *peer, Packet *packet);
	void OnOpenChannelReply(RakPeerInterface *peer, Packet *packet);
	void OnVoiceData(RakPeerInterface *peer, Packet *packet);
	void OpenChannel(RakPeerInterface *peer, Packet *packet);
	void FreeChannelMemory(SystemAddress recipient);
	void FreeChannelMemory(unsigned index);
	void WriteOutputToChannel(VoiceChannel *channel, char *dataToWrite);
	void SetEncoderParameter(void* enc_state, int vartype, int val);
	void SetPreprocessorParameter(void* pre_state, int vartype, int val);
	
	RakPeerInterface *rakPeer;
	DataStructures::OrderedList<SystemAddress, VoiceChannel*, VoiceChannelComp> voiceChannels;
	int sampleRate;
	unsigned bufferSizeBytes;
	float *bufferedOutput;
	unsigned bufferedOutputCount;
	bool zeroBufferedOutput;
	int defaultEncoderComplexity;
	bool defaultVADState;
	bool defaultDENOISEState;
	bool defaultVBRState;

};

#endif

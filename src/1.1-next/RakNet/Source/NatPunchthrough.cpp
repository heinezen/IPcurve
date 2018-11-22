#include "NatPunchthrough.h"
#include "GetTime.h"
#include "RakPeerInterface.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "RakAssert.h"

//#include <stdio.h>

// Number is arbitrary, but you want a long enough interval to get a good sample, but short enough not to annoy the user.
static const int PING_INTERVAL=100;
// Fill up the ping buffer
static const int PING_COUNT=PING_TIMES_ARRAY_SIZE;
// What multiple to multiply the ping by to figure out how long to wait before sending the start time.
// Should be slightly larger than the ping so the last ping arrives
static const float SEND_TIMESTAMP_DELAY_PING_MULTIPLE=1.5f;
// What multiple to multiply the ping by to figure out when to send the NAT punchthrough request
// Should be large enough to make sure both systems get the message in the future, even if it needs to be resent due to packetloss.
// Too large is bad because the user has to wait and it makes it more likely our estimated ping is wrong
static const float SEND_PUNCHTHROUGH_DELAY_PING_MULTIPLE=5.0f;

#ifdef _MSC_VER
#pragma warning( push )
#endif


NatPunchthrough::NatPunchthrough()
{
	allowFacilitation=true;
	rakPeer=0;
}
NatPunchthrough::~NatPunchthrough()
{
	Clear();
}
void NatPunchthrough::FacilitateConnections(bool allow)
{
	allowFacilitation=allow;
}
bool NatPunchthrough::Connect(const char* host, unsigned short remotePort, const char *passwordData, int passwordDataLength, SystemAddress facilitator)
{
	SystemAddress systemAddress;
	systemAddress.SetBinaryAddress(host);
	systemAddress.port=remotePort;
	return Connect(systemAddress, passwordData, passwordDataLength, facilitator);
}
bool NatPunchthrough::Connect(SystemAddress receiver, const char *passwordData, int passwordDataLength, SystemAddress facilitator)
{
	if (rakPeer->IsConnected(facilitator)==false)
		return false;

	RakNet::BitStream outBitstream;
	outBitstream.Write((MessageID)ID_NAT_PUNCHTHROUGH_REQUEST);
	outBitstream.Write(receiver);

	// Remember this connection request
	NatPunchthrough::ConnectionRequest *connectionRequest = new NatPunchthrough::ConnectionRequest;
	connectionRequest->receiver=receiver;
	connectionRequest->facilitator=facilitator;
	if (passwordDataLength)
	{
		connectionRequest->passwordData = new char [passwordDataLength];
		memcpy(connectionRequest->passwordData, passwordData, passwordDataLength);
	}
	else
		connectionRequest->passwordData = 0;
	connectionRequest->sender=UNASSIGNED_SYSTEM_ADDRESS;
	connectionRequest->passwordDataLength=passwordDataLength;
	connectionRequest->facilitator=facilitator;
	connectionRequest->nextActionTime=0;
	connectionRequest->facilitatingConnection=false;
	connectionRequest->timeoutTime=RakNet::GetTime()+30000;
	connectionRequest->pingCount=0;
	connectionRequestList.Insert(connectionRequest);

	rakPeer->Send(&outBitstream, HIGH_PRIORITY, RELIABLE, 0, facilitator, false);

	return true;
}
void NatPunchthrough::Clear(void)
{
	unsigned i;
	for (i=0; i < connectionRequestList.Size(); i++)
	{
		delete [] connectionRequestList[i]->passwordData;
		delete connectionRequestList[i];
	}
	connectionRequestList.Clear();
}
void NatPunchthrough::OnAttach(RakPeerInterface *peer)
{
	rakPeer=peer;
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void NatPunchthrough::Update(RakPeerInterface *peer)
{
	if (connectionRequestList.Size())
	{
		RakNetTime time = RakNet::GetTime();
		RakNet::BitStream outBitstream;
		unsigned i;
		i=0;
		while (i < connectionRequestList.Size())
		{
			// Remove old connection requests that get no answer
			if (connectionRequestList[i]->timeoutTime < time)
			{
				delete [] connectionRequestList[i]->passwordData;
				delete connectionRequestList[i];
				connectionRequestList.RemoveAtIndex(i);
				continue;
			}
			
			// If we are a facilitator, do pings
			if (connectionRequestList[i]->facilitatingConnection)
			{
				if (time >= connectionRequestList[i]->nextActionTime )
				{
					if (connectionRequestList[i]->pingCount < PING_COUNT)
					{
						// Ping
						connectionRequestList[i]->pingCount++;
						rakPeer->Ping(connectionRequestList[i]->receiver);
						rakPeer->Ping(connectionRequestList[i]->sender);
						connectionRequestList[i]->nextActionTime=time+PING_INTERVAL;
					}
					else if (connectionRequestList[i]->pingCount == PING_COUNT)
					{
						// Done pinging.  Wait till the next stage
						int receiverPing, senderPing;
						receiverPing=rakPeer->GetAveragePing(connectionRequestList[i]->receiver);
						senderPing=rakPeer->GetAveragePing(connectionRequestList[i]->sender);
						if (receiverPing > senderPing)
							connectionRequestList[i]->nextActionTime=(RakNetTime)(receiverPing*SEND_TIMESTAMP_DELAY_PING_MULTIPLE);
						else
							connectionRequestList[i]->nextActionTime=(RakNetTime)(senderPing*SEND_TIMESTAMP_DELAY_PING_MULTIPLE);
						connectionRequestList[i]->pingCount++;
					}
					else
					{
						// Send the timestamped message to both systems so they send datagrams to each other simultaneously
						int receiverPing, senderPing;
						RakNetTime delayTime;
						receiverPing=rakPeer->GetAveragePing(connectionRequestList[i]->receiver);
						senderPing=rakPeer->GetAveragePing(connectionRequestList[i]->sender);
						if (receiverPing > senderPing)
							delayTime=(RakNetTime)(receiverPing*SEND_PUNCHTHROUGH_DELAY_PING_MULTIPLE);
						else
							delayTime=(RakNetTime)(senderPing*SEND_PUNCHTHROUGH_DELAY_PING_MULTIPLE);

						outBitstream.Reset();
						outBitstream.Write((MessageID)ID_TIMESTAMP);
						outBitstream.Write(time+delayTime);
						outBitstream.Write((MessageID)ID_NAT_CONNECT_AT_TIME);
						rakPeer->Send(&outBitstream, SYSTEM_PRIORITY, RELIABLE, 0, connectionRequestList[i]->sender, false);

						outBitstream.Reset();
						outBitstream.Write((MessageID)ID_TIMESTAMP);
						outBitstream.Write(time+delayTime);
						outBitstream.Write((MessageID)ID_NAT_SEND_OFFLINE_MESSAGE_AT_TIME);
						outBitstream.Write(connectionRequestList[i]->sender);
						rakPeer->Send(&outBitstream, SYSTEM_PRIORITY, RELIABLE, 0, connectionRequestList[i]->receiver, false);

						delete [] connectionRequestList[i]->passwordData;
						delete connectionRequestList[i];
						connectionRequestList.RemoveAtIndex(i);
						continue;
					}
				}	
			}
			// Else not a facilitator.  If nextActionTime is non-zero, connect or send the offline message
			else if (connectionRequestList[i]->nextActionTime && time >= connectionRequestList[i]->nextActionTime)
			{
				if (connectionRequestList[i]->receiver!=UNASSIGNED_SYSTEM_ADDRESS)
				{
					// Connect to this system.
					rakPeer->Connect(connectionRequestList[i]->receiver.ToString(false), connectionRequestList[i]->receiver.port, connectionRequestList[i]->passwordData, connectionRequestList[i]->passwordDataLength);
				}
				else
				{
					RakAssert(connectionRequestList[i]->sender!=UNASSIGNED_SYSTEM_ADDRESS);

					// Send offline message to this system, hopefully at the exact same time that system tries to connect to us.
					rakPeer->Ping(connectionRequestList[i]->sender.ToString(false), connectionRequestList[i]->sender.port, false);
				}

				delete [] connectionRequestList[i]->passwordData;
				delete connectionRequestList[i];
				connectionRequestList.RemoveAtIndex(i);
				continue;
			}
			
			i++;
		}
	}
}
PluginReceiveResult NatPunchthrough::OnReceive(RakPeerInterface *peer, Packet *packet)
{
	unsigned char packetId;
	if (packet->length>sizeof(MessageID)+sizeof(RakNetTime) && packet->data[0]==ID_TIMESTAMP)
		packetId=packet->data[sizeof(MessageID)+sizeof(RakNetTime)];
	else
		packetId=packet->data[0];

	switch (packetId) 
	{
	case ID_NAT_PUNCHTHROUGH_REQUEST:
		OnPunchthroughRequest(peer, packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE; // Absorb
	case ID_NAT_TARGET_CONNECTION_LOST:
	case ID_NAT_TARGET_NOT_CONNECTED:
		// Sender only
		return RemoveRequestByFacilitator(packet->systemAddress);
	case ID_NAT_CONNECT_AT_TIME:
		OnConnectAtTime(peer, packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE; // Absorb
	case ID_NAT_SEND_OFFLINE_MESSAGE_AT_TIME:
		OnSendOfflineMessageAtTime(peer, packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE; // Absorb
	case ID_DISCONNECTION_NOTIFICATION:
	case ID_CONNECTION_LOST:
		OnCloseConnection(peer, packet->systemAddress);
		return RR_CONTINUE_PROCESSING;
	}

	return RR_CONTINUE_PROCESSING;
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void NatPunchthrough::OnCloseConnection(RakPeerInterface *peer, SystemAddress systemAddress)
{
	if (allowFacilitation==false)
		return;

	// If in ping mode, send ID_NAT_TARGET_CONNECTION_LOST
	if (connectionRequestList.Size())
	{
		unsigned i;
		i=0;
		while (i < connectionRequestList.Size())
		{
			if (connectionRequestList[i]->facilitatingConnection &&
				(connectionRequestList[i]->receiver==systemAddress ||
				connectionRequestList[i]->sender==systemAddress))
			{
				// This field is not used by the facilitator.
				RakAssert(connectionRequestList[i]->passwordData==0);

				if (connectionRequestList[i]->sender==systemAddress)
				{
					RakNet::BitStream outBitstream;
					outBitstream.Write((MessageID)ID_NAT_TARGET_CONNECTION_LOST);
					outBitstream.Write(connectionRequestList[i]->receiver);
					rakPeer->Send(&outBitstream, HIGH_PRIORITY, RELIABLE, 0, connectionRequestList[i]->sender, false);
				}

				delete [] connectionRequestList[i]->passwordData;
				delete connectionRequestList[i];
				connectionRequestList.RemoveAtIndex(i);
			}
			else
				i++;
		}
	}
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void NatPunchthrough::OnShutdown(RakPeerInterface *peer)
{

}
PluginReceiveResult NatPunchthrough::RemoveRequestByFacilitator(SystemAddress systemAddress)
{
	unsigned i;
	i=0;
	while (i < connectionRequestList.Size())
	{
		if (connectionRequestList[i]->facilitator==systemAddress)
		{
			delete [] connectionRequestList[i]->passwordData;
			delete connectionRequestList[i];
			connectionRequestList.RemoveAtIndex(i);
			return RR_CONTINUE_PROCESSING;
		}
		else
			i++;
	}
	return RR_STOP_PROCESSING_AND_DEALLOCATE;
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void NatPunchthrough::OnPunchthroughRequest(RakPeerInterface *peer, Packet *packet)
{
	if (allowFacilitation==false)
		return;

	SystemAddress target;
	RakNet::BitStream inBitstream(packet->data, packet->length, false);
	RakNet::BitStream outBitstream;
	inBitstream.IgnoreBits(8);
	if (inBitstream.Read(target)==false)
		return;
	if (rakPeer->IsConnected(target)==false)
	{
		outBitstream.Write((MessageID)ID_NAT_TARGET_NOT_CONNECTED);
		outBitstream.Write(target);
		rakPeer->Send(&outBitstream, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, false);
	}
	else
	{
		// Remember this connection request
		NatPunchthrough::ConnectionRequest *connectionRequest = new NatPunchthrough::ConnectionRequest;
		connectionRequest->receiver=target;
		connectionRequest->facilitator=UNASSIGNED_SYSTEM_ADDRESS;
		connectionRequest->passwordData = 0;
		connectionRequest->sender=packet->systemAddress;
		connectionRequest->passwordDataLength=0;
		connectionRequest->facilitatingConnection=true;
		connectionRequest->nextActionTime=RakNet::GetTime()+PING_INTERVAL;
		connectionRequest->pingCount=1;
		connectionRequest->timeoutTime=RakNet::GetTime()+30000;
		connectionRequestList.Insert(connectionRequest);

		rakPeer->Ping(connectionRequest->receiver);
		rakPeer->Ping(connectionRequest->sender);
	}
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void NatPunchthrough::OnConnectAtTime(RakPeerInterface *peer, Packet *packet)
{
	// Find this connection request and set the connection timer.
	unsigned i;
	for (i=0; i < connectionRequestList.Size(); i++)
	{
		if (connectionRequestList[i]->nextActionTime==0 &&
			connectionRequestList[i]->facilitatingConnection==false &&
			connectionRequestList[i]->facilitator==packet->systemAddress)
		{
			// Send a message at the router to the remote system, to open up the firewall / NAT
			peer->SendTTL1(connectionRequestList[i]->receiver.ToString(false), connectionRequestList[i]->receiver.port, 0);

			RakNet::BitStream inBitstream(packet->data, packet->length, false);
			inBitstream.IgnoreBits(8);
			inBitstream.Read(connectionRequestList[i]->nextActionTime);
			return;
		}
	}
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void NatPunchthrough::OnSendOfflineMessageAtTime(RakPeerInterface *peer, Packet *packet)
{
	RakNet::BitStream inBitstream(packet->data, packet->length, false);
	RakNetTime nextActionTime;
	inBitstream.IgnoreBits(8);
	SystemAddress sender;
	inBitstream.Read(nextActionTime);
	inBitstream.IgnoreBits(8);
	if (inBitstream.Read(sender)==false)
		return;

	NatPunchthrough::ConnectionRequest *connectionRequest = new NatPunchthrough::ConnectionRequest;
	connectionRequest->receiver=UNASSIGNED_SYSTEM_ADDRESS;
	connectionRequest->facilitator=packet->systemAddress;
	connectionRequest->sender=sender;
	connectionRequest->passwordData = 0;
	connectionRequest->passwordDataLength=0;
	connectionRequest->facilitatingConnection=false;
	connectionRequest->nextActionTime=nextActionTime;
	connectionRequest->pingCount=0;
	connectionRequest->timeoutTime=RakNet::GetTime()+30000;
	connectionRequestList.Insert(connectionRequest);

	// Send a message at the router to the remote system, to open up the firewall / NAT
	peer->SendTTL1(sender.ToString(false),sender.port, 0);
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

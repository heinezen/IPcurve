/// \file
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

#include "PacketLogger.h"
#include "BitStream.h"
#include "DS_List.h"
#include "InternalPacket.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "StringCompressor.h"
#include "RPCMap.h"
#include "GetTime.h"
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning( push )
#endif

PacketLogger::PacketLogger()
{
	rakPeer=0;
	printId=true;
	printAcks=true;
	prefix[0]=0;
	suffix[0]=0;
}
PacketLogger::~PacketLogger()
{
}
void PacketLogger::OnAttach(RakPeerInterface *peer)
{
	rakPeer=peer;
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100 : unreferenced formal parameter
#endif
void PacketLogger::Update(RakPeerInterface *peer)
{

}
void PacketLogger::OnDirectSocketSend(const char *data, const unsigned bitsUsed, SystemAddress remoteSystemAddress)
{
	char str[256];
	SystemAddress localSystemAddress;
	localSystemAddress = rakPeer->GetInternalID();

	if (printId==false)
	{
		sprintf(str, "%sSnd,Raw,  NIL,  NIL,%5i,%5i,%i,%u:%i,%u:%i%s", prefix, data[0], bitsUsed,RakNet::GetTime(),
			localSystemAddress.binaryAddress, localSystemAddress.port, remoteSystemAddress.binaryAddress, remoteSystemAddress.port, suffix);
	}
	else
	{
		sprintf(str, "%sSnd,Raw,NIL,NIL,%s,%i,%i,%u:%i,%u:%i%s", prefix,IDTOString(data[0]), bitsUsed,RakNet::GetTime(),
			localSystemAddress.binaryAddress, localSystemAddress.port, remoteSystemAddress.binaryAddress, remoteSystemAddress.port, suffix);
	}
	AddToLog(str);
}

void PacketLogger::LogHeader(void)
{
	AddToLog("S|R,Typ,Pckt#,Frm #,PktID,BitLn,Time     ,Local IP:Port   ,RemoteIP:Port\n");
}
void PacketLogger::OnDirectSocketReceive(const char *data, const unsigned bitsUsed, SystemAddress remoteSystemAddress)
{
	char str[256];
	SystemAddress localSystemAddress;
	localSystemAddress = rakPeer->GetInternalID();
	if (printId==false)
	{
		sprintf(str, "%sRcv,Raw,  NIL,  NIL,%5i,%5i,%i,%u:%i,%u:%i%s", prefix,data[0], bitsUsed,RakNet::GetTime(),
			localSystemAddress.binaryAddress, localSystemAddress.port, remoteSystemAddress.binaryAddress, remoteSystemAddress.port, suffix);
	}
	else
	{
		sprintf(str, "%sRcv,Raw,NIL,NIL,%s,%i,%i,%u:%i,%u:%i%s", prefix,IDTOString(data[0]), bitsUsed,RakNet::GetTime(),
			localSystemAddress.binaryAddress, localSystemAddress.port, remoteSystemAddress.binaryAddress, remoteSystemAddress.port, suffix);
	}

	AddToLog(str);
}
void PacketLogger::OnInternalPacket(InternalPacket *internalPacket, unsigned frameNumber, SystemAddress remoteSystemAddress, RakNetTime time, bool isSend)
{
	char str[256];
	char sendType[4];
	SystemAddress localSystemAddress;
	localSystemAddress = rakPeer->GetInternalID();

	if (isSend)
		strcpy(sendType, "Snd");
	else
		strcpy(sendType, "Rcv");

	// TODO - put this back in a different form
	/*
	if (internalPacket->isAcknowledgement)
	{
		if (printAcks)
		{
			if (printId==false)
				sprintf(str, "%s,Ack,%5i,%5i,  NIL,    1,%i,%u:%i,%u:%i\n",sendType, internalPacket->messageNumber,frameNumber,time,
				localSystemAddress.binaryAddress, localSystemAddress.port, remoteSystemAddress.binaryAddress, remoteSystemAddress.port);
			else
				sprintf(str, "%s,Ack,%i,%i,NIL,1,%i,%u:%i,%u:%i\n",sendType, internalPacket->messageNumber,frameNumber,time,
				localSystemAddress.binaryAddress, localSystemAddress.port, remoteSystemAddress.binaryAddress, remoteSystemAddress.port);
		}
		else
			str[0]=0;
	}
	else
	*/
	{
		if (internalPacket->data[0]==ID_TIMESTAMP && internalPacket->data[sizeof(unsigned char)+sizeof(RakNetTime)]!=ID_RPC)
		{
			if (printId==false)
			{
				sprintf(str, "%s%s,Tms,%5i,%5i,%5i,%5i,%i,%u:%i,%u:%i%s",prefix,sendType, internalPacket->messageNumber,frameNumber,
					internalPacket->data[1+sizeof(int)], internalPacket->dataBitLength,time,
					localSystemAddress.binaryAddress, localSystemAddress.port, remoteSystemAddress.binaryAddress, remoteSystemAddress.port, suffix);
			}
			else
			{
				sprintf(str, "%s%s,Tms,%i,%i,%s,%i,%i,%u:%i,%u:%i%s",prefix,sendType, internalPacket->messageNumber,frameNumber,
					IDTOString(internalPacket->data[1+sizeof(int)]), internalPacket->dataBitLength,time,
					localSystemAddress.binaryAddress, localSystemAddress.port, remoteSystemAddress.binaryAddress, remoteSystemAddress.port, suffix);
			}

		}
		else if (internalPacket->data[0]==ID_RPC || (internalPacket->dataBitLength>(sizeof(unsigned char)+sizeof(RakNetTime))*8 && internalPacket->data[0]==ID_TIMESTAMP && internalPacket->data[sizeof(unsigned char)+sizeof(RakNetTime)]==ID_RPC))
		{
			const char *uniqueIdentifier = rakPeer->GetRPCString((const char*) internalPacket->data, internalPacket->dataBitLength, isSend==true ? remoteSystemAddress : UNASSIGNED_SYSTEM_ADDRESS);

			
			if (internalPacket->data[0]==ID_TIMESTAMP)
				sprintf(str, "%s%s,RpT,%5i,%5i,%s,%5i,%i,%u:%i,%u:%i%s",prefix,sendType, internalPacket->messageNumber,frameNumber,
				uniqueIdentifier, internalPacket->dataBitLength,time,
				localSystemAddress.binaryAddress, localSystemAddress.port, remoteSystemAddress.binaryAddress, remoteSystemAddress.port, suffix);
			else
				sprintf(str, "%s%s,Rpc,%5i,%5i,%s,%5i,%i,%u:%i,%u:%i%s",prefix,sendType, internalPacket->messageNumber,frameNumber,
				uniqueIdentifier, internalPacket->dataBitLength,time,
				localSystemAddress.binaryAddress, localSystemAddress.port, remoteSystemAddress.binaryAddress, remoteSystemAddress.port, suffix);
		}
		else
		{
			if (printId==false)
			{
				sprintf(str, "%s%s,Nrm,%5i,%5i,%5i,%5i,%i,%u:%i,%u:%i%s",prefix,sendType, internalPacket->messageNumber,frameNumber,
					internalPacket->data[0], internalPacket->dataBitLength,time,
					localSystemAddress.binaryAddress, localSystemAddress.port, remoteSystemAddress.binaryAddress, remoteSystemAddress.port, suffix);
			}
			else
			{
				sprintf(str, "%s%s,Nrm,%i,%i,%s,%i,%i,%u:%i,%u:%i%s",prefix,sendType, internalPacket->messageNumber,frameNumber,
					IDTOString(internalPacket->data[0]), internalPacket->dataBitLength,time,
					localSystemAddress.binaryAddress, localSystemAddress.port, remoteSystemAddress.binaryAddress, remoteSystemAddress.port, suffix);
			}
		}
	}

	AddToLog(str);
}
void PacketLogger::AddToLog(const char *str)
{
	WriteLog(str);
}
void PacketLogger::WriteLog(const char *str)
{
	printf("%s", str);
}
void PacketLogger::SetPrintID(bool print)
{
	printId=print;
}
void PacketLogger::SetPrintAcks(bool print)
{
	printAcks=print;
}
char* PacketLogger::BaseIDTOString(unsigned char Id)
{
	if (Id >= ID_USER_PACKET_ENUM)
        return 0;

	const char *IDTable[(int)ID_USER_PACKET_ENUM]=
	{
		"ID_INTERNAL_PING",
			"ID_PING",
			"ID_PING_OPEN_CONNECTIONS",
			"ID_CONNECTED_PONG",
			"ID_CONNECTION_REQUEST",
			"ID_SECURED_CONNECTION_RESPONSE",
			"ID_SECURED_CONNECTION_CONFIRMATION",
			"ID_RPC_MAPPING",
			"ID_DETECT_LOST_CONNECTIONS",
			"ID_OPEN_CONNECTION_REQUEST",
			"ID_OPEN_CONNECTION_REPLY",
			"ID_RPC",
			"ID_RPC_REPLY",
			"ID_CONNECTION_REQUEST_ACCEPTED",
			"ID_CONNECTION_ATTEMPT_FAILED",
			"ID_ALREADY_CONNECTED",
			"ID_NEW_INCOMING_CONNECTION",
			"ID_NO_FREE_INCOMING_CONNECTIONS",
			"ID_DISCONNECTION_NOTIFICATION",
			"ID_CONNECTION_LOST",
			"ID_RSA_PUBLIC_KEY_MISMATCH",
			"ID_CONNECTION_BANNED",
			"ID_INVALID_PASSWORD",
			"ID_MODIFIED_PACKET",
			"ID_TIMESTAMP",
			"ID_PONG",
			"ID_ADVERTISE_SYSTEM",
			"ID_REMOTE_DISCONNECTION_NOTIFICATION",
			"ID_REMOTE_CONNECTION_LOST",
			"ID_REMOTE_NEW_INCOMING_CONNECTION",
			"ID_DOWNLOAD_PROGRESS",
			"ID_FILE_LIST_TRANSFER_HEADER",
			"ID_FILE_LIST_TRANSFER_FILE",
			"ID_DDT_DOWNLOAD_REQUEST",
			"ID_TRANSPORT_STRING",
			"ID_REPLICA_MANAGER_CONSTRUCTION",
			"ID_REPLICA_MANAGER_DESTRUCTION",
			"ID_REPLICA_MANAGER_SCOPE_CHANGE",
			"ID_REPLICA_MANAGER_SERIALIZE",
			"ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE",
			"ID_CONNECTION_GRAPH_REQUEST",
			"ID_CONNECTION_GRAPH_REPLY",
			"ID_CONNECTION_GRAPH_UPDATE",
			"ID_CONNECTION_GRAPH_NEW_CONNECTION",
			"ID_CONNECTION_GRAPH_CONNECTION_LOST",
			"ID_CONNECTION_GRAPH_DISCONNECTION_NOTIFICATION",
			"ID_ROUTE_AND_MULTICAST",
			"ID_RAKVOICE_OPEN_CHANNEL_REQUEST",
			"ID_RAKVOICE_OPEN_CHANNEL_REPLY",
			"ID_RAKVOICE_CLOSE_CHANNEL",
			"ID_RAKVOICE_DATA",
			"ID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE",
			"ID_AUTOPATCHER_CREATION_LIST",
			"ID_AUTOPATCHER_DELETION_LIST",
			"ID_AUTOPATCHER_GET_PATCH",
			"ID_AUTOPATCHER_PATCH_LIST",
			"ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR",
			"ID_AUTOPATCHER_FINISHED_INTERNAL",
			"ID_AUTOPATCHER_FINISHED",
			"ID_AUTOPATCHER_RESTART_APPLICATION",
			"ID_NAT_PUNCHTHROUGH_REQUEST",
			"ID_NAT_TARGET_NOT_CONNECTED",
			"ID_NAT_TARGET_CONNECTION_LOST",
			"ID_NAT_CONNECT_AT_TIME",
			"ID_NAT_SEND_OFFLINE_MESSAGE_AT_TIME",
			"ID_DATABASE_QUERY_REQUEST",
			"ID_DATABASE_UPDATE_ROW",
			"ID_DATABASE_REMOVE_ROW",
			"ID_DATABASE_QUERY_REPLY",
			"ID_DATABASE_UNKNOWN_TABLE",
			"ID_DATABASE_INCORRECT_PASSWORD",
			"ID_READY_EVENT_SET",
			"ID_READY_EVENT_UNSET",
			"ID_READY_EVENT_ALL_SET",
			"ID_READY_EVENT_QUERY",
	};

	return (char*)IDTable[Id];
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
char* PacketLogger::UserIDTOString(unsigned char Id)
{
	// Users should override this
	return "Unknown";
}
char* PacketLogger::IDTOString(unsigned char Id)
{
	char *out;
	out=BaseIDTOString(Id);
	if (out)
		return (char*)out;
	return UserIDTOString(Id);
}
void PacketLogger::SetPrefix(const char *_prefix)
{
	strncpy(prefix, _prefix, 255);
	prefix[255]=0;
}
void PacketLogger::SetSuffix(const char *_suffix)
{
	strncpy(suffix, _suffix, 255);
	suffix[255]=0;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

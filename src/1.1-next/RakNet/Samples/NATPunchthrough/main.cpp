/// \file
/// \brief Tests the NAT-punchthrough plugin
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

#include "RakPeerInterface.h"
#include "RakSleep.h"
#include <stdio.h>
#include "RakNetworkFactory.h"
#include "NatPunchthrough.h"
#include <string.h>
#include <conio.h>
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakSleep.h"

#include "PacketLogger.h"

#include "GetTime.h"

// This sample starts one of three systems: A facilitator, a recipient, and a sender.  The sender wants to connect to the recipient but
// cannot because both the sender and recipient are behind NATs.  So they both connect to the facilitator first.
// The facilitator will synchronize a countdown timer so that the sender will try to connect to the recipient at the same time
// the recipient will send an offline message to the sender.  This will, in theory, punch through the NAT.
//
// An easy way to know the IPs of other systems is to have all clients connect to a lobby server.
// The lobby server can be as simple as an instance of RakNet using the ConnectionGraph plugin.
// That plugin will cause the lobby server to send ID_REMOTE_* system notifications to its connected systems.
void main(void)
{
	RakPeerInterface *rakPeer=RakNetworkFactory::GetRakPeerInterface();
	NatPunchthrough natPunchthrough;
	char mode[64], recipientIP[64], facilitatorIP[64];
	rakPeer->AttachPlugin(&natPunchthrough);
	printf("Tests the NAT Punchthrough plugin\n");
	printf("Difficulty: Intermediate\n\n");
	printf("Act as (S)ender, (R)ecipient, or (F)acilitator?\n");
	gets(mode);
	if (mode[0]=='s' || mode[0]=='S')
	{
		SocketDescriptor socketDescriptor(60063,0);
		rakPeer->Startup(2,0,&socketDescriptor, 1);
		printf("Enter facilitator IP: ");
		gets(facilitatorIP);
		if (facilitatorIP[0]==0)
			strcpy(facilitatorIP, "127.0.0.1");
		printf("Connecting to facilitator.\n");
		rakPeer->Connect(facilitatorIP, 60065, 0, 0);
	}
	else if (mode[0]=='r' || mode[0]=='R')
	{
		SocketDescriptor socketDescriptor(60064,0);
		rakPeer->Startup(2,0,&socketDescriptor, 1);
		rakPeer->SetMaximumIncomingConnections(1);
		printf("Enter facilitator IP: ");
		gets(facilitatorIP);
		if (facilitatorIP[0]==0)
			strcpy(facilitatorIP, "127.0.0.1");
		printf("Connecting to facilitator.\n");
		rakPeer->Connect(facilitatorIP, 60065, 0, 0);
	}
	else
	{
		SocketDescriptor socketDescriptor(60065,0);
		rakPeer->Startup(2,0,&socketDescriptor, 1);
		rakPeer->SetMaximumIncomingConnections(2);
		printf("Ready.\n");
	}

	printf("Press 'q' to quit.\n");
	Packet *p;
	while (1)
	{
		p=rakPeer->Receive();
		while (p)
		{
			if (p->data[0]==ID_DISCONNECTION_NOTIFICATION)
				printf("ID_DISCONNECTION_NOTIFICATION\n");
			else if (p->data[0]==ID_CONNECTION_LOST)
				printf("ID_CONNECTION_LOST\n");
			else if (p->data[0]==ID_NO_FREE_INCOMING_CONNECTIONS)
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
			else if (p->data[0]==ID_NEW_INCOMING_CONNECTION)
			{
				if (mode[0]=='r' || mode[0]=='R')
					printf("Test successful.  Sender connected.\n");
				else
					printf("ID_NEW_INCOMING_CONNECTION\n");
			}
			else if (p->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
			{
				if (mode[0]=='s' || mode[0]=='S')
				{
					if (p->systemAddress.port==60065)
						printf("Hit 'c' to try to connect through the facilitator\n");
					else
						printf("Test successful.  You are connected to the receiver.\n");
				}
				else
					printf("ID_CONNECTION_REQUEST_ACCEPTED.\n");
			}
			else if (p->data[0]==ID_CONNECTION_ATTEMPT_FAILED)
			{
				if (mode[0]=='s' || mode[0]=='S')
					printf("Test failed.  You did not connect.\n");
				else
					printf("ID_CONNECTION_ATTEMPT_FAILED\n");
			}
			else if (p->data[0]==ID_NAT_TARGET_NOT_CONNECTED)
			{
				SystemAddress systemAddress;
				RakNet::BitStream b(p->data, p->length, false);
				b.IgnoreBits(8); // Ignore the ID_...
				b.Read(systemAddress);
				printf("ID_NAT_TARGET_NOT_CONNECTED to %s\n", systemAddress.ToString());
			}
			else if (p->data[0]==ID_NAT_TARGET_CONNECTION_LOST)
			{
				SystemAddress systemAddress;
				RakNet::BitStream b(p->data, p->length, false);
				b.IgnoreBits(8); // Ignore the ID_...
				b.Read(systemAddress);
				printf("ID_NAT_TARGET_CONNECTION_LOST to %s\n", systemAddress.ToString());
			}

			rakPeer->DeallocatePacket(p);
			p=rakPeer->Receive();
		}

		if (kbhit())
		{
			char ch=getch();
			if (ch=='c' && (mode[0]=='s' || mode[0]=='S'))
			{
				printf("Enter recipient IP: "); 
				gets(recipientIP);
				if (recipientIP[0]==0)
					strcpy(recipientIP, "127.0.0.1");
				// Connect to the recipient by going through the facilitator, which for the purposes of this sample we can
				// assume is the first system connected (GetSystemAddressFromIndex(0))
				natPunchthrough.Connect(recipientIP, 60064, 0, 0, rakPeer->GetSystemAddressFromIndex(0));
				printf("Attempting connection.\n");
			}
			else if (ch=='q')
				break;
		}

		RakSleep(0);
	}

	RakNetworkFactory::DestroyRakPeerInterface(rakPeer);
}

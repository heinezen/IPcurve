// ----------------------------------------------------------------------
// RakNet version 1.0
// Filename ChatExample.cpp
// Created by Rakkar Software (rakkar@rakkarsoftware.com) January 24, 2003
// Very basic chat engine example
// modifyed by romulo fernandes (abra185@gmail.com)
// ----------------------------------------------------------------------

#include "PacketEnumerations.h"
#include "RakNetworkFactory.h"
#include "RakClientInterface.h"
#include "NetworkTypes.h"
#include "BitStream.h"

#include "kbhit.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// Include common enumerations, structures, etc
#include "chat_common.h"

// We copy this from Multiplayer.cpp to keep things all in one file for this example
unsigned char GetPacketIdentifier(Packet *p);

int main(void)
{
	RakNetStatisticsStruct *rss;
	// Pointers to the interfaces of our server and client.
	// Note we can easily have both in the same program
	RakClientInterface *client=RakNetworkFactory::GetRakClientInterface();

	// Holds packets
	Packet* p;

	// GetPacketIdentifier returns this
	unsigned char packetIdentifier;

	// Just so we can remember where the packet came from
	bool isServer;

	// Record the first client that connects to us so we can pass it to the ping function
	PlayerID clientID=UNASSIGNED_PLAYER_ID;

	// Crude interface

	// Holds user data
	char ip[30], serverPort[30], clientPort[30];
	StaticClientDataStruct staticClientData;

	// A client
	isServer=false;

	// Get our input
	puts("Enter the client port to listen on");
	gets(clientPort);
	if (clientPort[0]==0)
		strcpy(clientPort, "5000");
	puts("Enter your name (up to 40 characters)");
	gets(staticClientData.name);
	if (staticClientData.name[0]==0)
		strcpy(staticClientData.name, "Default client name");
	// Note this passes by value, because otherwise people could
	// get access to and damage our internal data
	// UNASSIGNED_PLAYER_ID is to specify changing our own data, rather than our copy of another clients data
	client->SetStaticClientData(UNASSIGNED_PLAYER_ID, (char*)&staticClientData, sizeof(StaticClientDataStruct));
	puts("Enter IP to connect to");
	gets(ip);
	client->AllowConnectionResponseIPMigration(false);
	if (ip[0]==0)
		strcpy(ip, "127.0.0.1");
	
		
	puts("Enter the port to connect to");
	gets(serverPort);
	if (serverPort[0]==0)
		strcpy(serverPort, "10000");

	// Connecting the client is very simple.  0 means we don't care about
	// a connectionValidationInteger, and false for low priority threads
	bool b = client->Connect(ip, atoi(serverPort), atoi(clientPort), 0, 30);
	
	if (b)
		puts("Attempting connection");
	else
	{
		puts("Bad connection attempt.  Terminating.");
		exit(1);
	}

	puts("'quit' to quit. 'stat' to show stats. 'ping' to ping. Type to talk.");
	char message[2048];

	// Loop for input until end of input
	while ( !feof(stdin) )
	{
		// This sleep keeps RakNet responsive
#ifdef _WIN32
		Sleep(30);
#else
		usleep(30 * 1000);
#endif


		if (kbhit())
		{
			// Notice what is not here: something to keep our network running.  It's
			// fine to block on gets or anything we want
			// Because the network engine was painstakingly written using threads.
			gets(message);

			// Clear remaining input
			while (kbhit()) getchar();

			// user typed nothing of interest
			if ( message[0] == 0 )
				continue ;

			if (strcmp(message, "quit")==0)
			{
				puts("Quitting.");
				break;
			}

			if (strcmp(message, "stat")==0)
			{
				
				rss=client->GetStatistics();
				StatisticsToString(rss, message, 2);
				printf("%s", message);
				printf("Ping=%i\n", client->GetAveragePing());
			

				continue;
			}

			if (strcmp(message, "ping")==0)
			{
				if (client->IsConnected())
					client->PingServer();

				continue;
			}

			// message is the data to send
			// strlen(message)+1 is to send the null terminator
			// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
			// RELIABLE_ORDERED means make sure the message arrives in the right order
			client->Send(message, (int) strlen(message)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0);
		}

		// Get a packet from either the server or the client

		p = client->Receive();

	
		if (p==0)
			continue; // Didn't get any packets

		// We got a packet, get the identifier with our handy function
		packetIdentifier = GetPacketIdentifier(p);

		// Check if this is a network message packet
		switch (packetIdentifier)
		{
			case ID_DISCONNECTION_NOTIFICATION:
				  // Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_CONNECTION_LOST\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
				break;
			case ID_REMOTE_EXISTING_CONNECTION: // Server telling you of an existing connection that was there before you connected
				printf("ID_REMOTE_EXISTING_CONNECTION\n");
				break;
			case ID_CONNECTION_BANNED: // Banned from this server
				printf("We are banned from this server.\n");
				break;			
			case ID_RECEIVED_STATIC_DATA:
				 // Got static data
				printf("ID_RECEIVED_STATIC_DATA\n");
	
				printf("Server name: %s\n", ((StaticHostDataStruct*)(client->GetStaticServerData())->GetData())->serverName);
				printf("Server MOTD: %s\n", ((StaticHostDataStruct*)(client->GetStaticServerData())->GetData())->MOTD);
	
				break;

			case ID_NO_FREE_INCOMING_CONNECTIONS:
				 // Sorry, the server is full.  I don't do anything here but
				// A real app should tell the user
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;
			case ID_MODIFIED_PACKET:
				// Cheater!
				printf("ID_MODIFIED_PACKET\n");
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST\n");
				break;

			case ID_CONNECTION_REQUEST_ACCEPTED:
				// This tells the client they have connected
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				break;

			case ID_CONNECTION_ATTEMPT_FAILED:
				// Tells us we were unable to connect
				printf("ID_CONNECT_ATTEMPT_FAILED\n") ;
				break ;

			case ID_PONG:
				// Received a pong from the server
				// TODO: What should I do?
				printf("ID_PONG\n") ;
				break ;

			default:
					// In this simple chat client, we assume that anything else
					// is a chat message. The pitfall to this is that
					// we consume any new/unrecognized packet IDs; this is not
					// recommended behavior in your own clients!

					printf("%s\n", p->data);

				break;
		}


		// We're done with the packet
		client->DeallocatePacket(p);
	}

	// Be nice and let the server know we quit.
	client->Disconnect(300);

	// We're done with the network
	RakNetworkFactory::DestroyRakClientInterface(client);

	return 0;
}

// Copied from Multiplayer.cpp
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
unsigned char GetPacketIdentifier(Packet *p)
{
	if (p==0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else
		return (unsigned char) p->data[0];
}

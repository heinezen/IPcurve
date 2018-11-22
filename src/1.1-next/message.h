#ifndef MESSAGE_H
#define MESSAGE_H

#include "config.h"

#include <raknet/MessageIdentifiers.h>
#include <raknet/RakNetTypes.h>
#include <string>

enum {
	MESSAGE_ERROR = ID_USER_PACKET_ENUM,
	MESSAGE_CONNECT,
	MESSAGE_CHAT,
	MESSAGE_SERVERSYNC,
	MESSAGE_CLIENTSYNC,
	MESSAGE_LOBBYTAKE,
	MESSAGE_LOBBYSTATE,
	MESSAGE_EVENT,
	MESSAGE_REQUEST,
	MESSAGE_GAMEINIT,
	MESSAGE_SCORE,
	MESSAGE_LINE,
	MESSAGE_HELLO,
	MESSAGE_METASERVER_REPLY,
	MESSAGE_METASERVER_REQUEST,
	MESSAGE_METASERVER_ADDSERVER,
	MESSAGE_METASERVER_REMOVESERVER,
	MESSAGE_INFO_REQUEST,
	MESSAGE_INFO_REPLY
};

#define CHAT_BROADCAST 0xFF

#define EVENT_WELCOME 0
#define EVENT_STARTGAME 1
#define EVENT_STARTROUND 2
#define EVENT_ENDROUND 3
#define EVENT_ENDGAME 4

#define REQUEST_STARTGAME 0
#define REQUEST_STARTROUND 1

class MessageBase{
	public:
		unsigned char mtype;
		inline MessageBase(){ mtype = MESSAGE_ERROR; };
};

class MessageConnect:public MessageBase{
	public:
		inline MessageConnect(){ mtype = MESSAGE_CONNECT; };
		char name[MAX_NICK_LENGTH+1];
};

class MessageChat:public MessageBase{
	public:
		inline MessageChat(){ mtype = MESSAGE_CHAT; receiver = CHAT_BROADCAST; };
		char text[256];
		unsigned char receiver;
};

class MessageServerSync:public MessageBase{
	public:
		inline MessageServerSync(){ mtype = MESSAGE_SERVERSYNC; };
		short x[MAX_PLAYERS];
		short y[MAX_PLAYERS];
		unsigned char width[MAX_PLAYERS];
};

class MessageClientSync:public MessageBase{
	public:
		inline MessageClientSync(){ mtype = MESSAGE_CLIENTSYNC; };
		unsigned char keys[MAX_PLAYERS];
};

class MessageLobbyTake:public MessageBase{
	public:
		inline MessageLobbyTake(){ mtype = MESSAGE_LOBBYTAKE; };
		unsigned char takereq[MAX_PLAYERS];
		unsigned char special[MAX_PLAYERS];
};

class MessageLobbyState:public MessageBase{
	public:
		inline MessageLobbyState(){ mtype = MESSAGE_LOBBYSTATE; };
		char controllers[MAX_PLAYERS][MAX_NICK_LENGTH+1];
		char players[MAX_CONNECTIONS][MAX_NICK_LENGTH+1];
		bool yourcontrol[MAX_PLAYERS];
};

class MessageEvent:public MessageBase{
	public:
		inline MessageEvent(){ mtype = MESSAGE_EVENT; };
		unsigned char event;
};

class MessageGameInit:public MessageBase{
	public:
		inline MessageGameInit(){ mtype = MESSAGE_GAMEINIT; };
		short xsize,ysize;
};

class MessageRequest:public MessageBase{
	public:
		inline MessageRequest(){ mtype = MESSAGE_REQUEST; };
		unsigned char request;
};

class MessageScore:public MessageBase{
	public:
		inline MessageScore(){ mtype = MESSAGE_SCORE; };
		signed char scores[MAX_PLAYERS];
		signed char loads[MAX_PLAYERS];
};

class MessageLine:public MessageBase{
	public:
		inline MessageLine(){ mtype = MESSAGE_LINE; };
		short x1,y1,x2,y2;
		unsigned char w,col;
};

class MessageHello:public MessageBase{
	public:
		inline MessageHello(){ mtype = MESSAGE_HELLO; };
		char serverName[MAX_NICK_LENGTH+1];
		short port;
		bool password;
		char desc[MAX_DESC_LENGTH+1];
};

class MessageMetaserverReply:public MessageBase{
	public:
		inline MessageMetaserverReply(){ mtype = MESSAGE_METASERVER_REPLY; };
		char server[128];
		int port;
		bool password;
		char name[MAX_NICK_LENGTH+1];
		char desc[MAX_DESC_LENGTH+1];
};

class MessageMetaserverRequest:public MessageBase{
	public:
		inline MessageMetaserverRequest(){ mtype = MESSAGE_METASERVER_REQUEST; version = VERSION;};
		unsigned short version;
};

class MessageMetaserverAddServer:public MessageBase{
	public:
		inline MessageMetaserverAddServer(){ mtype = MESSAGE_METASERVER_ADDSERVER; version = VERSION;};
		unsigned short version;
		char serverName[MAX_NICK_LENGTH+1];
		bool password;
		short port;
		char desc[MAX_DESC_LENGTH+1];		
};

class MessageMetaserverRemoveServer:public MessageBase{
	public:
		inline MessageMetaserverRemoveServer(){ mtype = MESSAGE_METASERVER_REMOVESERVER; version = VERSION;};
		char serverName[MAX_NICK_LENGTH+1];
		unsigned short version;
};

class MessageInfoRequest:public MessageBase{
	public:
		inline MessageInfoRequest(){ mtype = MESSAGE_INFO_REQUEST;};
		unsigned int timestamp;
};

class MessageInfoReply:public MessageBase{
	public:
		inline MessageInfoReply(){ mtype = MESSAGE_INFO_REPLY;};
		unsigned int timestamp;
		signed char players;
};

/*class MessageMetaserverAck:public MessageBase{
	public:
		inline MessageMetaserverAck(){ mtype = MESSAGE_METASERVER_ACK; version = VERSION;};
		unsigned short version;
};*/
static unsigned char GetPacketIdentifier(Packet *p)
{
	if(!p)
		return 255;

	if((unsigned char)p->data[0] == ID_TIMESTAMP)
		return (unsigned char)p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	else
		return (unsigned char)p->data[0];
}

static std::string GetHost(SystemAddress addr) {
	std::string host =addr.ToString();
	int port_pos = host.rfind(":");
	host = host.substr(0,port_pos);
	return host;
}



static void DumpMessageCodes()
{
	//Debugging: dump all signal codes!
	printf("MESSAGE_ERROR = %d \n"
		   "MESSAGE_CONNECT = %d \n"
		   "MESSAGE_CHAT = %d \n"
		   "MESSAGE_SERVERSYNC = %d \n"
		   "MESSAGE_CLIENTSYNC = %d \n"
		   "MESSAGE_LOBBYTAKE = %d \n"
		   "MESSAGE_LOBBYSTATE = %d \n"
		   "MESSAGE_EVENT = %d \n"
		   "MESSAGE_REQUEST = %d \n"
		   "MESSAGE_GAMEINIT = %d \n"
		   "MESSAGE_SCORE = %d \n"
		   "MESSAGE_LINE = %d \n"
		   "MESSAGE_HELLO = %d \n"
		   "MESSAGE_METASERVER_REPLY = %d \n"
		   "MESSAGE_METASERVER_REQUEST = %d \n"
		   "MESSAGE_METASERVER_ADDSERVER = %d \n"
		   "MESSAGE_METASERVER_REMOVESERVER = %d \n",
			MESSAGE_ERROR,
		    MESSAGE_CONNECT,
		    MESSAGE_CHAT,
		    MESSAGE_SERVERSYNC,
		    MESSAGE_CLIENTSYNC,
		    MESSAGE_LOBBYTAKE,
		    MESSAGE_LOBBYSTATE,
		    MESSAGE_EVENT,
		    MESSAGE_REQUEST,
		    MESSAGE_GAMEINIT,
		    MESSAGE_SCORE,
		    MESSAGE_LINE,
		    MESSAGE_HELLO,
		    MESSAGE_METASERVER_REPLY,
		    MESSAGE_METASERVER_REQUEST,
		    MESSAGE_METASERVER_ADDSERVER,
		    MESSAGE_METASERVER_REMOVESERVER);
}
#endif

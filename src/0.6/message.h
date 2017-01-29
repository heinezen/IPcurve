#ifndef MESSAGE_H
#define MESSAGE_H

#include "config.h"

#define MESSAGE_ERROR 0
#define MESSAGE_CONNECT 1
#define MESSAGE_CHAT 2
#define MESSAGE_SERVERSYNC 3
#define MESSAGE_CLIENTSYNC 4
#define MESSAGE_LOBBYTAKE 5
#define MESSAGE_LOBBYSTATE 6
#define MESSAGE_EVENT 7
#define MESSAGE_REQUEST 8
#define MESSAGE_GAMEINIT 9
#define MESSAGE_SCORE 10
#define MESSAGE_LINE 11
#define MESSAGE_HELLO 12
#define MESSAGE_METASERVER_REPLY 13
#define MESSAGE_METASERVER_REQUEST 14
#define MESSAGE_METASERVER_ADDSERVER 15
#define MESSAGE_METASERVER_REMOVESERVER 16

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
		inline MessageBase(){ mtype = MESSAGE_ERROR; };
		unsigned char mtype;
};

class MessageConnect:public MessageBase{
	public:
		inline MessageConnect(){ mtype = MESSAGE_CONNECT; };
		char name[MAX_NICK_LENGTH+1];
};

class MessageChat:public MessageBase{
	public:
		inline MessageChat(){ mtype = MESSAGE_CHAT; receiver = CHAT_BROADCAST; };
		char text[128];
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
};

class MessageMetaserverReply:public MessageBase{
	public:
		inline MessageMetaserverReply(){ mtype = MESSAGE_METASERVER_REPLY; };
		char server[128];
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
		short port;
};

class MessageMetaserverRemoveServer:public MessageBase{
	public:
		inline MessageMetaserverRemoveServer(){ mtype = MESSAGE_METASERVER_REMOVESERVER; version = VERSION;};
		unsigned short version;
};

#endif

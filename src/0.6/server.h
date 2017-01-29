#ifndef SERVER_H
#define SERVER_H
#include <string>
#include <stdio.h>
#include <enet/enet.h>
#include "config.h"
#include "player.h"
#include "message.h"
#include "settings.h"

class Game;

class CServer{
	public:
		CServer();
		~CServer();
		bool listen(int port);
		bool close();
		void update(int delay = SERVER_TIMEOUT);
		void sync(MessageServerSync* m);
		void sendScoreboard(signed char* scores, signed char* loads);
		void broadcast_chat_msg(std::string m);
		bool isFinished();
		void endGame();
		void reqEndGame();
		void DrawLine(short x1, short y1, short x2, short y2, unsigned char w, unsigned char val);
	private:
		int gameport;
		Game* game;
		std::string servernick;
		std::string metaserver;
		bool finished;
		void sendToMetaserver(MessageBase* mess);
		void startGame();
		void startRound();
		void stopLookup();
		ENetHost* socket;
		ENetHost* lookupSocket;
		void processMessage(int size, void* data, PlayerInfo* info, ENetPeer* peer);
		void lobby_sync();
		void throwEvent(int event, ENetPeer* rec = 0);
		//int id_count;
		int controller[MAX_PLAYERS];
		unsigned char special[MAX_PLAYERS];
		unsigned char botAlg[MAX_PLAYERS];
		bool allowSpecials;
		int roundTimeout;
		int roundWantedDelay;
		double spdMulti;
		
		bool enableHoles;
		int holeDist;
		int holeDistRnd;
		double holeSize;
};
#endif

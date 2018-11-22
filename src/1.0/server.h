#ifndef SERVER_H
#define SERVER_H
#include <string>
#include <map>
#include <stdio.h>

#include <raknet/MessageIdentifiers.h>
#include <raknet/RakNetworkFactory.h>
#include <raknet/RakPeerInterface.h>
#include <raknet/RakNetStatistics.h>
#include <raknet/RakNetTypes.h>
#include <raknet/RakSleep.h>

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
		std::string serverdesc;
		std::string serverpass;
		std::string metaserver;
		bool finished;
		void sendToMetaserver(char* mess, int mess_size);
		void startGame();
		void startRound();
		void stopLookup();
		RakPeerInterface* peer;
		RakPeerInterface* lookupPeer;
		std::map<SystemAddress,PlayerInfo*> player_info;
		void processMessage(Packet* packet);
		void lobby_sync();
		void throwEvent(int event, SystemAddress rec = UNASSIGNED_SYSTEM_ADDRESS);
		//int id_count;
		int controller[MAX_PLAYERS];
		unsigned char special[MAX_PLAYERS];
		unsigned char botAlg[MAX_PLAYERS];
		bool allowSpecials;
		int roundTimeout;
		int roundWantedDelay;
		double spdMulti;
		double turnAngle;
		
		bool enableHoles;
		int holeDist;
		int holeDistRnd;
		double holeSize;
		int metaserverPing;
};
#endif

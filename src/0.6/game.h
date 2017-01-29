#ifndef GAME_H
#define GAME_H
#include <stdio.h>
#include <string>
#include "server.h"
#include "snake.h"
#include "message.h"


#define STATE_NOTSTARTED 0
#define STATE_ENDOFROUND 1
#define STATE_RUNNING 2
#define STATE_ENDOFGAME 3

class Game{
	public:
		Game(CServer* gameserver, int w = STDFIELDWIDTH, int h = STDFIELDHEIGHT);
		~Game();
		
		int getWidth();
		int getHeight();

		CServer* getServer();
		Snake** getSnakes();

		void startRound(int timeout, int wantedDelay);

		void processMessage(MessageClientSync* m, PlayerInfo* player);
		void kickPlayer(PlayerInfo* player);
		unsigned char getState();
		void endGame();
		
		bool addPlayer(Snake* player, int num = -1);
		void updateScores(bool inc = false);
		
		void setBarrier(Snake* snake);
		void openHole(Snake* snake);
		void DrawLine(short x1, short y1, short x2, short y2, short w, unsigned char val);
		unsigned char getMap(int x, int y);
	private:
		void getRanks(unsigned char ranks[]);
		int width,height;
		int playersNow,players;
		unsigned char state;
		unsigned char map[MAXFIELDWIDTH][MAXFIELDHEIGHT];
		CServer* server;
		Snake* snakes[MAX_PLAYERS];
};
#endif
 

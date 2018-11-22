#ifndef SNAKE_H
#define SNAKE_H

#include <string>
#include <stdio.h>
#include "player.h"

#define BORDERSAVE 50

#define MAX_LASTSTAND 10

class Game;

class Snake{
	public:
		Snake(PlayerInfo* control, Game* game, double spd = STDSPEED, int special = ABILITY_NONE, double HoleSize = STDHOLESIZE, 
		      int HoleDist = STDHOLEDIST, int HoleDistRnd = STDHOLEDISTRND, unsigned char Width = STDWIDTH, 
				double rad = STDRAD);
		virtual ~Snake();

		virtual void restart(int rank);
		
		void setNum(unsigned char i);
		unsigned char getNum();
		
		short getX();
		short getY();
		double getAngle();
		unsigned char getWidth();
		
		void setX(short nx);
		void setY(short ny);
		bool getVisible();
		bool isAlive();
		int getSpecial();
		int getSpecialCount();

		virtual void move(int delay);
		void kill();
		virtual void setKeys(unsigned char key);

		PlayerInfo* getController();
		Game* getGame();

		virtual int botAlg();
		
		bool isInLast(short x, short y);
		
		void incScore();
		void setScore(int s);
		int getScore();
		
		bool useSpecial(int ability = -1, bool self = false);
		bool isSpecialInUse(int ability);
		void specialInUse();
		void specialAfterUse();
	protected:
		double speed, radius, holeSize;
		int ability, holeDist, holeDistRnd;
		int score;
		int specialT;
		int specialC;
		int specialcache;
		unsigned char width,num;
		PlayerInfo* safeController;
		PlayerInfo* controller;
		Game* game;

		double x, y, angle;
		int abilityTime, hole;
		unsigned char keys;
		bool alive;
		bool didHole;
		short lastPosX[MAX_LASTSTAND];
		short lastPosY[MAX_LASTSTAND];
		short lastPosW[MAX_LASTSTAND];
		unsigned char lastPosPointer;
};

/** IMPLEMENTATION OF CurveBot **/

class BotSnake:public Snake{
	public:
			BotSnake(PlayerInfo* control, Game* game, double spd = STDSPEED, int special = ABILITY_NONE, double HoleSize = STDHOLESIZE, 
						int HoleDist = STDHOLEDIST, int HoleDistRnd = STDHOLEDISTRND, unsigned char Width = STDWIDTH, 
						double rad = STDRAD);
	protected:
		int scan(double angle, bool snakesOnly = false);
		virtual void botMaybeUseAbility();
		int ArrayMax(int arr[], int size);
		unsigned char getRandomAbility();
};

class NormalBotSnake:public BotSnake{
	public:
		NormalBotSnake(PlayerInfo* control, Game* game, double spd = STDSPEED, int special = ABILITY_NONE, double HoleSize = STDHOLESIZE, 
				   int HoleDist = STDHOLEDIST, int HoleDistRnd = STDHOLEDISTRND, unsigned char Width = STDWIDTH, 
				   double rad = STDRAD);
		virtual ~NormalBotSnake();

		virtual void restart(int rank);
		virtual void move(int delay);
		virtual void setKeys(unsigned char key);
		virtual int botAlg();
	private:
		int stay;
};

class EnhancedBotSnake:public BotSnake{
	public:
		EnhancedBotSnake(PlayerInfo* control, Game* game, double spd = STDSPEED, int special = ABILITY_NONE, double HoleSize = STDHOLESIZE, 
							int HoleDist = STDHOLEDIST, int HoleDistRnd = STDHOLEDISTRND, unsigned char Width = STDWIDTH, 
							double rad = STDRAD);
		virtual ~EnhancedBotSnake();

		virtual void restart(int rank);
		virtual void move(int delay);
		virtual void setKeys(unsigned char key);
		virtual int botAlg();
	private:
		int stay;
};

class ForcefieldBotSnake:public BotSnake{
	public:
		ForcefieldBotSnake(PlayerInfo* control, Game* game, double spd = STDSPEED, int special = ABILITY_NONE, double HoleSize = STDHOLESIZE, 
							  int HoleDist = STDHOLEDIST, int HoleDistRnd = STDHOLEDISTRND, unsigned char Width = STDWIDTH, 
							  double rad = STDRAD);
		virtual ~ForcefieldBotSnake();

		virtual void restart(int rank);
		virtual void move(int delay);
		virtual void setKeys(unsigned char key);
		virtual int botAlg();
	private:
		float forces[MAXFIELDWIDTH][MAXFIELDHEIGHT];
		int stay;
	protected:
		virtual void botMaybeUseAbility();
};

class DumbBotSnake:public BotSnake{
	public:
		DumbBotSnake(PlayerInfo* control, Game* game, double spd = STDSPEED, int special = ABILITY_NONE, double HoleSize = STDHOLESIZE, 
							  int HoleDist = STDHOLEDIST, int HoleDistRnd = STDHOLEDISTRND, unsigned char Width = STDWIDTH, 
							  double rad = STDRAD);
		virtual ~DumbBotSnake();

		virtual void restart(int rank);
		virtual void move(int delay);
		virtual void setKeys(unsigned char key);
		virtual int botAlg();
	private:
		int stay;
};

#endif

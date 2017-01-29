#include <string>
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include "snake.h"
#include "server.h"
#include "game.h"

#define INT(a) (int)floor(a)

Snake::Snake(PlayerInfo* control, Game* g, double spd, int special, double HoleSize,
             int HoleDist, int HoleDistRnd, unsigned char Width, double rad)
{
	safeController = 0;
	controller = control;
	if(controller==0){
		safeController = new PlayerInfo("CURVEBOT",0,-1);
	}
	game = g;

	ability = special;

	holeSize = HoleSize;
	holeDist = HoleDist;
	holeDistRnd = HoleDistRnd;
	radius = rad;
	speed = spd;
	width = Width;
	lastPosPointer = 0;
	score = 0;
	didHole = false;
	specialT = 0;
	specialC = 0;
	num = 0xFF;
}

Snake::~Snake()
{
	if(safeController)
		delete safeController;
}

void Snake::restart(int rank)
{
	x = (rand()/(RAND_MAX-1.0))*(game->getWidth()-BORDERSAVE*2)+BORDERSAVE;
	y = (rand()/(RAND_MAX-1.0))*(game->getHeight()-BORDERSAVE*2)+BORDERSAVE;
	angle = (rand()/(RAND_MAX-1.0))*6.28;
	hole = (int)floor(-(rand()/(RAND_MAX-1.0))*holeDistRnd)-holeDist;
	alive = true;
	keys = 0;
	specialC = rank;
	if(specialT>0||ability==ABILITY_NOHOLE){
		specialT = 0;
		specialAfterUse();
	}
	specialcache = 0;
	for(int i=0;i<MAX_LASTSTAND;i++){
		lastPosX[i] = getX();//+(int)floor(i*width/MAX_LASTSTAND);
		lastPosY[i] = getY();//+(int)floor(i*width/MAX_LASTSTAND);
		lastPosW[i] = width;
	}
	lastPosPointer = 0;
}

void Snake::setX(short nx)
{
	/*x=nx-width/2;
	if(x<width/2)
		x=width/2;
	if(x>(game->getWidth()-width/2-1))
		x=game->getWidth()-width/2-1;*/
	x = nx;
}

void Snake::setY(short ny)
{
	/*y=ny-width/2;
	if(y<width/2)
		y=width/2;
	if(y>(game->getHeight()-width/2-1))
		y=game->getHeight()-width/2-1;*/
	y = ny;
}

short Snake::getX()
{
	return (short)floor(x);
}

short Snake::getY()
{
	return (short)floor(y);
}
double Snake::getAngle()
{
	return angle;
}

bool Snake::getVisible()
{
	return (hole<0);
}

bool Snake::isAlive()
{
	return alive;
}

bool Snake::useSpecial(int special, bool self)
{
	if(ability==ABILITY_NOHOLE&&specialT>0&&specialT<300)
		specialAfterUse();
	if(ability==ABILITY_NONE)
		return false;
	if(ability==ABILITY_WALLS&&self)
		return false;
	if(ability!=special&&special!=-1)
		return false;
	if(specialT!=0)
		return false;
	if(specialC<1&&(ability!=ABILITY_NOHOLE))
		return false;
	specialC--;
	switch(ability){
		case ABILITY_ACCELERATE:
			specialT = ABI_ACCEL_LENGTH;
			speed += 2;
		break;
		case ABILITY_RADIUS:
			specialT = ABI_RADIUS_LENGTH;
			speed /= 3;
			radius *= 3;
		break;
		case ABILITY_JUMP:
			specialT = ABI_JUMP_PERIOD;
			x += cos(angle)*ABI_JUMP_LENGTH;
			y += sin(angle)*ABI_JUMP_LENGTH;
			didHole=true;
			break;
		case ABILITY_MAKEHOLE:
			specialT = ABI_MAKEHOLE_PERIOD;
			game->openHole(this);
		break;
		case ABILITY_NOHOLE:
			specialT = 1000;
			specialC = 1;
		break;
		case ABILITY_BARRIER:
			specialT = ABI_BARRIER_PERIOD;
			game->setBarrier(this);
		break;
		case ABILITY_90DEGS:
			specialT = ABI_90DEGS_LENGTH;
		break;
		case ABILITY_WIDTH:
			specialT = ABI_WIDTH_LENGTH;
			specialcache = width;
		break;
		case ABILITY_INVISIBLE:
			specialT = ABI_INVISIBLE_LENGTH;
		break;
	}
	game->updateScores();
	return true;
}
void Snake::specialAfterUse()
{
	switch(ability){
		case ABILITY_ACCELERATE:
			speed -= 2;
		break;
		case ABILITY_RADIUS:
			speed *= 3;
			radius /= 3;
		break;
		case ABILITY_WALLS:
			didHole=true;
			lastPosX[lastPosPointer] = getX();
			lastPosY[lastPosPointer] = getY();
			lastPosW[lastPosPointer++] = width;
			if(lastPosPointer>=MAX_LASTSTAND)
				lastPosPointer = 0;
		break;
		case ABILITY_NOHOLE:
			hole = -holeDist-(int)floor((rand()/(RAND_MAX-1.0)-0.5)*holeDistRnd);
			specialC = 0;
			specialT = 1;
			game->updateScores();
			specialT = -500;
		break;
		case ABILITY_WIDTH:
			width = specialcache;
			hole = -holeDist-(int)floor((rand()/(RAND_MAX-1.0)-0.5)*holeDistRnd);
		break;
		case ABILITY_INVISIBLE:
			hole = -holeDist-(int)floor((rand()/(RAND_MAX-1.0)-0.5)*holeDistRnd);
		break;
	}
}
void Snake::specialInUse()
{
	switch(ability){
		case ABILITY_NOHOLE:
			if(specialT<200)
				specialT = 200;
			hole = -1000;
		break;
		case ABILITY_INVISIBLE:
			hole = -1000;
		break;
		case ABILITY_WIDTH:
			hole = -1000;
			if(specialT>ABI_WIDTH_LENGTH/2)
				width = specialcache+(ABI_WIDTH_MAXWIDTH*(ABI_WIDTH_LENGTH-specialT))/(ABI_WIDTH_LENGTH/2);
			else
				width = specialcache+(ABI_WIDTH_MAXWIDTH*specialT)/(ABI_WIDTH_LENGTH/2);
		break;
		case ABILITY_90DEGS:
			if(keys&KEY_LEFT){
				angle -= 1.5;
				specialT = 0;
			}
			if(keys&KEY_RIGHT){
				angle += 1.5;
				specialT = 0;
			}
		break;
	}
}

void Snake::setNum(unsigned char i)
{
	num = i;
}

unsigned char Snake::getNum()
{
	return num;
}

bool Snake::isSpecialInUse(int ability)
{
	return(this->ability==ability&&specialT>0);
}

void Snake::move(int delay)
{
	didHole = false;
	if(keys&KEY_LEFT) angle -= radius*speed;
	if(keys&KEY_RIGHT) angle += radius*speed;
	if(keys&KEY_SPECIAL) useSpecial(-1,true);
	if(hole<0){
		hole += (int)floor(40*speed);
		if(hole>=0){
			hole = (int)floor(holeSize*20);
		}
	}else{
		hole -= (int)floor(40*speed);
		if(hole<=0){
			hole = -holeDist-(int)floor((rand()/(RAND_MAX-1.0)-0.5)*holeDistRnd);
		}
		game->DrawLine(lastPosX[(lastPosPointer+7)%MAX_LASTSTAND],
							lastPosY[(lastPosPointer+7)%MAX_LASTSTAND],
							lastPosX[(lastPosPointer+7)%MAX_LASTSTAND],
							lastPosY[(lastPosPointer+7)%MAX_LASTSTAND],
							(lastPosW[(lastPosPointer+7)%MAX_LASTSTAND]>width?
							 lastPosW[(lastPosPointer+7)%MAX_LASTSTAND]:width)+5/*+6*/,
							0xFF);
	}
	x += cos(angle)*speed;
	y += sin(angle)*speed;
	if(specialT>0){
		specialT -= (int)floor(40*speed);
		specialInUse();
		if(specialT<=0){
			specialT = 0;
			specialAfterUse();
			specialcache = 0;
		}
	}
	if(specialT<0){
		specialT += (int)floor(40*speed);
		if(specialT>=0){
			specialT = 0;
		}
	}
	lastPosX[lastPosPointer] = getX();
	lastPosY[lastPosPointer] = getY();
	lastPosW[lastPosPointer++] = width;
	if(lastPosPointer>=MAX_LASTSTAND)
		lastPosPointer = 0;
}

void Snake::incScore()
{
	score++;
}

void Snake::setScore(int s)
{
	score = s;
}

int Snake::getScore()
{
	return score;
}

void Snake::kill()
{
	alive = false;
}

void Snake::setKeys(unsigned char key)
{
	keys = key;
}

int Snake::getSpecial()
{
	return ability;
}

int Snake::getSpecialCount()
{
	return specialC;
}

PlayerInfo* Snake::getController()
{
	return controller!=0?controller:safeController;
}

Game* Snake::getGame()
{
	return game;
}

int Snake::botAlg()
{
	return BOTALG_NOBOT;
}

bool Snake::isInLast(short x, short y)
{
	for(int i=0;i<MAX_LASTSTAND;i++){
		int ww = width>lastPosW[i]?width:lastPosW[i];
		if(i==((lastPosPointer+MAX_LASTSTAND-1)%MAX_LASTSTAND))
			continue;
		if(x<lastPosX[i]+ww/2&&y<lastPosY[i]+ww/2&&x>=lastPosX[i]-ww/2&&y>=lastPosY[i]-ww/2)
			return true;
	}
	return false;
}

unsigned char Snake::getWidth()
{
	return didHole?0:width;
}



/** -------------------------- IMPLEMENTATION OF CurveBot ------------------------------------- **/
int BotSnake::scan(double angle,bool snakesOnly)
{
	int res = 0;
	double deltax = cos(this->angle+angle)*4, deltay = sin(this->angle+angle)*4;
	double tx = x+deltax, ty = y+deltay;
	bool b = true;
	int i,j,ix,iy;
	while(b&&res<1000){
		res += 4;
		tx += deltax;
		ty += deltay;
		if(ability==ABILITY_WALLS&&specialC>0){
			if(tx<=3)
				tx = game->getWidth()-4;
			if(ty<=3)
				ty = game->getHeight()-4;
			if(tx>=game->getWidth()-3)
				tx = 4;
			if(ty>=game->getHeight()-3)
				ty = 4;
		}
		ix = (int)floor(tx);
		iy = (int)floor(ty);
		for(i=-2-width/2;i<width/2+2&&b;i++){
			for(j=-2-width/2;j<width/2+2&&b;j++){
				if(snakesOnly)
					b &= game->getMap(ix+i,iy+j)==0xFF||game->getMap(ix+i,iy+j)==0xFE;
				else
					b &= game->getMap(ix+i,iy+j)==0xFF;
			}
		}
	}
	return res;
}

int BotSnake::ArrayMax(int arr[], int size)
{
	int max = -0xFFFF;
	int maxp = -1;
	for(int i=0;i<size;i++){
		if(max<arr[i]){
			max = arr[i];
			maxp = i;
		}
	}
	return maxp;
}

void BotSnake::botMaybeUseAbility()
{
	Snake** snakes = game->getSnakes();
	
	if(ability==ABILITY_BARRIER||ability==ABILITY_ACCELERATE){
		double dx,dy;
		for(int i=0;i<MAX_PLAYERS;i++){
			if(i==num)
				continue;
			if(snakes[i]==0)
				continue;
			if(!snakes[i]->isAlive())
				continue;
			dx = snakes[i]->getX()-getX();
			dy = snakes[i]->getY()-getY();
			dx *= dx;
			dy *= dy;
			dx += dy;
			if(dx<500){
				keys |= KEY_SPECIAL;
				return;
			}
		}
	}else if(ability==ABILITY_MAKEHOLE){
		int dist = scan(0,true);
		if(dist<30){
			keys |= KEY_SPECIAL;
		}
	}else if(ability==ABILITY_JUMP){
		int dist = scan(0,true);
		if(dist<5){
			keys |= KEY_SPECIAL;
		}
	}
}

unsigned char BotSnake::getRandomAbility()
{
	double d = ((double)rand()/(RAND_MAX+1.0))*5;
	printf("%f\n",d);
	unsigned char r = (int)floor(d)+1;
	if(r==ABILITY_RADIUS)
		r = ABILITY_WALLS;
	return r;
	//return ABILITY_BARRIER;
}

BotSnake::BotSnake(PlayerInfo* control, Game* game, double spd, int special, double HoleSize, 
										 int HoleDist, int HoleDistRnd, unsigned char Width, double rad)
	:Snake(control,game,spd,special,HoleSize,HoleDist,HoleDistRnd,Width,rad)
{
	if(ability==ABILITY_BOTSCHOICE)
		ability = getRandomAbility();
}

/** CurveBot, NORMAL **/

/* The NORMAL Bot as seen in kurveSDL. This bot scans some angles to measure the distance to the next obstacle
   in this direction. Then it steers to the direction with the biggest distance. A set of special rules prevent the
   bot form turning into walls. */

NormalBotSnake::NormalBotSnake(PlayerInfo* control, Game* game, double spd, int special, double HoleSize, 
					int HoleDist, int HoleDistRnd, unsigned char Width, double rad)
	:BotSnake(control,game,spd,special,HoleSize,HoleDist,HoleDistRnd,Width,rad)
{
	stay = 0;
}

NormalBotSnake::~NormalBotSnake()
{
}

void NormalBotSnake::restart(int rank)
{
	Snake::restart(rank);
	stay = 0;
}

#define BOTALG_NORMAL_NUMANGLES 9
#define BOTALG_NORMAL_STAY 200

void NormalBotSnake::move(int delay)
{
	double angles[BOTALG_NORMAL_NUMANGLES] = {0,110*RAD,-110*RAD,96*RAD,-96*RAD,42*RAD,-42*RAD,18*RAD,-18*RAD};
	int scans[BOTALG_NORMAL_NUMANGLES];
	if(stay<=0){
		for(int i=0;i<BOTALG_NORMAL_NUMANGLES;i++){
			scans[i] = scan(angles[i]);
		}
		// special rules to enhance the bot a little bit
		if(scans[8]<15) scans[4] = 1;
		if(scans[7]<15) scans[3] = 1;
		if(scans[0]<10) scans[4] = 1;
		if(scans[0]<10) scans[3] = 1;
		if(scans[5]<30) scans[4] = 1;
		if(scans[6]<30) scans[3] = 1;

		if(scans[8]<50) scans[2] = 1;
		if(scans[7]<50) scans[1] = 1;
		if(scans[0]<20) scans[2] = 1;
		if(scans[0]<20) scans[1] = 1;
		if(scans[0]>200) scans[2] = 1;
		if(scans[0]>200) scans[1] = 1;
		if(scans[5]<60) scans[2] = 1;
		if(scans[6]<60) scans[1] = 1;
		if(scans[3]<86) scans[2] = 1;
		if(scans[4]<86) scans[1] = 1;
		
		if(scans[7]<10) scans[6] = 1;
		if(scans[8]<10) scans[5] = 1;
		if((scans[0]>250)&&(scans[7]>100)&&(scans[8]>100)) 
			scans[0] = 0xFFFF;
		int dir = ArrayMax(scans,BOTALG_NORMAL_NUMANGLES);
		stay = BOTALG_NORMAL_STAY;
		if(dir==0){
			keys = 0;
		}else if(dir%2){
			keys = KEY_RIGHT;
		}else{
			keys = KEY_LEFT;
		}
	}else{
		stay -= (int)floor(40*speed);
	}
	botMaybeUseAbility();
	Snake::move(delay);
}

void NormalBotSnake::setKeys(unsigned char key){}

int NormalBotSnake::botAlg()
{
	return BOTALG_NORMAL;
}


/** CurveBot, ENHANCED **/

/* The ENHANCED Bot as seen in kurveSDL. Scanning/Measuring works like in the NORMAL bot, but this one
   has a better set of special rules */

EnhancedBotSnake::EnhancedBotSnake(PlayerInfo* control, Game* game, double spd, int special, double HoleSize, 
										 int HoleDist, int HoleDistRnd, unsigned char Width, double rad)
	:BotSnake(control,game,spd,special,HoleSize,HoleDist,HoleDistRnd,Width,rad)
{
	stay = 0;
}

EnhancedBotSnake::~EnhancedBotSnake()
{
}

void EnhancedBotSnake::restart(int rank)
{
	Snake::restart(rank);
	stay = 0;
}

#define BOTALG_ENHANCED_NUMANGLES 9
#define BOTALG_ENHANCED_STAY 100

void EnhancedBotSnake::move(int delay)
{
	double angles[BOTALG_ENHANCED_NUMANGLES] = {0,110*RAD,-110*RAD,60*RAD,-60*RAD,33*RAD,-33*RAD,16*RAD,-16*RAD};
	int scans[BOTALG_ENHANCED_NUMANGLES];
	if(stay<=0){
		for(int i=0;i<BOTALG_ENHANCED_NUMANGLES;i++){
			scans[i] = scan(angles[i]);
		}
		
		// special rules, EnhancedBot's core
		if(scans[6]<3) scans[7] = 1;
		if(scans[5]<3) scans[8] = 1;
		if(scans[8]<10) scans[4] = 1;
		if(scans[7]<10) scans[3] = 1;
		if(scans[5]<25) scans[4] = 1;
		if(scans[6]<25) scans[3] = 1;
		if(scans[8]<10) scans[2] = 1;
		if(scans[7]<10) scans[1] = 1;
		if(scans[5]<30) scans[2] = 1;
		if(scans[6]<30) scans[1] = 1;
		if(scans[3]<44) scans[2] = 1;
		if(scans[4]<44) scans[1] = 1;
		if(scans[8]<8) scans[6] = 1;
		if(scans[7]<8) scans[5] = 1;
		scans[1] -= 40;
		scans[2] -= 40;
		
		if(keys&KEY_LEFT){
			scans[1] -= 350;
			scans[3] -= 180;
			scans[5] -= 30;
			scans[7] -= 15;
		}

		if(keys&KEY_RIGHT){
			scans[2] -= 350;
			scans[4] -= 180;
			scans[6] -= 30;
			scans[8] -= 15;
		}

		if(scans[0]>120&&(scans[7]>100||scans[8]>100))
			scans[0] += 50;
		
		
		int dir = ArrayMax(scans,BOTALG_ENHANCED_NUMANGLES);
		if(dir==1||dir==2)
			stay = 3*BOTALG_ENHANCED_STAY;
		else if(dir==3||dir==4)
			stay = 2*BOTALG_ENHANCED_STAY;
		else if(dir==5||dir==6)
			stay = BOTALG_ENHANCED_STAY;
		else
			stay = 0;
		
		if(dir==0){
			keys = 0;
		}else if(dir%2){
			keys = KEY_RIGHT;
		}else{
			keys = KEY_LEFT;
		}
	}else{
		stay -= (int)floor(40*speed);
	}
	botMaybeUseAbility();
	Snake::move(delay);
}

void EnhancedBotSnake::setKeys(unsigned char key){}

int EnhancedBotSnake::botAlg()
{
	return BOTALG_ENHANCED;
}

/** CurveBot, MR. FORCE **/

/* Ok, this is a bot that was not seen before (in "kurveSDL" or "Achtung, d Kurve"),
   it's based on 2D-Forcefields and should play reasonably well. (In BOTvsBOT it's
   the bot in average, even slightly better than ENHANCED) */

#define BOTALG_FORCEFIELD_MAX_FORCE 1000

ForcefieldBotSnake::ForcefieldBotSnake(PlayerInfo* control, Game* game, double spd, int special, double HoleSize, 
											  int HoleDist, int HoleDistRnd, unsigned char Width, double rad)
	:BotSnake(control,game,spd,special,HoleSize,HoleDist,HoleDistRnd,Width,rad)
{
	if(ability==ABILITY_WALLS)
		ability = ABILITY_RADIUS;
}

ForcefieldBotSnake::~ForcefieldBotSnake()
{
}

void ForcefieldBotSnake::botMaybeUseAbility()
{
	if(ability==ABILITY_RADIUS||ability==ABILITY_90DEGS){
		float force = forces[lastPosX[lastPosPointer]][lastPosY[lastPosPointer]];
		if(force>BOTALG_FORCEFIELD_MAX_FORCE/2){
			keys |= KEY_SPECIAL;
			return;
		}
	}
	BotSnake::botMaybeUseAbility();
}

void ForcefieldBotSnake::restart(int rank)
{
	Snake::restart(rank);
	stay = 0;
	float mx = MAXFIELDWIDTH/2.0, my = MAXFIELDHEIGHT/2.0;
	float dx,dy;
	int xx,yy;
	for(xx=0;xx<MAXFIELDWIDTH;xx++){
		for(yy=0;yy<MAXFIELDHEIGHT;yy++){
			dx = my;
			dy = mx;
			if(xx>mx){
				dx = (MAXFIELDWIDTH-xx);
			//	dx *= 2;
			}else if(xx<mx){
				dx = (xx);
		//		dx *= 2;
			}
			if(yy>my){
				dy = (MAXFIELDHEIGHT-yy);
	//			dy *= 2;
			}else if(yy<my){
				dy = (yy);
//				dy *= 2;
			}
			if(dx==0||dy==0){
				forces[xx][yy] = BOTALG_FORCEFIELD_MAX_FORCE*6.6;
			}else{
				forces[xx][yy] = ((1/dx+1/dy)*BOTALG_FORCEFIELD_MAX_FORCE)*6.6;
			}
		}
	}
}

void ForcefieldBotSnake::move(int delay)
{
	Snake** snakes = game->getSnakes();
	float dx,dy;
	int xx,yy,i;
	int snakex,snakey,ssx,ssy,tsx,tsy;
	for(i=0;i<MAX_PLAYERS;i++){
		if(snakes[i]==0)
			continue;
		if(i==num){
			snakex = lastPosX[lastPosPointer];
			snakey = lastPosY[lastPosPointer];
		}else{
			snakex = snakes[i]->getX();
			snakey = snakes[i]->getY();
		}
		if(snakex<0||snakey<0)
			continue;
		ssx = snakex-50;
		tsx = snakex+50;
		ssy = snakey-50;
		tsy = snakey+50;
		if(ssx<0)
			ssx = 0;
		if(ssy<0)
			ssy = 0;
		if(tsx>MAXFIELDWIDTH)
			tsx = MAXFIELDWIDTH;
		if(tsy>MAXFIELDHEIGHT)
			tsy = MAXFIELDHEIGHT;
		for(xx=ssx;xx<tsx;xx++){
			for(yy=ssy;yy<tsy;yy++){
				dx = fabs(xx-snakex);//*1.0;//5.5;//((i==num)?7.0:10.0);///2.3;//*200.0/MAXFIELDWIDTH;
				dy = fabs(yy-snakey);//*1.0;//5.5;//((i==num)?7.0:10.0);///2.3;//*200.0/MAXFIELDHEIGHT;
				dx *= dx;
				dy *= dy;
				dx += dy;
				dx = sqrt(dx)*4.5;
				if(dx==0)
					forces[xx][yy] += BOTALG_FORCEFIELD_MAX_FORCE;
				else
					forces[xx][yy] += (1/dx)*BOTALG_FORCEFIELD_MAX_FORCE;
			}
		}
	}
	if(stay<=0){
		int lx,ly,mx,my,mxp,myp,rx,ry;
		mx = getX();
		my = getY();
	
		lx = mx + (int)floor(cos(angle-1.57)*speed);
		ly = my + (int)floor(sin(angle-1.57)*speed);
		rx = mx + (int)floor(cos(angle+1.57)*speed);
		ry = my + (int)floor(sin(angle+1.57)*speed);
		mx = mx + (int)floor(cos(angle)*speed);
		my = my + (int)floor(sin(angle)*speed);
		mxp = mx + (int)floor(cos(angle)*speed*55);
		myp = my + (int)floor(sin(angle)*speed*55);
		lx = lx<0?0:lx; lx = lx<MAXFIELDWIDTH?lx:MAXFIELDWIDTH-1;
		ly = ly<0?0:ly; ly = ly<MAXFIELDHEIGHT?ly:MAXFIELDHEIGHT-1;
		mx = mx<0?0:mx; mx = mx<MAXFIELDWIDTH?mx:MAXFIELDWIDTH-1;
		my = my<0?0:my; my = my<MAXFIELDHEIGHT?my:MAXFIELDHEIGHT-1;
		mxp = mxp<0?0:mxp; mxp = mxp<MAXFIELDWIDTH?mxp:MAXFIELDWIDTH-1;
		myp = myp<0?0:myp; myp = myp<MAXFIELDHEIGHT?myp:MAXFIELDHEIGHT-1;
		rx = rx<0?0:rx; rx = rx<MAXFIELDWIDTH?rx:MAXFIELDWIDTH-1;
		ry = ry<0?0:ry; ry = ry<MAXFIELDHEIGHT?ry:MAXFIELDHEIGHT-1;
	
		float fl=forces[lx][ly],fm=forces[mx][my],fr=forces[rx][ry];
		if(forces[mxp][myp]>BOTALG_FORCEFIELD_MAX_FORCE){
			fm += 10*BOTALG_FORCEFIELD_MAX_FORCE;
			stay = 100;
		}
		keys = 0;
		if(fl<fr&&fl<fm){
			keys = KEY_LEFT;
		}else	if(fr<fm){
			keys = KEY_RIGHT;
		}
	}else{
		stay -= (int)floor(40*speed);
	}
	Snake::move(delay);
	botMaybeUseAbility();
}

void ForcefieldBotSnake::setKeys(unsigned char key){}

int ForcefieldBotSnake::botAlg()
{
	return BOTALG_FORCEFIELD;
}


/** CurveBot, DUMB **/

/* DUMB bot of "Achtung d Kurve". Uses a fixed coordinate system. */

DumbBotSnake::DumbBotSnake(PlayerInfo* control, Game* game, double spd, int special, double HoleSize, 
										 int HoleDist, int HoleDistRnd, unsigned char Width, double rad)
	:BotSnake(control,game,spd,special,HoleSize,HoleDist,HoleDistRnd,Width,rad)
{
	if(ability==ABILITY_BOTSCHOICE)
		ability = ABILITY_NONE;
}

DumbBotSnake::~DumbBotSnake()
{
}

void DumbBotSnake::restart(int rank)
{
	Snake::restart(rank);
	stay = 0;
}

#define BOTALG_DUMB_NUMANGLES 8
#define BOTALG_DUMB_STAY 2000

void DumbBotSnake::move(int delay)
{
	if(stay<=0){
		double angles[BOTALG_DUMB_NUMANGLES] = {0,45*RAD,90*RAD,135*RAD,180*RAD,225*RAD,270*RAD,315*RAD};
		int scans[BOTALG_DUMB_NUMANGLES];
		for(int i=0;i<BOTALG_DUMB_NUMANGLES;i++){
			scans[i] = scan(angles[i]-angle);
		}
	
		keys = 0;
		int dir = ArrayMax(scans,BOTALG_DUMB_NUMANGLES);
		if(fabs(angles[dir]-angle)>3.14){
			if(angle>angles[dir])
				angles[dir] += 3.14;
			if(angle<angles[dir])
				angles[dir] -= 3.14;
		}
		if(angle<angles[dir])
			keys = KEY_RIGHT;
		else if(angle>angles[dir])
			keys = KEY_LEFT;
		if(fabs(angles[dir]-angle)>1.4){
			stay = BOTALG_DUMB_STAY;
		}
	}else{
		stay -= (int)floor(40*speed);
	}
	botMaybeUseAbility();
	Snake::move(delay);
}

void DumbBotSnake::setKeys(unsigned char key){}

int DumbBotSnake::botAlg()
{
	return BOTALG_DUMB;
}



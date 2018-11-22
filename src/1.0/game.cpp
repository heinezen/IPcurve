#include <math.h>
#include <stdio.h>
#include <string>
#include <SDL/SDL.h>
#include "game.h"
#include "server.h"
#include "config.h"
#include "snake.h"
#include "text.h"
#include "utils.h"

void Game::setBarrier(Snake* snake)
{
	int xx = snake->getX()-(int)floor(cos(snake->getAngle())*10);
	int yy = snake->getY()-(int)floor(sin(snake->getAngle())*10);
	int x1 = xx-(int)floor(cos(snake->getAngle()+1.57)*ABI_BARRIER_LENGTH);
	int y1 = yy-(int)floor(sin(snake->getAngle()+1.57)*ABI_BARRIER_LENGTH);
	int x2 = xx+(int)floor(cos(snake->getAngle()+1.57)*ABI_BARRIER_LENGTH);
	int y2 = yy+(int)floor(sin(snake->getAngle()+1.57)*ABI_BARRIER_LENGTH);
	int ww = snake->getWidth();
	DrawLine(x1,y1,x2,y2,ww,snake->getNum());
}

void Game::openHole(Snake* snake)
{
	int x1 = snake->getX()+(int)floor(cos(snake->getAngle())*10);
	int y1 = snake->getY()+(int)floor(sin(snake->getAngle())*10);
	int x2 = snake->getX()+(int)floor(cos(snake->getAngle())*ABI_MAKEHOLE_LENGTH);
	int y2 = snake->getY()+(int)floor(sin(snake->getAngle())*ABI_MAKEHOLE_LENGTH);
	int ww = snake->getWidth()+8;
	DrawLine(x1,y1,x2,y2,ww,0xFF);
}

void Game::DrawLine(short x1, short y1, short x2, short y2, short w, unsigned char val)
{
	server->DrawLine(x1,y1,x2,y2,w,val);
	int i, j;
	if(x1==x2&&y1==y2){
		for(i=0;i<w;i++){
			for(j=0;j<w;j++){
				int tx = x1 + i - w/2;
				int ty = y1 + j - w/2;
				if(tx>0&&tx<width&&ty>0&&ty<height)
					map[tx][ty] = val;
			}
		}
	}
	int dx = x2-x1;
	int dy = y2-y1;
	int sdx = dx<0?-1:1;
	int sdy = dy<0?-1:1;
	dx = sdx*dx+1;
	dy = sdy*dy+1;
	int x = 0;
	int y = 0;
	int px = x1;
	int py = y1;
	if(dx >= dy){
		for(x=0;x<dx;x++){
			for(i=0;i<w;i++){
				for(j=0;j<w;j++){
					int tx = px + i - w/2;
					int ty = py + j - w/2;
					if(tx>0&&tx<width&&ty>0&&ty<height)
						map[tx][ty] = val;
				}
			}
			y += dy;
			if(y>=dx){
				y -= dx;
				py += sdy;
			}
			px += sdx;
		}
	}else{
		for(y=0;y<dy;y++){
			for(i=0;i<w;i++){
				for(j=0;j<w;j++){
					int tx = px + i - w/2;
					int ty = py + j - w/2;
					if(tx>0&&tx<width&&ty>0&&ty<height)
						map[tx][ty] = val;
				}
			}
			x += dx;
			if(x>=dy){
				x -= dy;
				px += sdx;
			}
			py += sdy;
		}
	}
}

Game::Game(CServer* gameserver, int w, int h)
{
	srand((unsigned)time(0));
	server = gameserver;
	width = w;
	height = h;
	state = STATE_NOTSTARTED;
	players = 0;
	bots = 0;
}

Game::~Game()
{
	for(int i=0;i<MAX_PLAYERS;i++){
		if(snakes[i] != 0)
			delete snakes[i];
	}
}

int Game::getWidth()
{
	return width;
}

int Game::getHeight()
{
	return height;
}

CServer* Game::getServer()
{
	return server;
}


void Game::updateScores(bool inc)
{
	signed char scoret[MAX_PLAYERS];
	signed char loadst[MAX_PLAYERS];
	for(int j=0;j<MAX_PLAYERS;j++){
		if(snakes[j]==0){
			scoret[j] = -1;
			loadst[j] = -1;
			continue;
		}
		if(!snakes[j]->isAlive()){
			scoret[j] = snakes[j]->getScore();
			loadst[j] = (snakes[j]->getSpecial()>0)?snakes[j]->getSpecialCount():-1;
		}else{
			if(inc)
				snakes[j]->incScore();
			scoret[j] = snakes[j]->getScore();
			loadst[j] = (snakes[j]->getSpecial()>0)?snakes[j]->getSpecialCount():-1;
		}
	}
	//sort(scoret,0,MAX_PLAYERS);
	server->sendScoreboard(scoret,loadst);
}

void Game::endGame()
{
	state = STATE_ENDOFGAME;
}

void Game::getRanks(unsigned char ranks[])
{
	int i,j,m;
	signed char scoret[MAX_PLAYERS];
	for(j=0;j<MAX_PLAYERS;j++){
		ranks[j] = 0;
		if(snakes[j]==0)
			scoret[j] = -1;
		else scoret[j] = snakes[j]->getScore();
	}
	for(i=0;i<MAX_PLAYERS;i++){
		m = MAX_PLAYERS;
			for(j=0;j<MAX_PLAYERS;j++){
				if(scoret[i]>=scoret[j])
					m--;
			}
		ranks[i]=m;
	}
}

void Game::startRound(int timeout, int wantedDelay)
{
	
	players = 0;
	unsigned char ranks[MAX_PLAYERS];
	getRanks(ranks);
	Snake* lastsnake = 0;
	for(int i=0;i<MAX_PLAYERS;i++){
		if(snakes[i] != 0){
			snakes[i]->restart(ranks[i]+1);
			players++;
			lastsnake = snakes[i];
		}
	}
	if(players==1)
		lastsnake->restart(99);
	updateScores();
	for(int xx=0;xx<width;xx++){
		for(int yy=0;yy<height;yy++){
			map[xx][yy] = 0xFF;
		}
	}
	MessageServerSync mess;
	for(int i=0;i<MAX_PLAYERS;i++){
		mess.x[i] = -1;
		mess.y[i] = -1;
		mess.width[i] = 0;
	}
	int timestamp, delta, time;
	delta = 0;
	state = STATE_RUNNING;
	playersNow = players;

	if((players-bots)<1)
		state = STATE_ENDOFGAME;
//	getRanks(ranks);
	for(int j=MAX_PLAYERS-1;j>=0;j--) {
		for(int i=0;i<MAX_PLAYERS&&state==STATE_RUNNING&&(playersNow>1||(playersNow==1&&players==1));i++) {
			if(snakes[i] == 0)
				continue;
			if(ranks[i] != j)
				continue;
			mess.width[i] = snakes[i]->getWidth();
			mess.x[i] = snakes[i]->getX()-mess.width[i]/2;
			mess.y[i] = snakes[i]->getY()-mess.width[i]/2;
			for(time=0;time<8&&state==STATE_RUNNING&&(playersNow>1||(playersNow==1&&players==1));time++) {
				server->update(90);
				if(time%2==0){
					DrawLine(mess.x[i]+mess.width[i]/2-2,
							 mess.y[i]+mess.width[i]/2-2,
							 mess.x[i]+mess.width[i]/2-2,
							 mess.y[i]+mess.width[i]/2-2,
							 mess.width[i]+4,
							 0xFF);
				}else{
					server->sync(&mess);
				}
			}
		}
	}
	time = 0;
	
	while((playersNow>1||(playersNow==1&&players==1))&&(time<(timeout*1000)||timeout==0)&&state==STATE_RUNNING){
		timestamp = SDL_GetTicks();
		server->update(wantedDelay-delta);
		for(int i=0; i<MAX_PLAYERS; i++){
			if(snakes[i] == 0){
				mess.x[i] = -1;
				mess.y[i] = -1;
				mess.width[i] = 0;
			}else if(!snakes[i]->isAlive()){
				mess.x[i] = -1;
				mess.y[i] = -1;
				mess.width[i] = 0;
			}else{
				int ox = snakes[i]->getX();
				int oy = snakes[i]->getY();
				snakes[i]->move(wantedDelay);
				bool mapcoll = false;
				bool wallcoll = false;
				int w = snakes[i]->getWidth();
				int x = snakes[i]->getX();
				int y = snakes[i]->getY();
				if(x>((width-w/2))||y>((height-w/2))||x<((w/2))||y<((w/2))){
					if(snakes[i]->useSpecial(ABILITY_WALLS)){
						printf("warping from %d %d\n",x,y);
						if(x> (width-w/2) ) x = (w/2)+1;
						else if(x< (w/2) ) x = (width-(w/2))-1;
						else if(y> (height-w/2) ) y = (w/2)+1;
						else if(y< (w/2) ) y = (height-(w/2))-1;
						ox = x; oy = y;
						printf("warping to %d %d\n",x,y);
						snakes[i]->setX(x);
						snakes[i]->setY(y);
						snakes[i]->specialAfterUse();
					}else{
						wallcoll = true;
					}
				}
				if(!wallcoll){
					for(int xx=-w/2;xx<w/2;xx++){
						for(int yy=-w/2;yy<w/2;yy++){
							if(map[x+xx][y+yy]!=0xFF){
								if(!(snakes[i]->isInLast(x+xx,y+yy))){
									mapcoll = true;
									printf("mapcoll at %d %d\n",x+xx,y+yy);
								}
							}
							map[x+xx][y+yy] = i;
						}
					}
					//for(int xx=-w/2;xx<w/2;xx++){
					//	for(int yy=-w/2;yy<w/2;yy++){
					//		map[ox+xx][oy+yy] = i;
					//	}
					//}
				}
				if(mapcoll||wallcoll){
					snakes[i]->kill();
					//char t[3];
					//sprintf(t,"%d",i);
					//std::string s = t;
					server->broadcast_chat_msg(aux::str_arg(aux::str_arg(CCapt::MSG_LOOSE,snakes[i]->getController()->name),aux::str_fromInt(i+1)));
					playersNow--;
					updateScores(true);
				}
				mess.x[i] = snakes[i]->getX()-w/2;
				mess.y[i] = snakes[i]->getY()-w/2;
				if(snakes[i]->isSpecialInUse(ABILITY_INVISIBLE)){
					mess.width[i] = 0;
				}else{
					mess.width[i] = snakes[i]->getWidth();
				}
			}
		}
		server->sync(&mess);
		delta = SDL_GetTicks()-timestamp;
//SDL_Delay((wantedDelay-delta)/2);
//		if(wantedDelay-delta>0)
		//if(wantedDelay>0)
		//	SDL_Delay(wantedDelay);
		//printf("test\n");
		//server->update();
//		SDL_Delay(50);
		time += delta;
		//printf("test3\n");
	}
	//printf("test4\n");
	if(state!=STATE_RUNNING)
		return;
	state = STATE_ENDOFROUND;
	for(int j=0;j<MAX_PLAYERS;j++){
		if(snakes[j]==0)
			continue;
		if(((snakes[j]->getScore()>=(players-1)*10)&&(!(players==1)))){
			state = STATE_ENDOFGAME;
		}
	}
	if((players-bots)<1)
		state = STATE_ENDOFGAME;
}

Snake** Game::getSnakes()
{
	return &(snakes[0]);
}

void Game::processMessage(MessageClientSync* m, PlayerInfo* player)
{
	for(int i=0; i<MAX_PLAYERS; i++){
		if(snakes[i] == 0) continue;
		if(!snakes[i]->isAlive()) continue;
		if(!(snakes[i]->getController()->id == player->id)) continue;
		snakes[i]->setKeys(m->keys[i]);
	}
}

void Game::kickPlayer(PlayerInfo* player)
{
	for(int i=0; i<MAX_PLAYERS; i++){
		if(snakes[i] == 0) continue;
		if(snakes[i]->getController()->id==player->id){
			snakes[i]->kill();
			delete snakes[i];
			snakes[i] = 0;
			players--;
			if((players-bots)<1){
				//server->endGame();
				state = STATE_ENDOFGAME;
			}
			if(state==STATE_RUNNING)
				playersNow--;
		}
	}
}

unsigned char Game::getMap(int x, int y)
{
	if(x<0)
		return 0xFE;
	if(y<0)
		return 0xFE;
	if(x>=width)
		return 0xFE;
	if(y>=height)
		return 0xFE;
	return map[x][y];
}


bool Game::addPlayer(Snake* player, int num)
{
	if(num==-1){
		for(int i=0;i<MAX_PLAYERS;i++){
			if(snakes[i] == 0){
				snakes[i] = player;
				player->setNum(i);
				players++;
				if(state==STATE_RUNNING)
					playersNow++;
				if(player->botAlg() != BOTALG_NOBOT)
					bots++;
				return true;
			}
		}
	}else{
		if(snakes[num] == 0){
			snakes[num] = player;
			player->setNum(num);
			players++;
			if(player->botAlg() != BOTALG_NOBOT)
				bots++;
			if(state==STATE_RUNNING)
				playersNow++;
			return true;
		}
	}
	delete player;
	return false;
}

unsigned char Game::getState()
{
	return state;
}

#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "config.h"
#include "text.h"
#include "client.h"
#include "clientgui.h"
#include "message.h"
#include "settings.h"


ClientGUI::ClientGUI(SDL_Surface* Surface, bool fullscreen)
{
	CConfig cfg;
	memcpy(&cfgkeys,&(cfg.keys),sizeof(cfg.keys));
	memcpy(&cfgcolors,&(cfg.colors),sizeof(cfg.colors));
	
	surface = Surface;
	state = CSTATE_CONNECTING;
	ownSDL = false;
	if(surface == 0){
		ownSDL = true;
		SDL_Init(SDL_INIT_VIDEO);
		#ifdef WIN32
			surface = SDL_SetVideoMode(SCREENWIDTH,SCREENHEIGHT,SCREENBPP,SDL_SWSURFACE|((fullscreen)?SDL_FULLSCREEN:0));
		#else
			surface = SDL_SetVideoMode(SCREENWIDTH,SCREENHEIGHT,SCREENBPP,SDL_HWSURFACE|((fullscreen)?SDL_FULLSCREEN:0));
		#endif
		TTF_Init();
	}
	SDL_ShowCursor(SDL_DISABLE);
	titleFont = TTF_OpenFont(FONT_TITLEFONT,FONT_TITLESIZE);
	normalFont = TTF_OpenFont(FONT_NORMALFONT,FONT_NORMALSIZE);
	tinyFont = TTF_OpenFont(FONT_TINYFONT,FONT_TINYSIZE);
	client = new CClient(this);
	width = -1;
	height = -1;
	chatting = false;
	myChatMsg = "";
}

ClientGUI::~ClientGUI()
{
	delete client;
	SDL_ShowCursor(SDL_ENABLE);
	TTF_CloseFont(titleFont);
	TTF_CloseFont(normalFont);
	TTF_CloseFont(tinyFont);
	if(ownSDL){
		TTF_Quit();
		SDL_Quit();
	}
}

bool ClientGUI::abort_events()
{
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		switch(e.type){
			case SDL_QUIT:
				state=CSTATE_ABORT;
				return false;
			break;
			case SDL_KEYDOWN:
				if(e.key.keysym.sym==ABORT_KEY){
					state=CSTATE_ABORT;
					return false;
				}
			break;	
		}
	}
	return true;
}

void ClientGUI::execute(std::string host,int port,std::string name)
{
	if(state != CSTATE_CONNECTING) return;
	SDL_FillRect(surface,0,0);
	SDL_Color c;
	c.r = 255;
	c.g = 127;
	c.b = 0;
	char ports[15];
	sprintf(ports,"Port %d",port);
	std::string titleS;
	titleS = CCapt::CONNECTINGTO+host;
	std::string titleN;
	titleN = CCapt::WITHNAME+name;
	paintFont(titleFont,titleS.c_str(),c,surface,10,100);
	paintFont(titleFont,ports,c,surface,10,180);
	paintFont(titleFont,titleN.c_str(),c,surface,10,260);
	SDL_Flip(surface);
	
	
	client->connect(host,port,name);
	while(client->connected()&&state==CSTATE_CONNECTING&&abort_events()){
		client->update();
	}
	if(state==CSTATE_CONNECTING||state==CSTATE_ABORT){
		client->disconnect();
		return;
	}
	while(state==CSTATE_LOBBY&&client->connected()){
		client->update();
		if(!(state == CSTATE_LOBBY&&client->connected()))
			break;
		lobby_events();
	}
	if(!(state==CSTATE_ENDOFROUND||state==CSTATE_RUNNING)||state==CSTATE_ABORT){
		client->disconnect();
		return;
	}
	while(client->connected()&&state==CSTATE_ENDOFROUND){
		while(client->connected()&&state==CSTATE_ENDOFROUND){
			client->update();
			eor_events();
		}
		if(state!=CSTATE_RUNNING||state==CSTATE_ABORT){
			client->disconnect();
			return;
		}
		while(client->connected()&&state==CSTATE_RUNNING){
			client->update(0);
		}
	}
	client->disconnect();
}

void ClientGUI::eor_events()
{
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		switch(e.type){
			case SDL_QUIT:
				abort_game();
			break;
			case SDL_KEYDOWN:
				if(e.key.keysym.sym==ABORT_KEY)
					abort_game();
				if(e.key.keysym.sym==SCREENSHOT_KEY)
					SDL_SaveBMP(surface,SCREENSHOT_IMAGE_FILE);
				if(chatting){
					if(e.key.keysym.sym==CHAT_KEY){
						client->chat_msg(myChatMsg);
						chatting = false;
					}
					if(e.key.keysym.sym==SDLK_BACKSPACE){
						myChatMsg = myChatMsg.substr(0,myChatMsg.length()-1);
					}
					if(e.key.keysym.unicode>0x1F&&e.key.keysym.unicode<0x7F)
						myChatMsg = myChatMsg+(char)e.key.keysym.unicode;
					chat(myChatMsg);
				}else{
					if(e.key.keysym.sym==CHAT_KEY){
						chatting = true;
						myChatMsg = "";
						chat(CCapt::MSG_PLEASECHAT);
					}
					if(e.key.keysym.sym==START_KEY)
						client->request(REQUEST_STARTROUND);
				}
			break;	
		}
	}
}

void ClientGUI::game_events()
{
	SDL_Event e;
	bool c = false;
	int i = 0;
	while(SDL_PollEvent(&e)){
		switch(e.type){
			case SDL_QUIT:
				abort_game();
			break;
			case SDL_KEYDOWN:
				if(e.key.keysym.sym==ABORT_KEY)
					abort_game();
				if(e.key.keysym.sym==SCREENSHOT_KEY)
					SDL_SaveBMP(surface,SCREENSHOT_IMAGE_FILE);
				if(chatting){
					if(e.key.keysym.sym==CHAT_KEY){
						client->chat_msg(myChatMsg);
						chatting = false;
					}
					//printf("unicode = %d\n",e.key.keysym.sym);
					if(e.key.keysym.sym==SDLK_BACKSPACE){
						myChatMsg = myChatMsg.substr(0,myChatMsg.length()-1);
					}
					if(e.key.keysym.unicode>0x1F&&e.key.keysym.unicode<0x7F)
						myChatMsg = myChatMsg+(char)e.key.keysym.unicode;
					chat(myChatMsg);
				}else{
					if(e.key.keysym.sym==CHAT_KEY){
						chatting = true;
						myChatMsg = "";
						chat(CCapt::MSG_PLEASECHAT);
					}
					for(i=0;i<MAX_PLAYERS;i++){
						if(e.key.keysym.sym==cfgkeys[i][0]){
							myKeys[i] = KEY_LEFT;
							c = true;
						}else if(e.key.keysym.sym==cfgkeys[i][1]){
							myKeys[i] = KEY_RIGHT;
							c = true;
						}else if(e.key.keysym.sym==cfgkeys[i][2]){
							myKeys[i] |= KEY_SPECIAL;
							c = true;
						}
					}
					if(c)
						client->sendKeys(myKeys);
				}
			break;	
			case SDL_KEYUP:
				for(i=0;i<MAX_PLAYERS;i++){
					if(e.key.keysym.sym==cfgkeys[i][0]&&myKeys[i]&KEY_LEFT){
						myKeys[i] = 0;
						c = true;
					}else if(e.key.keysym.sym==cfgkeys[i][1]&&myKeys[i]&KEY_RIGHT){
						myKeys[i] = 0;
						c = true;
					}else if(e.key.keysym.sym==cfgkeys[i][2]){
						myKeys[i] &= !KEY_SPECIAL;
						c = true;
					}
				}
				if(c)
					client->sendKeys(myKeys);
			break;	
			case SDL_MOUSEBUTTONDOWN:
					for(i=0;i<MAX_PLAYERS;i++){
						if(e.button.button==(cfgkeys[i][0]-SDLK_WORLD_42)){
							myKeys[i] = KEY_LEFT;
							c = true;
						}else if(e.button.button==(cfgkeys[i][1]-SDLK_WORLD_42)){
							myKeys[i] = KEY_RIGHT;
							c = true;
						}else if(e.button.button==(cfgkeys[i][2]-SDLK_WORLD_42)){
							myKeys[i] |= KEY_SPECIAL;
							c = true;
						}
					}
					if(c)
						client->sendKeys(myKeys);
			break;
			case SDL_MOUSEBUTTONUP:
				for(i=0;i<MAX_PLAYERS;i++){
					if(e.button.button==(cfgkeys[i][0]-SDLK_WORLD_42)&&myKeys[i]&KEY_LEFT){
						myKeys[i] = 0;
						c = true;
					}else if(e.button.button==(cfgkeys[i][1]-SDLK_WORLD_42)&&myKeys[i]&KEY_RIGHT){
						myKeys[i] = 0;
						c = true;
					}else if(e.button.button==(cfgkeys[i][2]-SDLK_WORLD_42)){
						myKeys[i] &= !KEY_SPECIAL;
						c = true;
					}
				}
				if(c)
					client->sendKeys(myKeys);
			break;
		}
	}
}

void ClientGUI::lobby_events()
{
	SDL_Event e;
	bool c;
	while(SDL_PollEvent(&e)){
		switch(e.type){
			case SDL_QUIT:
				abort_game();
			break;
			case SDL_KEYDOWN:
				if(e.key.keysym.sym==ABORT_KEY)
					abort_game();
				c = false;
				for(int i=0;i<MAX_PLAYERS;i++){
					if(e.key.keysym.sym==cfgkeys[i][0]){
						iControl[i] = 3;
						c = true;
					}else if(e.key.keysym.sym==cfgkeys[i][1]){
						iControl[i] = 1;
						c = true;
					}
				}
				if(c)
					client->lobby_take(iControl);
				if(e.key.keysym.sym==START_KEY)
					client->request(REQUEST_STARTGAME);
			break;	
			case SDL_MOUSEBUTTONDOWN:
				c = false;
				for(int i=0;i<MAX_PLAYERS;i++){
					if(e.button.button==(cfgkeys[i][0]-SDLK_WORLD_42)){
						iControl[i] = 3;
						c = true;
					}else if(e.button.button==(cfgkeys[i][1]-SDLK_WORLD_42)){
						iControl[i] = 1;
						c = true;
					}
				}
				if(c)
					client->lobby_take(iControl);
			break;
		}
	}
}

CClient* ClientGUI::getClient()
{
	return client;
}

SDL_Surface* ClientGUI::getSurface()
{
	return surface;
}

int ClientGUI::getState()
{
	return state;
}

void ClientGUI::lobby_sync(MessageLobbyState* m)
{
	if(state != CSTATE_LOBBY) return;
	for(int i=0; i<MAX_PLAYERS; i++){
		if(m->controllers[i] != 0)
			controller[i] = m->controllers[i];
		else
			controller[i] = "";
		iControl[i] = m->yourcontrol[i]?2:0;
	}
	lobby_paint();
}

void ClientGUI::gameSync(MessageServerSync* m)
{
	if(state != CSTATE_RUNNING) return;
	SDL_Rect r;
	for(int i=0;i<MAX_PLAYERS;i++){
		if(m->x[i]==-1)
			continue;
		if(m->width[i]>0){
			r.x = m->x[i]+xoff;
			r.y = m->y[i]+yoff;
			r.w = m->width[i];
			r.h = m->width[i];
			SDL_FillRect(surface,&r,colors[i]);
			int ix = r.x-smoothcoords[i][0];
			int iy = r.y-smoothcoords[i][1];
			ix *= ix;
			iy *= iy;
			int dist = ix+iy;
			if(smoothcoords[i][0]<0xFFFF&&(dist>10)){
				r.x = (r.x*2+smoothcoords[i][0])/3;
				r.y = (r.y*2+smoothcoords[i][1])/3;
				r.w = m->width[i];
				r.h = m->width[i];
				SDL_FillRect(surface,&r,colors[i]);
				r.x = (r.x+smoothcoords[i][0]*2)/3;
				r.y = (r.y+smoothcoords[i][1]*2)/3;
				SDL_FillRect(surface,&r,colors[i]);
			}else	if(smoothcoords[i][0]<0xFFFF&&(dist>5)){
				r.x = (r.x+smoothcoords[i][0])/2;
				r.y = (r.y+smoothcoords[i][1])/2;
				r.w = m->width[i];
				r.h = m->width[i];
				SDL_FillRect(surface,&r,colors[i]);
			}
		}
		smoothcoords[i][0] = m->x[i]+xoff;
		smoothcoords[i][1] = m->y[i]+yoff;
	}
	SDL_Flip(surface);
	game_events();
}

void ClientGUI::start_lobby()
{
	state = CSTATE_LOBBY;
	for(int i=0; i<MAX_PLAYERS; i++)
		controller[i] = "";
	lobby_paint();
}

void ClientGUI::start_game()
{
	if(state==CSTATE_ABORT)
		return;
	state = CSTATE_ENDOFROUND;
}

void ClientGUI::end_round()
{
	if(state==CSTATE_ABORT)
		return;
	chat(CCapt::WAIT_ENDOFROUND);
	state = CSTATE_ENDOFROUND;
}

void ClientGUI::updateScoreboard(signed char* scores, signed char* loads)
{
	if(state==CSTATE_RUNNING||state==CSTATE_ENDOFROUND){
		SDL_Rect r;
		r.x = SCREENWIDTH-SCOREOFFSET;
		r.y = 0;
		r.w = SCOREOFFSET;
		r.h = SCREENHEIGHT-MESSAGEOFFSET;
		SDL_FillRect(surface,&r,0x1111);
		for(int i=0;i<MAX_PLAYERS;i++){
			if(scores[i]<0)
				continue;
			SDL_Color c;
			c.r = cfgcolors[i][0];
			c.g = cfgcolors[i][1];
			c.b = cfgcolors[i][2];
			char num[5];
			sprintf(num,"%d",scores[i]);
			paintFont(titleFont,num,c,surface,SCREENWIDTH-SCOREOFFSET,20+i*78);
			if(loads[i]>-1){
				sprintf(num,"%d",loads[i]);
				paintFont(tinyFont,num,c,surface,SCREENWIDTH-SCOREOFFSET+70,80+i*78);
			}
		}
		SDL_UpdateRect(surface,r.x,r.y,r.w,r.h);
	}
}

void ClientGUI::DrawLine(short x1, short y1, short x2, short y2, unsigned char w, unsigned char val)
{
	if(x1==x2&&y1==y2){
		SDL_Rect r;
		r.x = x1-w/2+xoff;
		r.y = y1-w/2+yoff;
		r.w = w;
		r.h = w;
		SDL_FillRect(surface,&r,val==0xFF?0:colors[val]);
		//SDL_Flip(surface);
		SDL_UpdateRect(surface,x1+xoff,y1+yoff,w,w);
		return;
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
			SDL_Rect r;
			r.x = px - w/2;
			r.y = py - w/2;
			r.h = w;
			r.w = w;
			if(r.x>0&&r.x<width-w&&r.y>0&&r.y<height-w){
				r.x += xoff;
				r.y += yoff;
				SDL_FillRect(surface,&r,val==0xFF?0:colors[val]);
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
			SDL_Rect r;
			r.x = px - w/2;
			r.y = py - w/2;
			r.h = w;
			r.w = w;
			if(r.x>0&&r.x<width-w&&r.y>0&&r.y<height-w){
				r.x += xoff;
				r.y += yoff;
				SDL_FillRect(surface,&r,val==0xFF?0:colors[val]);
			}
			x += dx;
			if(x>=dy){
				x -= dy;
				px += sdx;
			}
			py += sdy;
		}
	}	
	//SDL_UpdateRect(surface,(x1-w/2)+xoff,(y1-w/2)+yoff,(x2-x1)+w,(y2-y1)+w);
	//SDL_Flip(surface);
}

void ClientGUI::start_round()
{
	if(state==CSTATE_ABORT)
		return;
	state = CSTATE_RUNNING;
	if(width<0||height<0){
		client->disconnect();
		state = CSTATE_ABORT;
		return;
	}
	game_paint();
	for(int i=0;i<MAX_PLAYERS;i++){
		myKeys[i] = 0;
		colors[i] = SDL_MapRGB(surface->format,cfgcolors[i][0],cfgcolors[i][1],cfgcolors[i][2]);
		smoothcoords[i][0] = 0xFFFF;
		smoothcoords[i][1] = 0xFFFF;
	}
	chat(CCapt::MSG_NEWROUND);
	chatting = false;
}

void ClientGUI::end_game()
{
	if(state==CSTATE_ABORT)
		return;
	state = CSTATE_ENDOFGAME;
}

void ClientGUI::abort_game()
{
	chat(CCapt::WAIT_DISCONNECTING);
	state = CSTATE_ABORT;
	
	//SDL_FillRect(surface,0,0);
	//SDL_Color c;
	//c.r = 255;
	//c.g = 127;
	//c.b = 0;
	//paintFont(normalFont,CCapt::WAIT_DISCONNECTING,c,surface,10,SCREENHEIGHT-MESSAGEOFFSET);
	//SDL_Flip(surface);
}

void ClientGUI::game_init(MessageGameInit* init)
{
	width = init->xsize;
	height = init->ysize;
	xoff = (SCREENWIDTH-SCOREBOARDWIDTH*2-width)/2;
	yoff = (SCREENHEIGHT-MESSAGEBOARDWIDTH*2-height)/2;
}

void ClientGUI::game_paint()
{
	if(state != CSTATE_RUNNING) return;
	SDL_FillRect(surface,0,0x1111);
	SDL_Rect r;
	r.x = xoff;
	r.y = yoff;
	r.w = width;
	r.h = height;
	SDL_FillRect(surface,&r,0);
	SDL_Flip(surface);
}

void ClientGUI::chat(std::string message)
{
	if(state==CSTATE_RUNNING||state==CSTATE_ENDOFROUND){
		SDL_Rect r;
		r.x = 0;
		r.y = SCREENHEIGHT-MESSAGEOFFSET;
		r.w = SCREENWIDTH;
		r.h = MESSAGEOFFSET;
		SDL_FillRect(surface,&r,0x1111);
		SDL_Color c;
		c.r = 255;
		c.g = 127;
		c.b = 0;
		paintFont(normalFont,message.c_str(),c,surface,10,SCREENHEIGHT-MESSAGEOFFSET);
	
		SDL_Flip(surface);
	}
}


void ClientGUI::paintFont(TTF_Font* font, const char* text, SDL_Color c, SDL_Surface* target, int x, int y)
{
	SDL_Surface* tex;
	SDL_Rect dr;
	if(state == CSTATE_LOBBY||state == CSTATE_CONNECTING){
		tex = RENDERMENUFONT(font,text,c);
	}else{
		tex = RENDERGAMEFONT(font,text,c);
	}
	if(tex == 0){
		printf("Can't render font\n");
		return;
	}
	dr.x = x;
	dr.y = y;
	SDL_BlitSurface(tex,0,target,&dr);
	SDL_FreeSurface(tex);
}

void ClientGUI::lobby_paint()
{
	if(state != CSTATE_LOBBY) return;
	SDL_FillRect(surface,0,0);
	SDL_Color c;
	c.r = 255;
	c.g = 127;
	c.b = 0;
	paintFont(titleFont,CCapt::CAPT_LOBBY.c_str(),c,surface,0,0);
	for(int i=0; i<MAX_PLAYERS; i++){
		std::string t = "";
		if(controller[i] == ""){
			t.append(CCapt::PLAYER_UNUSED_1);
			t.append(1,(char)('0'+i));
			t.append(CCapt::PLAYER_UNUSED_2);
		}else{
			t.append(CCapt::PLAYER_CONTROLLEDBY_1);
			t.append(1,(char)('0'+i));
			t.append(CCapt::PLAYER_CONTROLLEDBY_2);
			t.append(controller[i]);
		}
		paintFont(normalFont,t.c_str(),c,surface,0,100+i*30);
	}
	SDL_Flip(surface);
}

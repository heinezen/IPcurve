#include <string>
#include <SDL/SDL.h>
#include "config.h"
#include "text.h"
#include "client.h"
#include "clientgui.h"
#include "message.h"
#include "settings.h"
#include "utils.h"


ClientGUI::ClientGUI(SDL_Surface* Surface, bool use_opengl, bool fullscreen)
{
	CConfig cfg;
	memcpy(&cfgkeys,&(cfg.keys),sizeof(cfg.keys));
	memcpy(&cfgcolors,&(cfg.colors),sizeof(cfg.colors));

	opengl = use_opengl;
	surface = Surface;
	state = CSTATE_CONNECTING;
	ownSDL = false;
	if(!surface) {
		ownSDL = true;
		SDL_Init(SDL_INIT_VIDEO);
		//#ifdef WIN32
		surface = SDL_SetVideoMode(SCREENWIDTH,SCREENHEIGHT,SCREENBPP,(opengl?SDL_HWSURFACE:SDL_SWSURFACE) | (fullscreen?SDL_FULLSCREEN:0) | (opengl?SDL_OPENGL:0));
		if(opengl)
			aux::InitOpenGL(SCREENWIDTH,SCREENHEIGHT);
		//#else
		//	surface = SDL_SetVideoMode(SCREENWIDTH,SCREENHEIGHT,SCREENBPP,SDL_HWSURFACE|((fullscreen)?SDL_FULLSCREEN:0));
		//#endif
		TTF_Init();
	} else {
		opengl = (surface->flags & SDL_OPENGL) > 0;		
	}
	SDL_ShowCursor(0);
	SDL_EnableUNICODE(1);
	titleFont = new CFont(FONT_TITLEFONT,FONT_TITLESIZE,opengl);
	normalFont = new CFont(FONT_NORMALFONT,FONT_NORMALSIZE,opengl);
	tinyFont = new CFont(FONT_TINYFONT,FONT_TINYSIZE,opengl);
	//titleFont = TTF_OpenFont((aux::data_path+FONT_TITLEFONT).c_str(),FONT_TITLESIZE);
	//normalFont = TTF_OpenFont((aux::data_path+FONT_NORMALFONT).c_str(),FONT_NORMALSIZE);
	//tinyFont = TTF_OpenFont((aux::data_path+FONT_TINYFONT).c_str(),FONT_TINYSIZE);
	client = new CClient(this);
	width = -1;
	height = -1;
	chatting = false;
	myChatMsg = "";
	netLobby = false;
	//for(int i=0;i<MAX_CHATS;i++)
	//	chatLog[i] = "";

}

ClientGUI::~ClientGUI()
{
	delete client;
	SDL_ShowCursor(1);
	delete titleFont;
	delete normalFont;
	delete tinyFont;
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

void ClientGUI::execute(std::string host,int port,std::string name, std::string pass)
{
	if(state != CSTATE_CONNECTING) return;
	if(opengl)
		glClear(GL_COLOR_BUFFER_BIT);
	else
		SDL_FillRect(surface,0,0);
	SDL_Color c;
	c.r = 255;
	c.g = 127;
	c.b = 0;
	std::string titleS = CCapt::CONNECTINGTO+host;
	std::string titleP = CCapt::PORT+aux::str_fromInt(port); 
	std::string titleN = CCapt::WITHNAME+name;
	titleFont->render_to(surface,10,100,c,titleS);
	titleFont->render_to(surface,10,170,c,titleP);
	titleFont->render_to(surface,10,240,c,titleN);
//	paintFont(titleFont,titleP,c,surface,10,180);
//	paintFont(titleFont,titleN,c,surface,10,260);
	if(opengl)
		SDL_GL_SwapBuffers();
	else
		SDL_Flip(surface);
	
	
	client->connect(host,port,name,pass);
	while(client->connected() && state==CSTATE_CONNECTING && abort_events()) {
		client->update();
	}
	printf("is_connected: %d - state %d \n",client->connected(),state);
	if(state==CSTATE_CONNECTING || state==CSTATE_ABORT) {
		client->disconnect();
		return;
	}
	printf("is_connected: %d - state %d \n",client->connected(),state);
	while(state==CSTATE_LOBBY && client->connected()) {
		client->update();
		if(!(state == CSTATE_LOBBY&&client->connected()))
			break;
		lobby_events();
	}
	printf("is_connected: %d - state %d \n",client->connected(),state);
	if(!(state==CSTATE_ENDOFROUND || state==CSTATE_RUNNING) || state==CSTATE_ABORT) {
		client->disconnect();
		return;
	}
	printf("is_connected: %d - state %d \n",client->connected(),state);
	while(client->connected() && (state==CSTATE_ENDOFROUND || state==CSTATE_RUNNING)) {
		while(client->connected() && state==CSTATE_ENDOFROUND) {
			client->update();
			eor_events();
		}
		if(state!=CSTATE_RUNNING || state==CSTATE_ABORT) {
			client->disconnect();
			return;
		}
		while(client->connected() && state==CSTATE_RUNNING) {
			client->update();
		}
	}
	client->disconnect();
	if(state==CSTATE_ENDOFGAME)
		show_gameover();
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
					SDL_SaveBMP(surface,(aux::screenshot_path+SCREENSHOT_IMAGE_FILE).c_str());
				if(chatting){
					if(e.key.keysym.sym==CHAT_KEY){
						client->chat_msg(myChatMsg);
						chatting = false;
						SDL_EnableKeyRepeat(0,0);
					}
					if(e.key.keysym.sym==SDLK_BACKSPACE)
						myChatMsg = myChatMsg.substr(0,myChatMsg.length()-1);
					else if(e.key.keysym.unicode>0x1F&&e.key.keysym.unicode<0xFF&&myChatMsg.length()<64)
						myChatMsg = myChatMsg+(char)e.key.keysym.unicode;
					chat(myChatMsg);
				}else{
					if(e.key.keysym.sym==CHAT_KEY){
						chatting = true;
						myChatMsg = "";
						chat(CCapt::MSG_PLEASECHAT);
						SDL_EnableKeyRepeat(200,70);
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
					SDL_SaveBMP(surface,(aux::screenshot_path+SCREENSHOT_IMAGE_FILE).c_str());
				if(chatting){
					if(e.key.keysym.sym==CHAT_KEY){
						client->chat_msg(myChatMsg);
						SDL_EnableKeyRepeat(0,0);
						chatting = false;
					}
					//printf("unicode = %d\n",e.key.keysym.sym);
					if(e.key.keysym.sym==SDLK_BACKSPACE)
						myChatMsg = myChatMsg.substr(0,myChatMsg.length()-1);
					else if(e.key.keysym.unicode>0x1F&&e.key.keysym.unicode<0xFF&&myChatMsg.length()<64)
						myChatMsg = myChatMsg+(char)e.key.keysym.unicode;
					chat(myChatMsg);
				}else{
					if(e.key.keysym.sym==CHAT_KEY){
						chatting = true;
						myChatMsg = "";
						chat(CCapt::MSG_PLEASECHAT);
						SDL_EnableKeyRepeat(200,70);
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
				if(chatting && netLobby){
					if(e.key.keysym.sym==CHAT_KEY){
						client->chat_msg(myChatMsg);
						chatting = false;
						SDL_EnableKeyRepeat(0,0);
					}
					//printf("unicode = %d\n",e.key.keysym.sym);
					if(e.key.keysym.sym==SDLK_BACKSPACE)
						myChatMsg = myChatMsg.substr(0,myChatMsg.length()-1);
					else if(e.key.keysym.unicode>0x1F&&e.key.keysym.unicode<0xFF&&myChatMsg.length()<50)
						myChatMsg = myChatMsg+(char)e.key.keysym.unicode;
					lobby_paint();
				}else{
					if(e.key.keysym.sym==CHAT_KEY && netLobby){
						chatting = true;
						myChatMsg = "";
						lobby_paint();
						SDL_EnableKeyRepeat(200,70);
					}
					c = false;
					for(int i=0;i<MAX_PLAYERS;i++){
						if(e.key.keysym.sym==cfgkeys[i][0]){
							iControl[i] = 3;
							c =	true;
						}else if(e.key.keysym.sym==cfgkeys[i][1]){
							iControl[i] = 1;
							c = true;
						}
					}
					if(c)
						client->lobby_take(iControl);
					if(e.key.keysym.sym==START_KEY)
						client->request(REQUEST_STARTGAME);
				}
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

void ClientGUI::getRanks(unsigned char ranks[])
{
	int i,j,m;
	for(i=0;i<MAX_PLAYERS;i++){
		m = MAX_PLAYERS;
			for(j=0;j<MAX_PLAYERS;j++){
				if(score[i]>=score[j])
					m--;
			}
		ranks[i]=m;
	}
}


bool ClientGUI::gameover_events()
{
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		switch(e.type){
			case SDL_QUIT:
				return false;
			break;
			case SDL_KEYDOWN:
				return false;
			break;	
		}
	}
	return true;
}

void ClientGUI::show_gameover()
{
	if(opengl)
		glClear(GL_COLOR_BUFFER_BIT);
	else
		SDL_FillRect(surface,0,0);

	titleFont->render_to(surface,0,0,255,127,0,CCapt::CAPT_GAME_OVER);
	unsigned char ranks[MAX_PLAYERS];
	getRanks(ranks);
	int cur_y = 0;
	for(int i=0;i<MAX_PLAYERS;i++) {
		for(int j=0;j<MAX_PLAYERS;j++) {
			if(ranks[j]==i && score[j]>=0) {
				std::string t = CCapt::getRankText(i+1,score[j],j+1,controller[j],netLobby);
				normalFont->render_to(surface,15,130+cur_y,cfgcolors[j][0],cfgcolors[j][1],cfgcolors[j][2],t);
				cur_y += 30;
				//printf("score: %d. player %d: %d points \n",i,j,score[j]);
			}
		}
	}
	
	if(opengl)
		SDL_GL_SwapBuffers();
	else
		SDL_Flip(surface);
	while(gameover_events());
}

void ClientGUI::lobby_sync(MessageLobbyState* m)
{
	if(state != CSTATE_LOBBY) return;
	players.clear();
	for(int i=0; i<MAX_PLAYERS; i++){
		if(m->controllers[i] != 0)
			controller[i] = m->controllers[i];
		else
			controller[i] = "";
			
		iControl[i] = m->yourcontrol[i]?2:0;
	}
	for(int i=0; i<MAX_CONNECTIONS; i++) {
		std::string s = m->players[i];
		if(s.length() > 0)
			players.push_back(s);
	}
	if(players.size()==1 && client->isLocalServer())
		netLobby = false;
	else
		netLobby = true;
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
			if(opengl) {
				glEnable(GL_LINE_SMOOTH);
				glLineWidth(m->width[i]);
				glDisable(GL_TEXTURE_2D);
				glColor3f(cfgcolors[i][0]/255.0,cfgcolors[i][1]/255.0,cfgcolors[i][2]/255.0);
				if(smoothcoords[i][0]<0xFFFF) {
					glBegin(GL_LINES);
						glVertex2f(smoothcoords[i][0]+m->width[i]/2,smoothcoords[i][1]+m->width[i]/2);
						glVertex2f(m->x[i]+xoff+m->width[i]/2,m->y[i]+yoff+m->width[i]/2);					
					glEnd();
				} else {
					aux::gfx_FillRect(0,m->x[i]+xoff,m->y[i]+yoff,m->width[i],m->width[i],colors[i],true);
				}
				glLineWidth(1);
				glDisable(GL_LINE_SMOOTH);
			} else {
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
				}else if(smoothcoords[i][0]<0xFFFF&&(dist>5)){
					r.x = (r.x+smoothcoords[i][0])/2;
					r.y = (r.y+smoothcoords[i][1])/2;
					r.w = m->width[i];
					r.h = m->width[i];
					SDL_FillRect(surface,&r,colors[i]);
				}
			}
		}
		smoothcoords[i][0] = m->x[i]+xoff;
		smoothcoords[i][1] = m->y[i]+yoff;
	}
	if(opengl)
		SDL_GL_SwapBuffers();
	else
		SDL_Flip(surface);
	game_events();
}

void ClientGUI::start_lobby()
{
	chatting = false;
	state = CSTATE_LOBBY;
	for(int i=0; i<MAX_PLAYERS; i++)
		controller[i] = "";
	lobby_paint();
}

void ClientGUI::start_game()
{
	chatting = false;
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
	if(state==CSTATE_RUNNING||state==CSTATE_ENDOFROUND||state==CSTATE_ENDOFGAME) {
		for(int i=0;i<MAX_PLAYERS;i++)
			score[i] = scores[i];
	}
	
	if(state==CSTATE_RUNNING||state==CSTATE_ENDOFROUND) {
		SDL_Rect r;
		r.x = SCREENWIDTH-SCOREOFFSET;
		r.y = 0;
		r.w = SCOREOFFSET;
		r.h = SCREENHEIGHT-MESSAGEOFFSET;
		aux::gfx_FillRect(surface,r.x,r.y,r.w,r.h,INGAME_BG_COLOR,opengl);
		unsigned char ranks[MAX_PLAYERS];
		getRanks(ranks);
		int cur_y = 0;
		for(int j=0;j<MAX_PLAYERS;j++){
			for(int i=0;i<MAX_PLAYERS;i++){
				if(ranks[i]==j && scores[i]>=0) {
					titleFont->render_to(surface,SCREENWIDTH-SCOREOFFSET,20+cur_y,cfgcolors[i][0],cfgcolors[i][1],cfgcolors[i][2],aux::str_fromInt(scores[i]));
					if(loads[i]>=0)
						tinyFont->render_to(surface,SCREENWIDTH-SCOREOFFSET+70,73+cur_y,cfgcolors[i][0],cfgcolors[i][1],cfgcolors[i][2],aux::str_fromInt(loads[i]));
					cur_y += 78;
				}
			}
		}
		if(opengl)
			SDL_GL_SwapBuffers();
		else
			SDL_UpdateRect(surface,r.x,r.y,r.w,r.h);
	}
}

void ClientGUI::DrawLine(short x1, short y1, short x2, short y2, unsigned char w, unsigned char val)
{
	if(opengl) {
		if(x1==x2 && y1==y2) {
			aux::gfx_FillRect(0,x1-(w/2)+xoff,y1-(w/2)+yoff,w,w,val==0xFF?0:colors[val],true);
			// printf("drawing quad, %d\n",SDL_GetTicks());
			SDL_GL_SwapBuffers();
			return;
		}
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(w);
		glDisable(GL_TEXTURE_2D);
		if(val==0xFF)
			glColor3f(0,0,0);
		else
			glColor3f(cfgcolors[val][0]/255.0,cfgcolors[val][1]/255.0,cfgcolors[val][2]/255.0);
		//printf("%d %d\n",x1,y1,x2,y2);
		if(x2>(width-w)) {
			float m = (float)(y2-y1)/(float)(x2-x1);
			x2 = width-w;
			y2 = (int)(y1+m*(x2-x1));
		}
		if(y2>(height-w)) {
			float m = (float)(y2-y1)/(float)(x2-x1);
			y2 = height-w;
			x2 = (int)(x1+(y2-y1)/m);
		}
		if(x2<w) {
			float m = (float)(y2-y1)/(float)(x2-x1);
			x2 = w;
			y2 = (int)(y1+m*(x2-x1));
		}
		if(y2<w) {
			float m = (float)(y2-y1)/(float)(x2-x1);
			y2 = w;
			x2 = (int)(x1+(y2-y1)/m);
		}
		glBegin(GL_LINES);
		glVertex2f(x1+xoff,y1+yoff);					
		glVertex2f(x2+xoff,y2+yoff);									
		glEnd();
		glLineWidth(1);
		glDisable(GL_LINE_SMOOTH);
		SDL_GL_SwapBuffers();
		return;
	}
	if(x1==x2&&y1==y2) {
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
	if(dx >= dy) {
		for(x=0;x<dx;x++) {
			SDL_Rect r;
			r.x = px - w/2;
			r.y = py - w/2;
			r.h = w;
			r.w = w;
			if(r.x>0&&r.x<width-w&&r.y>0&&r.y<height-w) {
				r.x += xoff;
				r.y += yoff;
				SDL_FillRect(surface,&r,val==0xFF?0:colors[val]);
			}
			y += dy;
			if(y>=dx) {
				y -= dx;
				py += sdy;
			}
			px += sdx;
		}
	} else {
		for(y=0;y<dy;y++) {
			SDL_Rect r;
			r.x = px - w/2;
			r.y = py - w/2;
			r.h = w;
			r.w = w;
			if(r.x>0&&r.x<width-w&&r.y>0&&r.y<height-w) {
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
	aux::gfx_FillRect(surface,0,0,surface->w,surface->h,INGAME_BG_COLOR,opengl);
	aux::gfx_FillRect(surface,xoff,yoff,width,height,0,opengl);
	if(opengl)
		SDL_GL_SwapBuffers();
	else
		SDL_Flip(surface);
}

void ClientGUI::chat(std::string message)
{
	for(int i=MAX_CHATS-1;i>0;i--)
		chatLog[i] = chatLog[i-1];
	chatLog[0] = message;
	
	if(state==CSTATE_LOBBY)
		lobby_paint();
		
	if(state==CSTATE_RUNNING||state==CSTATE_ENDOFROUND) {
		aux::gfx_FillRect(surface,0,SCREENHEIGHT-MESSAGEOFFSET,SCREENWIDTH,MESSAGEOFFSET,INGAME_BG_COLOR,opengl);
		SDL_Color c;
		c.r = 255;
		c.g = 127;
		c.b = 0;
		normalFont->render_to(surface,10,SCREENHEIGHT-MESSAGEOFFSET,c,message);
		if(opengl)
			SDL_GL_SwapBuffers();
		else
			SDL_UpdateRect(surface,10,SCREENHEIGHT-MESSAGEOFFSET,1000,MESSAGEOFFSET);
		//SDL_Flip(surface);
	}
}


/*void ClientGUI::paintFont(TTF_Font* font,std::string text, SDL_Color c, SDL_Surface* target, int x, int y)
{
	SDL_Surface* tex;
	SDL_Rect dr;
	if(state == CSTATE_LOBBY||state == CSTATE_CONNECTING){
		tex = RENDERMENUFONT(font,text.c_str(),c);
	}else{
		tex = RENDERGAMEFONT(font,text.c_str(),c);
	}
	if(tex == 0){
		printf("Can't render text: '%s'\n",text.c_str());
		return;
	}
	dr.x = x;
	dr.y = y;
	SDL_BlitSurface(tex,0,target,&dr);
	SDL_FreeSurface(tex);
}*/

void ClientGUI::lobby_paint()
{
	if(state != CSTATE_LOBBY)
		return;
	//printf("state: lobby, proc: lobby_paint; begin; ticks=%d \n",SDL_GetTicks());
	if(opengl)
		glClear(GL_COLOR_BUFFER_BIT);
	else
		SDL_FillRect(surface,0,0);
	titleFont->render_to(surface,0,0,255,127,0,CCapt::CAPT_LOBBY);
	tinyFont->render_to(surface,0,65,255,127,0,CCapt::CAPT_LOBBY_HELP);
	SDL_Color c;
	for(int i=0; i<MAX_PLAYERS; i++) {
		std::string t = "";
		c.r = cfgcolors[i][0]; c.g = cfgcolors[i][1]; c.b = cfgcolors[i][2];
		if(controller[i] == "") {
			t = aux::str_arg(aux::str_arg(aux::str_arg(CCapt::PLAYER_UNUSED,
				aux::str_fromInt(i+1)),
				aux::str_keyName(cfgkeys[i][0]) + "   " +
				aux::str_keyName(cfgkeys[i][1]) + "   " +
				aux::str_keyName(cfgkeys[i][2])),
				aux::str_keyName(cfgkeys[i][0]));
			//t.append(1,(char)('0'+i));
			//t.append(CCapt::PLAYER_UNUSED_2);

			c.r /= 2; c.g /= 2; c.b /= 2;
		} else {
			if(iControl[i]==2) {
				t = aux::str_arg(aux::str_arg(aux::str_arg(CCapt::PLAYER_MYCONTROL,
				aux::str_fromInt(i+1)),
				aux::str_keyName(cfgkeys[i][0]) + "   " +
				aux::str_keyName(cfgkeys[i][1]) + "   " +
				aux::str_keyName(cfgkeys[i][2])),
				aux::str_keyName(cfgkeys[i][1]));
			} else {
				t = aux::str_arg(aux::str_arg(aux::str_arg(CCapt::PLAYER_CONTROLLEDBY,
				aux::str_fromInt(i+1)),
				aux::str_keyName(cfgkeys[i][0]) + "   " +
				aux::str_keyName(cfgkeys[i][1]) + "   " +
				aux::str_keyName(cfgkeys[i][2])),
				controller[i]);
			}
			//t.append(CCapt::PLAYER_CONTROLLEDBY_1);
			//t.append(1,(char)('0'+i));
			//t.append(CCapt::PLAYER_CONTROLLEDBY_2);
			//t.append(controller[i]);
		}
		normalFont->render_to(surface,15,130+i*30,c,t);
	}
	//netLobby = true; // DEBUG
	if(netLobby) {
		aux::gfx_DrawRect(surface,795,419,210,279,0xFF0000,opengl);
		normalFont->render_to(surface,800,420,0,180,0,CCapt::CAPT_PLAYERS);
		for(int i=0;i<players.size();i++)
			tinyFont->render_to(surface,800,460+i*tinyFont->getHeight(),255,255,0,players[i]);

		aux::gfx_DrawRect(surface,15,419,781,245,0xFF0000,opengl);
		normalFont->render_to(surface,20,420,0,180,0,CCapt::CAPT_CHAT);
		for(int i=0;i<MAX_CHATS;i++)
			tinyFont->render_to(surface,20,635-i*tinyFont->getHeight(),255,127,0,chatLog[i]);

		aux::gfx_DrawRect(surface,15,663,781,35,0xFF0000,opengl);
						
		if(chatting) {
			if(myChatMsg.length() > 0)
				tinyFont->render_to(surface,20,668,0,180,0,myChatMsg);
			else
				tinyFont->render_to(surface,20,668,0,180,0,CCapt::MSG_PLEASECHAT);
		} else
			tinyFont->render_to(surface,20,668,0,180,0,CCapt::CAPT_RET_TO_CHAT);
		
	}
	//SDL_SaveBMP(surface,"beta.bmp");
	if(opengl)
		SDL_GL_SwapBuffers();
	else
		SDL_Flip(surface);
	//printf("state: lobby, proc: lobby_paint; end; ticks=%d \n",SDL_GetTicks());
}

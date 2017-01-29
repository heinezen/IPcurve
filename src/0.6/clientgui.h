#ifndef CLIENTGUI_H
#define CLIENTGUI_H
#include <stdio.h>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "config.h"
#include "settings.h"
#include "client.h"
#include "message.h"

#define CSTATE_CONNECTING 0
#define CSTATE_LOBBY 1
#define CSTATE_RUNNING 2
#define CSTATE_ENDOFROUND 3
#define CSTATE_ENDOFGAME 4
#define CSTATE_ABORT 5

class ClientGUI{
	public:
		ClientGUI(SDL_Surface* Surface = 0, bool fullscreen = true);
		~ClientGUI();
		
		CClient* getClient();
		SDL_Surface* getSurface();
		
		int getState();
		
		void lobby_sync(MessageLobbyState* m);
		void gameSync(MessageServerSync* m);
		
		void start_lobby();
		void start_game();
		void start_round();
		void end_game();
		void end_round();
		void abort_game();
		
		void game_init(MessageGameInit* init);
		
		void execute(std::string host,int port,std::string name);
		
		void chat(std::string message);
		void updateScoreboard(signed char* scores,signed char* loads);
		void DrawLine(short x1, short y1, short x2, short y2, unsigned char w, unsigned char val);
		
	private:
		void paintFont(TTF_Font* font, const char* text, SDL_Color c, SDL_Surface* target, int x, int y);
		void lobby_paint();
		void game_paint();
		void lobby_events();
		bool abort_events();
		void eor_events();
		void game_events();
		int state;
		bool ownSDL;
		bool chatting;
		std::string myChatMsg;
		int width,height,xoff,yoff;
		CClient* client;
		SDL_Surface* surface;
		TTF_Font* titleFont;
		TTF_Font* normalFont;
		TTF_Font* tinyFont;
		std::string controller[MAX_PLAYERS];
		unsigned char iControl[MAX_PLAYERS];
		unsigned char myKeys[MAX_PLAYERS];
		unsigned short colors[MAX_PLAYERS];
		unsigned short smoothcoords[MAX_PLAYERS][2];
		SDLKey cfgkeys[MAX_PLAYERS][3];
		unsigned char cfgcolors[MAX_PLAYERS][3];
};
#endif

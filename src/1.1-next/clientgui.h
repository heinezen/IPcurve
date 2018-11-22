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
#include "font.h"
//#include "menu.h"

#define CSTATE_CONNECTING 0
#define CSTATE_LOBBY 1
#define CSTATE_RUNNING 2
#define CSTATE_ENDOFROUND 3
#define CSTATE_ENDOFGAME 4
#define CSTATE_ABORT 5

class ClientGUI;

/*
class CLobby : public CMenu {
	public:
		CLobby(SDL_Surface* t, ClientGUI* gui);
		void tick();
		void init();
		void exit();
		void pkeyDown(SDL_keysym* sym);
	private:
		CFont* tinyFont;
		virtual void paint(SDL_Surface* target);
};
*/


class ClientGUI{
	public:
		ClientGUI(SDL_Surface* Surface = 0, bool use_opengl = false, bool fullscreen = true);
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
		void show_gameover();
		
		void game_init(MessageGameInit* init);
		
		void execute(std::string host,int port,std::string name, std::string pass = "");
		
		void chat(std::string message);
		void updateScoreboard(signed char* scores,signed char* loads);
		void DrawLine(short x1, short y1, short x2, short y2, unsigned char w, unsigned char val);
		
	private:
		friend class CLobby;
		//void paintFont(TTF_Font* font,std::string text, SDL_Color c, SDL_Surface* target, int x, int y);
		void lobby_paint();
		void game_paint();
		void lobby_events();
		bool abort_events();
		void eor_events();
		bool gameover_events();
		void game_events();
		void getRanks(unsigned char ranks[]);
		int state;
		bool ownSDL;
		bool chatting;
		bool netLobby;
		bool opengl;
		std::string myChatMsg;
		int width,height,xoff,yoff;
		CClient* client;
		SDL_Surface* surface;
		CFont* titleFont;
		CFont* normalFont;
		CFont* tinyFont;
		std::string chatLog[MAX_CHATS];
		std::string controller[MAX_PLAYERS];
		std::vector<std::string> players;
		signed char score[MAX_PLAYERS];
		unsigned char iControl[MAX_PLAYERS];
		unsigned char myKeys[MAX_PLAYERS];
		unsigned int colors[MAX_PLAYERS];
		unsigned short smoothcoords[MAX_PLAYERS][2];
		SDLKey cfgkeys[MAX_PLAYERS][3];
		unsigned char cfgcolors[MAX_PLAYERS][3];
};
#endif

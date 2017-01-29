#ifndef GAMEMENUS_H
#define GAMEMENUS_H
#include <stdio.h>
#include <string>
#include <vector>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "config.h"
#include "menu.h"
#include "text.h"
#include "settings.h"

class MainMenu:public CMenu{
	public:
		MainMenu(CControl* o,SDL_Surface* t = 0);
};

class ClientMenu:public CMenu{
	public:
		ClientMenu(CControl* o,SDL_Surface* t = 0);
		CTextEdit* ip;
		CTextEdit* port;
		CTextEdit* uname;
		CList* serverlist;
		CCheckBox* useMetaserver;
		std::string metaserver;
};

class ConfigMenu:public CMenu{
	public:
		ConfigMenu(CControl* o,SDL_Surface* t = 0);
		CTextEdit* nick;
		CTextEdit* s_port;
		CTextEdit* s_timeout;
		CNumberEdit* gameSpeed;
		CNumberEdit* speedMulti;
		CKeyEdit* keyInp[MAX_PLAYERS][3];
		CColorEdit* colorInp[MAX_PLAYERS];
		CImageList* specials[MAX_PLAYERS];
		CCheckBox* enableSpecials;
		CCheckBox* enableHoles;
		CCheckBox* englishLanguage;
		CCheckBox* registerMeta;
		CNumberEdit* holeDist;
		CNumberEdit* holeDistRnd;
		CNumberEdit* holeSize;
};
#endif

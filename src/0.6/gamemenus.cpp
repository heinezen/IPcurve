#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_ttf.h>
#include "config.h"
#include "settings.h"
#include "menu.h"
#include "clientgui.h"
#include "server.h"
#include "gamemenus.h"
#include "text.h"

/** MAIN MENU **/
void ExitClicked(CControl* sender,void* data)
{
	CMenu* menu = (CMenu*)data;
	menu->requestClose();
}

void ConnectAsClient(CControl* sender,void* data)
{
	CMenu* menu = (CMenu*)data;
	ClientMenu m(0,menu->getTarget());
	m.execute();
	menu->invalidate();
}

int RunServer(void* server)
{
	CServer* s = (CServer*)server;
	while(!s->isFinished()){
		s->update();
	}
	return 0;
}

void StartGame(CControl* sender,void* data)
{
	int r;
	CConfig* config = new CConfig();
	int port = atoi(config->s_port.c_str());
	std::string nick = config->nick.c_str();
	delete config;
	CMenu* menu = (CMenu*)data;
	CServer* s = new CServer();
	s->listen(port);
	SDL_Thread* sthread = SDL_CreateThread(RunServer,s);
	ClientGUI client(menu->getTarget());
	client.execute("localhost",port,nick);
	s->reqEndGame();
//	SDL_WaitThread(sthread,&r);
	SDL_Delay(500);
	SDL_KillThread(sthread);
	s->endGame();
	s->close();
	delete s;
	menu->invalidate();
}

void DoConfig(CControl* sender,void* data)
{
	CMenu* menu = (CMenu*)data;
	ConfigMenu m(0,menu->getTarget());
	m.execute();
	menu->invalidate();
}


MainMenu::MainMenu(CControl* o,SDL_Surface* t):CMenu(o,t)
{
	CConfig cfg;
	if(cfg.language==LANGUAGE_GERMAN)
		CCapt::setGerman();
	else
		CCapt::setEnglish();
		
	caption = CCapt::CAPT_MAINMENU;
	new CPushButton(this, 50, 100, CCapt::CAPT_NEWGAME, StartGame, this);
	new CPushButton(this, 50, 150, CCapt::CAPT_CLIENT, ConnectAsClient, this);
	new CPushButton(this, 50, 200, CCapt::CAPT_CONFIG, DoConfig, this);
	new CPushButton(this, 50, 600, CCapt::CAPT_EXIT, ExitClicked, this);
}

/** CLIENT MENU **/
void ExitClientClicked(CControl* sender,void* data)
{
	CConfig config;
	ClientMenu* menu = (ClientMenu*)data;
	config.nick = menu->uname->getText().c_str();
	config.c_ip = menu->ip->getText().c_str();
	config.c_port = menu->port->getText().c_str();
	config.c_usemeta = menu->useMetaserver->isChecked();
	menu->requestClose();
}

void ClientFindServers(CControl* sender,void* data)
{
	ClientMenu* menu = (ClientMenu*)data;
	CClient client(0);
	std::vector<std::string> servers;
	if(menu->useMetaserver->isChecked()){
		servers = client.findMetaserverHosts(menu->metaserver);
	}else{
		servers = client.findLanHosts();
	}
	menu->serverlist->clearElements();
	for(int i=0;i<servers.size();i++){
		menu->serverlist->addElement(servers[i]);
	}
	menu->invalidate();
}

void ClientJoinGame(CControl* sender,void* data)
{
	ClientMenu* menu = (ClientMenu*)data;
	ClientGUI client(menu->getTarget());
	client.execute(menu->ip->getText(),atoi(menu->port->getText().c_str()),menu->uname->getText());
	ClientFindServers(sender,menu);
	//menu->invalidate();
}

void EnterServer(CControl* sender,std::string which,void* data)
{
	ClientMenu* menu = (ClientMenu*)data;
	std::string::size_type pos = which.find(" <br> ");
	std::string ipport = which.substr(pos+6);
	pos = ipport.find("   ");
	std::string ip = ipport.substr(0,pos);
	std::string port = ipport.substr(pos+3);
	menu->ip->setText(ip);
	menu->port->setText(port);
}


ClientMenu::ClientMenu(CControl* o,SDL_Surface* t):CMenu(o,t)
{
	CConfig config;
	caption = CCapt::CAPT_CLIENTMENU;
//	new CPushButton(this, 50, 100, CCapt::CAPT_, ConnectAsClient, this);
	new CPushButton(this, 10, 200, CCapt::CAPT_CSERVERIP, 0, 0);
	new CPushButton(this, 10, 250, CCapt::CAPT_CSERVERPORT, 0, 0);
	new CPushButton(this, 10, 300, CCapt::CAPT_USERNAME, 0, 0);
	ip = new CTextEdit(this, 240, 200, 300, config.c_ip);
	port = new CTextEdit(this, 240, 250, 300, config.c_port,true);
	uname = new CTextEdit(this, 240, 300, 300, config.nick);
	new CPushButton(this, 200, 370, CCapt::CAPT_CONNECT, ClientJoinGame, this);
	
	
	metaserver = config.metaserver;
	new CPushButton(this, 630, 80, CCapt::CAPT_SERVERS, 0, 0);
	useMetaserver = new CCheckBox(this, 630, 116, CCapt::CAPT_USEMETA, config.c_usemeta, ClientFindServers, this, FONT_MENUSIZE-3);
	new CPushButton(this, 630, 145, CCapt::CAPT_RENEWSERVERS, ClientFindServers, this, FONT_MENUSIZE-5);
	serverlist = new CList(this,630,190,500,EnterServer,this,FONT_MENUSIZE-4);
	
	new CPushButton(this, 50, 700, CCapt::CAPT_CANCEL, ExitClientClicked, this);
	ClientFindServers(this,this);
	
}

/** CONFIG MENU **/
void ExitConfigClicked(CControl* sender,void* data)
{
	CConfig config;
	ConfigMenu* menu = (ConfigMenu*)data;
	config.nick = menu->nick->getText().c_str();
	config.s_port = menu->s_port->getText().c_str();
	config.s_timeout = atoi(menu->s_timeout->getText().c_str());
	config.s_gameSpeed = menu->gameSpeed->getValue();
	config.s_spdMulti = (menu->speedMulti->getValue()/222.0)+2;
	config.s_enableSpecials = menu->enableSpecials->isChecked();
	config.s_enableHoles = menu->enableHoles->isChecked();
	config.s_holeDist = (menu->holeDist->getValue()*30)+5000;
	config.s_holeRnd = (menu->holeDistRnd->getValue()*10)+2500;
	config.s_holeSize = (menu->holeSize->getValue()/30)+7;
	config.language = (menu->englishLanguage->isChecked());
	config.s_registermeta = (menu->registerMeta->isChecked());
	//holeSize = new CNumberEdit(this, 10, 440, 460, (int)floor(config.holeSize*100), CCapt::CAPT_SHOLEDISTRND);
	for(int i=0;i<MAX_PLAYERS;i++){
		config.keys[i][0] = menu->keyInp[i][0]->getKey();
		config.keys[i][1] = menu->keyInp[i][1]->getKey();
		config.keys[i][2] = menu->keyInp[i][2]->getKey();
		config.colors[i][0] = menu->colorInp[i]->getColor().r;
		config.colors[i][1] = menu->colorInp[i]->getColor().g;
		config.colors[i][2] = menu->colorInp[i]->getColor().b;
		config.special[i] = menu->specials[i]->getImage();
	}
	menu->requestClose();
}

ConfigMenu::ConfigMenu(CControl* o,SDL_Surface* t):CMenu(o,t)
{
	CConfig config;
	caption = CCapt::CAPT_CONFIGMENU;
	char temp[10];
	sprintf(temp,"%d",config.s_timeout);
	
	// NICK, SERVER-PORT, GAME-SPEED
	new CPushButton(this, 10, 80, CCapt::CAPT_USERNAME, 0, 0, FONT_MENUSIZE);
	nick = new CTextEdit(this, 230, 80, 200, config.nick);
	
	new CPushButton(this, 10, 120, CCapt::CAPT_SSERVERPORT, 0, 0, FONT_MENUSIZE);
	s_port = new CTextEdit(this, 230, 120, 200, config.s_port, true);
	
	new CPushButton(this, 10, 160, CCapt::CAPT_STIMEOUT, 0, 0, FONT_MENUSIZE);
	s_timeout = new CTextEdit(this, 230, 160, 200, temp, true);
	
	gameSpeed = new CNumberEdit(this, 10, 200, 420, config.s_gameSpeed, CCapt::CAPT_SGAMESPEED);
	
	speedMulti = new CNumberEdit(this, 10, 240, 420, (short)floor((config.s_spdMulti-2)*222), CCapt::CAPT_SSPEEDMULTI);
	
	enableSpecials = new CCheckBox(this, 10, 280, CCapt::CAPT_SENABLESPECIALS, config.s_enableSpecials);
	
	enableHoles = new CCheckBox(this, 10, 320, CCapt::CAPT_SENABLEHOLES, config.s_enableHoles);
	holeDist = new CNumberEdit(this, 10, 360, 420, ((config.s_holeDist-5000)/30), CCapt::CAPT_SHOLEDIST);
	holeDistRnd = new CNumberEdit(this, 10, 400, 420, ((config.s_holeRnd-2500)/10), CCapt::CAPT_SHOLEDISTRND);
	holeSize = new CNumberEdit(this, 10, 440, 420, (int)floor((config.s_holeSize-7)*30), CCapt::CAPT_SHOLESIZE);
	
	englishLanguage = new CCheckBox(this, 10, 480, CCapt::CAPT_LANGENGLISH, config.language);
	
	registerMeta = new CCheckBox(this, 10, 520, CCapt::CAPT_REGISTERMETA, config.s_registermeta);
	
	// PLAYER KEYS & COLORS
	new CPushButton(this, 550, 80, CCapt::CAPT_LEFTKEY, 0, 0, FONT_MENUSIZE);
	new CPushButton(this, 710, 80, CCapt::CAPT_RIGHTKEY, 0, 0, FONT_MENUSIZE);
	new CPushButton(this, 870, 80, CCapt::CAPT_SPECIALKEY, 0, 0, FONT_MENUSIZE);
	for(int i=0;i<MAX_PLAYERS;i++){
		sprintf(temp,"%d",i);
		new CPushButton(this, 436, 120+i*71, temp, 0, 0, 60);
		specials[i] = new CImageList(this, 495, 126+i*71, 50, config.special[i], SPECIALS_IMAGE_FILE);
		
		keyInp[i][0] = new CKeyEdit(this, 550, 120+i*71, 150, config.keys[i][0]);
		keyInp[i][1] = new CKeyEdit(this, 710, 120+i*71, 150, config.keys[i][1]);
		keyInp[i][2] = new CKeyEdit(this, 870, 120+i*71, 150, config.keys[i][2]);
		SDL_Color c;
		c.r = config.colors[i][0];
		c.g = config.colors[i][1];
		c.b = config.colors[i][2];
		colorInp[i] = new CColorEdit(this, 550, 153+i*71, 470, c);
	}
	
	// DONE BUTTON
	new CPushButton(this, 50, 700, CCapt::CAPT_DONE, ExitConfigClicked, this);
	
}

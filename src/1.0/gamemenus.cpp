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
#include "utils.h"

/** MAIN MENU **/
void ExitClicked(CControl* sender,void* data)
{
	CMenu* menu = (CMenu*)data;
	menu->requestClose();
}

void ConnectAsClient(CControl* sender,void* data)
{
	CMenu* menu = (CMenu*)data;
	menu->hideAllTooltips();
	ClientMenu* m = new ClientMenu(0,menu->getTarget());
	m->execute();
	delete m;
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
	CConfig* config = new CConfig();
	int port = aux::str_toInt(config->s_port);
	std::string nick = config->nick.c_str();
	std::string pass = config->s_password.c_str();
	delete config;
	CMenu* menu = (CMenu*)data;
	menu->cleanSDL();
	menu->hideAllTooltips();
	CServer* s = new CServer();
	s->listen(port);
	SDL_Thread* sthread = SDL_CreateThread(RunServer,s);
	ClientGUI client(menu->getTarget());
	client.execute("localhost",port,nick,pass);
	printf("requesting server slay\n");
	s->reqEndGame();
//	SDL_WaitThread(sthread,&r);
	//SDL_Delay(500);
	printf("killing my server thread friend\n");
	SDL_WaitThread(sthread,0);
	printf("ending serwar game\n");
	s->endGame();
	s->close();
	printf("ending this great game\n");
	delete s;
	menu->resetSDL();
	menu->invalidate();
}

void DoConfig(CControl* sender,void* data)
{
	CMenu* menu = (CMenu*)data;
	menu->hideAllTooltips();
	ConfigMenu* m = new ConfigMenu(0,menu->getTarget());
	m->execute();
	delete m;
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
	CPushButton* b;
	b = new CPushButton(this, 50, 100, CCapt::CAPT_NEWGAME);
		b->setTooltip(CCapt::CAPT_NEWGAME, CCapt::TOOLTIP_NEWGAME);
		b->setOnClick(StartGame, this);
	b = new CPushButton(this, 50, 150, CCapt::CAPT_CLIENT);
		b->setTooltip(CCapt::CAPT_CLIENT, CCapt::TOOLTIP_JOINGAME);
		b->setOnClick(ConnectAsClient, this);
	b = new CPushButton(this, 50, 200, CCapt::CAPT_CONFIG);
		b->setTooltip(CCapt::CAPT_CONFIG,CCapt::TOOLTIP_SETTINGS);
		b->setOnClick(DoConfig, this);
	b = new CPushButton(this, 50, 600, CCapt::CAPT_EXIT);
		b->setTooltip(CCapt::CAPT_EXIT,CCapt::TOOLTIP_EXIT);
		b->setOnClick(ExitClicked, this);
/*	CTabControl* tabs = new CTabControl(this,480,200,500,500);
	CTabWidget* tab1 = new CTabWidget(tabs,"test1");
	CTabWidget* tab2 = new CTabWidget(tabs,"test2");
	CTabWidget* tab3 = new CTabWidget(tabs,"test3");
	new CPushButton(tab1,500,300,"test1");
	new CPushButton(tab1,500,350,"test2");
	new CPushButton(tab1,500,400,"test3");
	new CPushButton(tab2,600,300,"2test1");
	new CPushButton(tab2,600,350,"2test2");
	new CPushButton(tab2,600,400,"2test3"); */
		//but->setTooltip("Start new Game","Starts a new Game 5235234 64 5634 4356 43 643 6 4356 34 56 3456 3456 3456 3456 34 565436");
	/*TableRow c;
	c.push_back("test1");
	c.push_back("test2");
	c.push_back("test3");
	c.push_back("test4");
	ColWidths* cols = new ColWidths();
	cols->push_back(50);
	cols->push_back(200);
	cols->push_back(140);
	cols->push_back(60);
	CTable* table = new CTable(this,555,200,450,500,c);
	table->setColWidths(cols);
	TableRow r;
	r.push_back("hallo1");
	r.push_back("hallo2");
	r.push_back("hallo3");
	r.push_back("hallo4");
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);
	table->addRow(r);*/
}

/** CLIENT MENU **/
void ExitClientClicked(CControl* sender,void* data)
{
	CConfig config;
	ClientMenu* menu = (ClientMenu*)data;
	//config.nick = menu->uname->getText().c_str();
	config.c_ip = menu->ip->getText().c_str();
	config.c_port = menu->port->getText().c_str();
	//config.c_usemeta = menu->useMetaserver->isChecked();
	menu->requestClose();
}

void ClientFindServers(CControl* sender,void* data)
{
	ClientMenu* menu = (ClientMenu*)data;
	CTabControl* tabs = (CTabControl*)menu->tabs;
	
	// If Custom-IP Tab, return
	if(tabs->getActiveTab()==2)
		return;

	CPopupProgress pro(menu,CCapt::CAPT_SERVERLOOKUP,CCapt::TEXT_SERVERLOOKUP);
	pro.show();

	CClient client(0);
	ServerList servers;
	CTable* serverlist = 0;
	if(tabs->getActiveTab()==1) {
		printf("using meta\n");
		servers = client.findMetaserverHosts(menu->metaserver);
		serverlist = menu->serverlist_meta;
	} else {
		servers = client.findLanHosts();
		serverlist = menu->serverlist_lan;
	}
	serverlist->clearRows();
	for(unsigned int i=0;i<servers.size();i++) {
		TableRow row;
		row.push_back(servers[i].name);
		row.push_back(servers[i].desc);
		if(servers[i].pass)
			row.push_back(CCapt::YES);
		else 
			row.push_back(CCapt::NO);
		row.push_back(servers[i].host);
		row.push_back(aux::str_fromInt(servers[i].port));
		serverlist->addRow(row);
	}
	pro.hide();
	serverlist->repaint();
	ClientError err = client.getError();
	if(err != NO_ERR) {
		CPopupMessage p(menu,CCapt::CAPT_ERROR,CCapt::getClientErrorText(err));
		p.execute();
	}
	//menu->invalidate();
}

void ClientJoinGame(CControl* sender,void* data)
{
	CConfig config;
	ClientMenu* menu = (ClientMenu*)data;
	menu->cleanSDL();
	ClientGUI client(menu->getTarget());
	client.execute(menu->ip->getText(),aux::str_toInt(menu->port->getText()),config.nick,menu->pass->getText());
	menu->resetSDL();
	menu->invalidate();
	ClientError err = client.getClient()->getError();
	if(err != NO_ERR) {
		CPopupMessage p(menu,CCapt::CAPT_ERROR,CCapt::getClientErrorText(err));
		p.execute();
	}
	//ClientFindServers(sender,menu);
}

void EnterServer(CControl* sender,void* data)
{
	TableRow* row = ((CTable*)sender)->getCurrentRow();
	if(!row)
		return;
	CConfig config;
	ClientMenu* menu = (ClientMenu*)data;
	std::string pass = "";
	if(row->at(2) == CCapt::YES) {
		CPopupInput inp(menu,CCapt::CAPT_ENTERPW,CCapt::TEXT_ENTERPW);
		inp.execute();
		if(!inp.getValue())
			return;
		pass = inp.getInput();
	} 
	menu->cleanSDL();
	ClientGUI client(menu->getTarget());
	client.execute(row->at(3),aux::str_toInt(row->at(4)),config.nick,pass);
	menu->resetSDL();
	menu->invalidate();
	ClientError err = client.getClient()->getError();
	if(err != NO_ERR) {
		CPopupMessage p(menu,CCapt::CAPT_ERROR,CCapt::getClientErrorText(err));
		p.execute();
	}
	ClientFindServers(sender,menu);
	/*	ClientMenu* menu = (ClientMenu*)data;
	std::string::size_type pos = which.find(" <br> ");
	std::string ipport = which.substr(pos+6);
	pos = ipport.find("   ");
	std::string ip = ipport.substr(0,pos);
	std::string port = ipport.substr(pos+3);
	menu->ip->setText(ip);
	menu->port->setText(port);*/
}


ClientMenu::ClientMenu(CControl* o,SDL_Surface* t):CMenu(o,t)
{
	CConfig config;
	caption = CCapt::CAPT_CLIENTMENU;
	
	tabs = new CTabControl(this,10,80,1004,600);
	tabs->setOnTabChanged(ClientFindServers,this);
	CTabWidget* tabLAN = new CTabWidget(tabs,CCapt::CAPT_TAB_LAN);
	CTabWidget* tabInternet = new CTabWidget(tabs,CCapt::CAPT_TAB_INTERNET);
	CTabWidget* tabCustomIP = new CTabWidget(tabs,CCapt::CAPT_TAB_CUSTOMIP);	
			
//	new CPushButton(this, 50, 100, CCapt::CAPT_, ConnectAsClient, this);
	// -------------------- CUSTOM IP -----------------------
	new CPushButton(tabCustomIP, 30, 200, CCapt::CAPT_CSERVERIP);
	new CPushButton(tabCustomIP, 30, 250, CCapt::CAPT_CSERVERPORT);
	new CPushButton(tabCustomIP, 30, 300, CCapt::CAPT_SERVERPASS);
	ip = new CTextEdit(tabCustomIP, 260, 200, 300, config.c_ip);
	port = new CTextEdit(tabCustomIP, 260, 250, 300, config.c_port,true);
	pass = new CTextEdit(tabCustomIP, 260, 300, 300, "");
	CPushButton* b;
	
	b = new CPushButton(tabCustomIP, 200, 370, CCapt::CAPT_CONNECT);
		b->setOnClick(ClientJoinGame, this);
	
	
	// ------------------- INTERNET ---------------------------
	metaserver = config.metaserver;
//	new CPushButton(, 630, 80, CCapt::CAPT_SERVERS); --- REMOVE CCapt::CAPT_SERVERS
	//useMetaserver = new CCheckBox(this, 630, 116, CCapt::CAPT_USEMETA, config.c_usemeta, FONT_MENUSIZE-3);
	//useMetaserver->setOnChange(ClientFindServers, this);  ---- REMOVE CCapt::CAPT_USEMETA
	b = new CPushButton(tabInternet, 36, 630, CCapt::CAPT_RENEWSERVERS);
		b->setOnClick(ClientFindServers, this);
		b->setPos((SCREENWIDTH - b->getW())/2,630);

	TableRow c;
	c.push_back(CCapt::CAPT_SERVERNAME);
	c.push_back(CCapt::CAPT_SERVERDESC);
	c.push_back(CCapt::CAPT_SERVERPASS);
	c.push_back(CCapt::CAPT_SERVERHOST);
	c.push_back(CCapt::CAPT_SERVERPORT);
	ColWidths* cols = new ColWidths();
	cols->push_back(205);
	cols->push_back(400);
	cols->push_back(80);	
	cols->push_back(190);
	cols->push_back(75);
	//CTable* table = new CTable(this,555,200,450,500,c);
	serverlist_meta = new CTable(tabInternet,36,120,950,500,c);
	serverlist_meta->setColWidths(cols);
	serverlist_meta->setOnChoice(EnterServer,this);
	
	
	// ---------------------- LAN -------------------------
	b = new CPushButton(tabLAN, 36, 630, CCapt::CAPT_RENEWSERVERS);
		b->setOnClick(ClientFindServers, this);
		b->setPos((SCREENWIDTH - b->getW())/2,630);
	
	cols = new ColWidths();
	cols->push_back(205);
	cols->push_back(400);
	cols->push_back(80);	
	cols->push_back(190);
	cols->push_back(75);
	//CTable* table = new CTable(this,555,200,450,500,c);
	serverlist_lan = new CTable(tabLAN,36,120,950,500,c);
	serverlist_lan->setColWidths(cols);
	serverlist_lan->setOnChoice(EnterServer,this);

//	serverlist->setOnElementClick(EnterServer,this);
	
	b = new CPushButton(this, 50, 700, CCapt::CAPT_CANCEL);
		b->setOnClick(ExitClientClicked, this);
	repaint();
	ClientFindServers(tabs,this);
	
	// Warning
	//CPopupMessage m(this,"TODO","Dieser Teil wird gerade überarbeitet. Raus hier.");
	//m.execute();
	//requestClose();
}

/** CONFIG MENU **/
void ExitConfigClicked(CControl* sender,void* data)
{
	CConfig config;
	ConfigMenu* menu = (ConfigMenu*)data;
	config.nick = menu->nick->getText().c_str();
	config.s_port = menu->s_port->getText().c_str();
	config.s_desc = menu->s_desc->getText().c_str();
	config.s_timeout = aux::str_toInt(menu->s_timeout->getText());
	config.s_gameSpeed = menu->gameSpeed->getValue();
	config.s_spdMulti = ((1000-menu->speedMulti->getValue())/222.0)+2;
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
	config.c_fullscreen = menu->fullscreen->isChecked();
	config.c_tooltips = menu->tooltips->isChecked();
	config.s_turnAngle =  (menu->turnAngle->getValue()/20000.0)+0.009;//config.s_turnAngle+0.01)*20000)
	config.s_password =  menu->s_pass->getText().c_str();
	//CPopupInput msg(menu,"input test","enter some junk");
	//msg.execute();
	menu->requestClose();
}

void ColorChanged(CControl* sender, void* menu)
{
	ConfigMenu* m = (ConfigMenu*)menu;
	for(int i=0;i<MAX_PLAYERS;i++)
		if(sender==m->colorInp[i]) {
			SDL_Color* c = new SDL_Color;
			*c = ((CColorEdit*)sender)->getColor();
			m->playerDesc[i]->setColor(c);
			m->playerDesc[i]->repaint();
			break;
		}

}

void AbilityChanged(CControl* sender, void* menu)
{
	ConfigMenu* m = (ConfigMenu*)menu;
	for(int i=0;i<MAX_PLAYERS;i++)
		if(sender==m->specials[i]) {
			m->specials[i]->setTooltip(aux::str_arg(CCapt::CAPT_SPECIAL,aux::str_fromInt(i+1)), aux::str_arg(aux::str_arg(CCapt::TOOLTIP_C_SPECIAL,aux::str_fromInt(i+1)),CCapt::TOOLTIP_SPECIAL_DESC[m->specials[i]->getImage()]));
			break;
		}

}


void LoadProfile(std::string which,ConfigMenu* m)
{
	GameProfile* p = m->profiler.getProfile(which);
	if(!p)
		return;
	CPopupDialog dlg(m,CCapt::CAPT_LOADPROFILE_CONFIRM, aux::str_arg(CCapt::TEXT_LOADPROFILE_CONFIRM,p->name));
	dlg.execute();
	if(!dlg.getValue())
		return;
	// load it
	m->s_timeout->setText(aux::str_fromInt(p->timeout));
	m->gameSpeed->setValue(p->gameSpeed);
	m->enableSpecials->setChecked(p->enableSpecials);
	m->enableHoles->setChecked(p->enableHoles);
	m->holeDist->setValue(p->holeDist);
	m->holeDistRnd->setValue(p->holeRnd);
	m->holeSize->setValue(p->holeSize);
	m->turnAngle->setValue(p->turnAngle);	
}

void SaveProfile(std::string which,ConfigMenu* m)
{
	GameProfile* o = m->profiler.getProfile(which);
	if(o) {
		CPopupDialog dlg(m,CCapt::CAPT_SAVEPROFILE_CONFIRM, aux::str_arg(CCapt::TEXT_SAVEPROFILE_CONFIRM,which));
		dlg.execute();
		if(!dlg.getValue())
			return;
		o->timeout = aux::str_toInt(m->s_timeout->getText());
		o->gameSpeed = m->gameSpeed->getValue();
		o->enableSpecials = m->enableSpecials->isChecked();
		o->enableHoles = m->enableHoles->isChecked();
		o->holeDist = m->holeDist->getValue();
		o->holeRnd = m->holeDistRnd->getValue();
		o->holeSize = m->holeSize->getValue();
		o->turnAngle = m->turnAngle->getValue();		
	} else {
		//save it
		GameProfile p;
		p.name = which;
		p.timeout = aux::str_toInt(m->s_timeout->getText());
		p.gameSpeed = m->gameSpeed->getValue();
		p.enableSpecials = m->enableSpecials->isChecked();
		p.enableHoles = m->enableHoles->isChecked();
		p.holeDist = m->holeDist->getValue();
		p.holeRnd = m->holeDistRnd->getValue();
		p.holeSize = m->holeSize->getValue();
		p.turnAngle = m->turnAngle->getValue();
		m->profiler.addProfile(p);
	}
}

void DeleteProfile(std::string which,ConfigMenu* m)
{
	GameProfile* p = m->profiler.getProfile(which);
	if(!p)
		return;
	CPopupDialog dlg(m,CCapt::CAPT_DELETEPROFILE_CONFIRM, aux::str_arg(CCapt::TEXT_DELETEPROFILE_CONFIRM,p->name));
	dlg.execute();
	if(!dlg.getValue())
		return;
	//delete it
	m->profiler.deleteProfile(which);
}

void RefreshProfiles(ConfigMenu* m)
{
	m->gameProfiles->clearElements();
	GameProfileList* profiles = m->profiler.getProfiles();
	GameProfileList::iterator it = profiles->begin();
	while(it != profiles->end()) {
		m->gameProfiles->addElement((*it).name);
		++it;
	}
	m->gameProfiles->repaint();
}

void ProfileClicked(CControl* sender,int button,std::string which,void* menu)
{
	ConfigMenu* m = (ConfigMenu*)menu;
	m->hideAllTooltips();
	if(button==1) {
		LoadProfile(which,m);
	} else if(button==2) {
		DeleteProfile(which,m);
		RefreshProfiles(m);
	} else if(button==3) {
		SaveProfile(which,m);
		RefreshProfiles(m);
	}
}

void AddProfile(CControl* sender, void* menu)
{
	ConfigMenu* m = (ConfigMenu*)menu;
	m->hideAllTooltips();
	CPopupInput dlg(m,CCapt::CAPT_SAVEPROFILE_NAME, CCapt::TEXT_SAVEPROFILE_NAME);
	dlg.execute();
	if(!dlg.getValue())
		return;
	SaveProfile(dlg.getInput(),m);
	RefreshProfiles(m);
}

ConfigMenu::ConfigMenu(CControl* o, SDL_Surface* t):CMenu(o,t)
{
	CConfig config;
	caption = CCapt::CAPT_CONFIGMENU;
	//char temp[10];
	//sprintf(temp,"%d",config.s_timeout);
	
	CTabControl* tabs = new CTabControl(this,10,80,1004,600);
	CTabWidget* tabGeneral = new CTabWidget(tabs,CCapt::CAPT_TAB_GENERAL);
	CTabWidget* tabGameType = new CTabWidget(tabs,CCapt::CAPT_TAB_GAME);
	CTabWidget* tabServNetwork = new CTabWidget(tabs,CCapt::CAPT_TAB_SRV_NETWORK);
	CTabWidget* tabClient = new CTabWidget(tabs,CCapt::CAPT_TAB_PLAYERS);

	/// ----------------------- GENERAL ------------------------
	new CPushButton(tabGeneral, 20, 120, CCapt::CAPT_USERNAME, FONT_MENUSIZE);
	nick = new CTextEdit(tabGeneral, 200, 120, 200, config.nick);
	nick->setTooltip(CCapt::CAPT_USERNAME,CCapt::TOOLTIP_USERNAME);

	englishLanguage = new CCheckBox(tabGeneral, 20, 160, CCapt::CAPT_LANGENGLISH, config.language);
	englishLanguage->setTooltip(CCapt::CAPT_LANGENGLISH,CCapt::TOOLTIP_S_ENGLISH);

	fullscreen = new CCheckBox(tabGeneral, 20, 200, CCapt::CAPT_FULLSCREEN, config.c_fullscreen);
	fullscreen->setTooltip(CCapt::CAPT_FULLSCREEN,CCapt::TOOLTIP_FULLSCREEN);

	tooltips = new CCheckBox(tabGeneral, 20, 240, CCapt::CAPT_TOOLTIPS, config.c_tooltips);
	tooltips->setTooltip(CCapt::CAPT_TOOLTIPS,CCapt::TOOLTIP_TOOLTIPS);
		
	/// ------------------ SERVER: GAMEPLAY ---------------------
	new CPushButton(tabGameType, 20, 120, CCapt::CAPT_STIMEOUT, FONT_MENUSIZE);
	s_timeout = new CTextEdit(tabGameType, 240, 120, 200, aux::str_fromInt(config.s_timeout), true);
	s_timeout->setTooltip(CCapt::CAPT_STIMEOUT,CCapt::TOOLTIP_S_TIMEOUT);
	
	gameSpeed = new CNumberEdit(tabGameType, 20, 160, 420, config.s_gameSpeed, CCapt::CAPT_SGAMESPEED);
	gameSpeed->setTooltip(CCapt::CAPT_SGAMESPEED,CCapt::TOOLTIP_S_SPEED);
	
	enableSpecials = new CCheckBox(tabGameType, 20, 200, CCapt::CAPT_SENABLESPECIALS, config.s_enableSpecials);
	enableSpecials->setTooltip(CCapt::CAPT_SENABLESPECIALS,CCapt::TOOLTIP_S_SPECIALS);
	
	enableHoles = new CCheckBox(tabGameType, 20, 240, CCapt::CAPT_SENABLEHOLES, config.s_enableHoles);
	enableHoles->setTooltip(CCapt::CAPT_SENABLEHOLES,CCapt::TOOLTIP_S_HOLES);
	holeDist = new CNumberEdit(tabGameType, 20, 280, 420, ((config.s_holeDist-5000)/30), CCapt::CAPT_SHOLEDIST);
	holeDist->setTooltip(CCapt::CAPT_SHOLEDIST,CCapt::TOOLTIP_S_HOLE_DIST);
	holeDistRnd = new CNumberEdit(tabGameType, 20, 320, 420, ((config.s_holeRnd-2500)/10), CCapt::CAPT_SHOLEDISTRND);
	holeDistRnd->setTooltip(CCapt::CAPT_SHOLEDISTRND,CCapt::TOOLTIP_S_HOLE_DIST_RAND);
	holeSize = new CNumberEdit(tabGameType, 20, 360, 420, (int)floor((config.s_holeSize-7)*30), CCapt::CAPT_SHOLESIZE);
	holeSize->setTooltip(CCapt::CAPT_SHOLESIZE,CCapt::TOOLTIP_S_HOLE_SIZE);	
	turnAngle = new CNumberEdit(tabGameType, 20, 400, 420, (int)floor((config.s_turnAngle-0.009)*20000), CCapt::CAPT_STURNANGLE);
	turnAngle->setTooltip(CCapt::CAPT_STURNANGLE,CCapt::TOOLTIP_S_TURNANGLE);
	// game profile list
	new CPushButton(tabGameType, 555, 120, CCapt::CAPT_GAMEPROFILES);
	CPushButton* addButton = new CPushButton(tabGameType, 555, 160, CCapt::CAPT_ADDPROFILE, FONT_MENUSIZE);
	addButton->setOnClick(AddProfile, this);
	addButton->setTooltip(CCapt::CAPT_ADDPROFILE,CCapt::TOOLTIP_ADDPROFILE);
	gameProfiles = new CList(tabGameType, 555, 200, 420);
	gameProfiles->setOnElementClick(ProfileClicked,this);
	gameProfiles->setTooltip(CCapt::CAPT_GAMEPROFILES,CCapt::TOOLTIP_GAMEPROFILES);
	GameProfileList* profiles = profiler.getProfiles();
	GameProfileList::iterator it = profiles->begin();
	while(it != profiles->end()) {
		gameProfiles->addElement((*it).name);
		++it;
	}
	
	
	
	/// ------------------- SERVER: NETWORK --------------------
	speedMulti = new CNumberEdit(tabServNetwork, 20, 120, 500, (short)floor(1000-((config.s_spdMulti-2)*222)), CCapt::CAPT_SSPEEDMULTI);
	speedMulti->setTooltip(CCapt::CAPT_SSPEEDMULTI,CCapt::TOOLTIP_S_MULTI);

	new CPushButton(tabServNetwork, 20, 160, CCapt::CAPT_SSERVERPORT, FONT_MENUSIZE);
	s_port = new CTextEdit(tabServNetwork, 240, 160, 280, config.s_port, true);
	s_port->setTooltip(CCapt::CAPT_SSERVERPORT,CCapt::TOOLTIP_S_PORT);

	registerMeta = new CCheckBox(tabServNetwork, 20, 200, CCapt::CAPT_REGISTERMETA, config.s_registermeta);
	registerMeta->setTooltip(CCapt::CAPT_REGISTERMETA,CCapt::TOOLTIP_S_METASERVER);
		
	new CPushButton(tabServNetwork, 20, 240, CCapt::CAPT_SSERVERDESC, FONT_MENUSIZE);
	s_desc = new CTextEdit(tabServNetwork, 20, 275, 500, config.s_desc, false);
	s_desc->setTooltip(CCapt::CAPT_SSERVERDESC,CCapt::TOOLTIP_S_DESC);

	new CPushButton(tabServNetwork, 20, 320, CCapt::CAPT_SSERVERPASS, FONT_MENUSIZE);
	s_pass = new CTextEdit(tabServNetwork, 200, 320, 320, config.s_password, false);
	s_pass->setTooltip(CCapt::CAPT_SSERVERPASS,CCapt::TOOLTIP_S_PASS);
	
	/// ------------------- CLIENT: PLAYERS --------------------
	new CPushButton(tabClient, 130, 120, CCapt::CAPT_LEFTKEY, FONT_MENUSIZE);
	new CPushButton(tabClient, 260, 120, CCapt::CAPT_RIGHTKEY, FONT_MENUSIZE);
	new CPushButton(tabClient, 390, 120, CCapt::CAPT_SPECIALKEY, FONT_MENUSIZE);
	for(int i=0;i<MAX_PLAYERS;i++){
		//sprintf(temp,"%d",i);
		playerDesc[i] = new CPushButton(tabClient, 27, 153+i*55, aux::str_fromInt(i+1), 55);
		specials[i] = new CImageList(tabClient, 70, 153+i*55, 50, config.special[i], aux::data_path+SPECIALS_IMAGE_FILE);
		specials[i]->setTooltip(aux::str_arg(CCapt::CAPT_SPECIAL,aux::str_fromInt(i+1)), aux::str_arg(aux::str_arg(CCapt::TOOLTIP_C_SPECIAL,aux::str_fromInt(i+1)),CCapt::TOOLTIP_SPECIAL_DESC[specials[i]->getImage()]));
		specials[i]->setOnChange(AbilityChanged, this);
		
		keyInp[i][0] = new CKeyEdit(tabClient, 130, 160+i*55, 120, config.keys[i][0]);
		keyInp[i][0]->setTooltip(aux::str_arg(CCapt::CAPT_KEY_LEFT,aux::str_fromInt(i+1)), aux::str_arg(CCapt::TOOLTIP_C_KEY_LEFT,aux::str_fromInt(i+1)));
		keyInp[i][1] = new CKeyEdit(tabClient, 260, 160+i*55, 120, config.keys[i][1]);
		keyInp[i][1]->setTooltip(aux::str_arg(CCapt::CAPT_KEY_RIGHT,aux::str_fromInt(i+1)), aux::str_arg(CCapt::TOOLTIP_C_KEY_RIGHT,aux::str_fromInt(i+1)));
		keyInp[i][2] = new CKeyEdit(tabClient, 390, 160+i*55, 120, config.keys[i][2]);
		keyInp[i][2]->setTooltip(aux::str_arg(CCapt::CAPT_KEY_SPECIAL,aux::str_fromInt(i+1)), aux::str_arg(CCapt::TOOLTIP_C_KEY_SPECIAL,aux::str_fromInt(i+1)));
		SDL_Color* c = new SDL_Color;
		c->r = config.colors[i][0];
		c->g = config.colors[i][1];
		c->b = config.colors[i][2];
		colorInp[i] = new CColorEdit(tabClient, 530, 160+i*55, 470, *c);
		colorInp[i]->setOnChange(ColorChanged, this);
		colorInp[i]->setTooltip(aux::str_arg(CCapt::CAPT_COLORCHANGE,aux::str_fromInt(i+1)), aux::str_arg(CCapt::TOOLTIP_C_COLORCHANGE,aux::str_fromInt(i+1)));
		playerDesc[i]->setColor(c);
	}
	
	// DONE BUTTON
	(new CPushButton(this, 50, 700, CCapt::CAPT_DONE))->setOnClick(ExitConfigClicked, this);
	
}

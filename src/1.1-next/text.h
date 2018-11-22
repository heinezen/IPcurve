#ifndef CTEXT_H
#define CTEXT_H
#include <string>
#include "config.h"
#include "client.h"

//enum ClientError;


class CCapt{

public:
	static int language;
	static void setGerman();
	static void setEnglish();
	static std::string PLAYER_UNUSED;
	static std::string PLAYER_MYCONTROL;
	static std::string PLAYER_CONTROLLEDBY;
	
	static std::string CONNECTINGTO;
	static std::string WITHNAME;
	static std::string PORT;

	static std::string WAIT_DISCONNECTING;
	static std::string WAIT_ENDOFROUND;
	static std::string MSG_NEWROUND;
	static std::string MSG_DISCONNECT;
	static std::string MSG_LOOSE;
	static std::string MSG_PLEASECHAT;

	static std::string CHAT_SAYS;

	static std::string CAPT_MAINMENU;
	static std::string CAPT_CLIENTMENU;
	static std::string CAPT_CONFIGMENU;
	static std::string CAPT_LOBBY;

	static std::string CAPT_EXIT;
	static std::string CAPT_CLIENT;
	static std::string CAPT_CONFIG;
	static std::string CAPT_NEWGAME;
	static std::string CAPT_DONE;
	static std::string CAPT_CONNECT;
	static std::string CAPT_CANCEL;

	static std::string CAPT_CSERVERIP;
	static std::string CAPT_CSERVERPORT;
	static std::string CAPT_USERNAME;

	static std::string CAPT_SSERVERPORT;
	static std::string CAPT_SSERVERDESC;
	static std::string CAPT_SSERVERPASS;
	static std::string CAPT_STIMEOUT;
	static std::string CAPT_SGAMESPEED;
	static std::string CAPT_SSPEEDMULTI;
	static std::string CAPT_SENABLESPECIALS;
	static std::string CAPT_SENABLEHOLES;
	static std::string CAPT_SHOLEDIST;
	static std::string CAPT_SHOLEDISTRND;
	static std::string CAPT_SHOLESIZE;
	static std::string CAPT_STURNANGLE;

	static std::string CAPT_LANGENGLISH;
	
	static std::string CAPT_LEFTKEY;
	static std::string CAPT_RIGHTKEY;
	static std::string CAPT_SPECIALKEY;

	static std::string CAPT_LOBBY_HELP;

	static std::string CAPT_REDCOMP;
	static std::string CAPT_GREENCOMP;
	static std::string CAPT_BLUECOMP;
	
	static std::string CAPT_RENEWSERVERS;
	static std::string CAPT_REGISTERMETA;
	static std::string CAPT_COLORCHANGE;
	static std::string CAPT_KEY_LEFT;
	static std::string CAPT_KEY_RIGHT;
	static std::string CAPT_KEY_SPECIAL;
	static std::string CAPT_SPECIAL;
	
	static std::string CAPT_ERROR;
	static std::string CAPT_GAME_OVER;

	static std::string CAPT_POPUP_OKAY;
	static std::string CAPT_POPUP_CANCEL;
	static std::string CAPT_POPUP_APPLY;

	static std::string CAPT_TITLE_EMPTY_STRING;
	static std::string CAPT_TEXT_EMPTY_STRING;

	static std::string CAPT_TOOLTIPS;
	static std::string CAPT_GAMEPROFILES;
	static std::string CAPT_FULLSCREEN;
	
	static std::string CAPT_TAB_GENERAL;
	static std::string CAPT_TAB_GAME;
	static std::string CAPT_TAB_SRV_NETWORK;
	static std::string CAPT_TAB_PLAYERS;

	static std::string CAPT_TAB_LAN;
	static std::string CAPT_TAB_INTERNET;
	static std::string CAPT_TAB_CUSTOMIP;
	static std::string CAPT_SERVERNAME;
	static std::string CAPT_SERVERDESC;
	static std::string CAPT_SERVERHOST;
	static std::string CAPT_SERVERPING;
	static std::string CAPT_SERVERPLAYERS;
	static std::string CAPT_SERVERPORT;
	static std::string CAPT_SERVERPASS;
	static std::string CAPT_SERVERLOOKUP;
	static std::string TEXT_SERVERLOOKUP;
		
	static std::string CAPT_SAVEPROFILE_NAME;
	static std::string TEXT_SAVEPROFILE_NAME;
	static std::string CAPT_LOADPROFILE_CONFIRM;
	static std::string TEXT_LOADPROFILE_CONFIRM;
	static std::string CAPT_SAVEPROFILE_CONFIRM;
	static std::string TEXT_SAVEPROFILE_CONFIRM;
	static std::string CAPT_DELETEPROFILE_CONFIRM;
	static std::string TEXT_DELETEPROFILE_CONFIRM;
	static std::string CAPT_ADDPROFILE;
	static std::string TOOLTIP_ADDPROFILE;
	static std::string TOOLTIP_GAMEPROFILES;

	static std::string CAPT_ENTERPW;
	static std::string TEXT_ENTERPW;

	static std::string YES;
	static std::string NO;

	static std::string TOOLTIP_FULLSCREEN;
	static std::string TOOLTIP_TOOLTIPS;
	
	static std::string TOOLTIP_NEWGAME;
	static std::string TOOLTIP_JOINGAME;
	static std::string TOOLTIP_SETTINGS;
	static std::string TOOLTIP_EXIT;

	static std::string CAPT_PLAYERS;
	static std::string CAPT_CHAT;
	 
	static std::string TOOLTIP_USERNAME;
	static std::string TOOLTIP_S_PORT;
	static std::string TOOLTIP_S_DESC;
	static std::string TOOLTIP_S_PASS;
	static std::string TOOLTIP_S_TIMEOUT;
	static std::string TOOLTIP_S_SPEED;
	static std::string TOOLTIP_S_MULTI;
	static std::string TOOLTIP_S_SPECIALS;
	static std::string TOOLTIP_S_HOLES;
	static std::string TOOLTIP_S_HOLE_DIST;
	static std::string TOOLTIP_S_HOLE_DIST_RAND;
	static std::string TOOLTIP_S_HOLE_SIZE;
	static std::string TOOLTIP_S_TURNANGLE;
	static std::string TOOLTIP_S_ENGLISH;
	static std::string TOOLTIP_S_METASERVER;
	static std::string TOOLTIP_S_SERVERDESC;
	static std::string TOOLTIP_C_COLORCHANGE;
	static std::string TOOLTIP_C_KEY_LEFT;
	static std::string TOOLTIP_C_KEY_RIGHT;
	static std::string TOOLTIP_C_KEY_SPECIAL;
	static std::string CAPT_RET_TO_CHAT;
	static std::string TOOLTIP_C_SPECIAL;
	static std::string TOOLTIP_SPECIAL_DESC[ABILITY_COUNT];
	
	static std::string getClientErrorText(ClientError err);
	static std::string getRankText(int rank, int score, int player, std::string controller, bool show_controller);
};

#endif

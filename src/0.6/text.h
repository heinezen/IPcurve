#ifndef CTEXT_H
#define CTEXT_H
#include <string>
class CCapt{

public:
	static void setGerman();
	static void setEnglish();
	static std::string PLAYER_UNUSED_1;
	static std::string PLAYER_UNUSED_2;
	static std::string PLAYER_CONTROLLEDBY_1;
	static std::string PLAYER_CONTROLLEDBY_2;

	static std::string CONNECTINGTO;
	static std::string WITHNAME;

	static std::string WAIT_DISCONNECTING;
	static std::string WAIT_ENDOFROUND;
	static std::string MSG_NEWROUND;
	static std::string MSG_DISCONNECT;
	static std::string MSG_LOOSE_PRE;
	static std::string MSG_LOOSE_MID;
	static std::string MSG_LOOSE_POST;
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
	static std::string CAPT_STIMEOUT;
	static std::string CAPT_SGAMESPEED;
	static std::string CAPT_SSPEEDMULTI;
	static std::string CAPT_SENABLESPECIALS;
	static std::string CAPT_SENABLEHOLES;
	static std::string CAPT_SHOLEDIST;
	static std::string CAPT_SHOLEDISTRND;
	static std::string CAPT_SHOLESIZE;

	static std::string CAPT_LANGENGLISH;
	
	static std::string CAPT_LEFTKEY;
	static std::string CAPT_RIGHTKEY;
	static std::string CAPT_SPECIALKEY;

	static std::string CAPT_REDCOMP;
	static std::string CAPT_GREENCOMP;
	static std::string CAPT_BLUECOMP;
	
	static std::string CAPT_SERVERS;
	static std::string CAPT_RENEWSERVERS;
	static std::string CAPT_USEMETA;
	static std::string CAPT_REGISTERMETA;
};

#endif

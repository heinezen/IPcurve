#include <string>
#include "text.h"

	 std::string CCapt::PLAYER_UNUSED_1 = "";
	 std::string CCapt::PLAYER_UNUSED_2 = "";
	 std::string CCapt::PLAYER_CONTROLLEDBY_1 = "";
	 std::string CCapt::PLAYER_CONTROLLEDBY_2 = "";

	 std::string CCapt::CONNECTINGTO = "";
	 std::string CCapt::WITHNAME = "";

	 std::string CCapt::WAIT_DISCONNECTING = "";
	 std::string CCapt::WAIT_ENDOFROUND = "";
	 std::string CCapt::MSG_NEWROUND = "";
	 std::string CCapt::MSG_DISCONNECT = "";
	 std::string CCapt::MSG_LOOSE_PRE = "";
	 std::string CCapt::MSG_LOOSE_MID = "";
	 std::string CCapt::MSG_LOOSE_POST = "";
	 std::string CCapt::MSG_PLEASECHAT = "";

	 std::string CCapt::CHAT_SAYS = "";

	 std::string CCapt::CAPT_MAINMENU  = "";
	 std::string CCapt::CAPT_CLIENTMENU = "";
	 std::string CCapt::CAPT_CONFIGMENU = "";
	 std::string CCapt::CAPT_LOBBY = "";

	 std::string CCapt::CAPT_EXIT = "";
	 std::string CCapt::CAPT_CLIENT = "";
	 std::string CCapt::CAPT_CONFIG = "";
	 std::string CCapt::CAPT_NEWGAME = "";
	 std::string CCapt::CAPT_DONE = "";
	 std::string CCapt::CAPT_CONNECT = "";
	 std::string CCapt::CAPT_CANCEL = "";

	 std::string CCapt::CAPT_CSERVERIP = "";
	 std::string CCapt::CAPT_CSERVERPORT = "";
	 std::string CCapt::CAPT_USERNAME = "";

	 std::string CCapt::CAPT_SSERVERPORT = "";
	 std::string CCapt::CAPT_STIMEOUT = "";
	 std::string CCapt::CAPT_SGAMESPEED = "";
	 std::string CCapt::CAPT_SSPEEDMULTI = "";
	 std::string CCapt::CAPT_SENABLESPECIALS = "";
	 std::string CCapt::CAPT_SENABLEHOLES = "";
	 std::string CCapt::CAPT_SHOLEDIST = "";
	 std::string CCapt::CAPT_SHOLEDISTRND = "";
	 std::string CCapt::CAPT_SHOLESIZE = "";

	 std::string CCapt::CAPT_LEFTKEY = "";
	 std::string CCapt::CAPT_RIGHTKEY = "";
	 std::string CCapt::CAPT_SPECIALKEY = "";

	 std::string CCapt::CAPT_REDCOMP = "";
	 std::string CCapt::CAPT_GREENCOMP = "";
	 std::string CCapt::CAPT_BLUECOMP = "";
	 
	 std::string CCapt::CAPT_LANGENGLISH = "";
	 std::string CCapt::CAPT_SERVERS = "";
	 std::string CCapt::CAPT_RENEWSERVERS = "";
	 std::string CCapt::CAPT_USEMETA = "";
	 std::string CCapt::CAPT_REGISTERMETA = "";

 void CCapt::setGerman(){
		 PLAYER_UNUSED_1 = "Spieler ";
		 PLAYER_UNUSED_2 = " ist unbenutzt";
		 PLAYER_CONTROLLEDBY_1 = "Spieler ";
		 PLAYER_CONTROLLEDBY_2 = " wird wird kontrolliert durch ";

		 CONNECTINGTO = "Verbinde zu ";
		 WITHNAME = "als Spieler ";

		 WAIT_DISCONNECTING = "Verbindung wird getrennt...";
		 WAIT_ENDOFROUND = "Ende der Runde...";
		 MSG_NEWROUND = "Neue Runde gestartet...";
		 MSG_DISCONNECT = " hat das Spiel verlassen";
		 MSG_LOOSE_PRE = "";
		 MSG_LOOSE_MID = "s Spieler ";
		 MSG_LOOSE_POST = " ist raus";
		 MSG_PLEASECHAT = "Nachricht eingeben...";

		 CHAT_SAYS = " sagt ";

		 CAPT_MAINMENU  = "IPCurve v0.6";
		 CAPT_CLIENTMENU = "Zu Server verbinden";
		 CAPT_CONFIGMENU = "Einstellungen";
		 CAPT_LOBBY = "Lobby";

		 CAPT_EXIT = "Beenden";
		 CAPT_CLIENT = "Zu Server verbinden";
		 CAPT_CONFIG = "Einstellungen";
		 CAPT_NEWGAME = "Neues Spiel";
		 CAPT_DONE = "Fertig";
		 CAPT_CONNECT = "Verbinden";
		 CAPT_CANCEL = "Abbrechen";

		 CAPT_CSERVERIP = "Server IP";
		 CAPT_CSERVERPORT = "Port";
		 CAPT_USERNAME = "NickName";

		 CAPT_SSERVERPORT = "ServerPort";
		 CAPT_STIMEOUT = "MaxSpielzeit";
		 CAPT_SGAMESPEED = "Spielgeschwindigkeit";
		 CAPT_SSPEEDMULTI = "Geschw. Multiplikator";
		 CAPT_SENABLESPECIALS = "Spezialeigenschaften";
		 CAPT_SENABLEHOLES = "Holes";
		 CAPT_SHOLEDIST = "HoleDistanz";
		 CAPT_SHOLEDISTRND = "HoleDistanzRandom";
		 CAPT_SHOLESIZE = "HoleSize";

		 CAPT_LEFTKEY = "Links";
		 CAPT_RIGHTKEY = "Rechts";
		 CAPT_SPECIALKEY = "Spezial";

		 CAPT_REDCOMP = "Rot";
		 CAPT_GREENCOMP = "Grun";
		 CAPT_BLUECOMP = "Blau";
	
		 CAPT_LANGENGLISH = "English language";
		 CAPT_SERVERS = "Serverliste";
		 CAPT_RENEWSERVERS = "Liste erneuern";
		 CAPT_USEMETA = "Internet Metaserver";
		 CAPT_REGISTERMETA = "Metaserver verwenden";
 }

void CCapt::setEnglish(){
	PLAYER_UNUSED_1 = "Player ";
	PLAYER_UNUSED_2 = " is unused";
	PLAYER_CONTROLLEDBY_1 = "Player ";
	PLAYER_CONTROLLEDBY_2 = " is being controlled by ";

	CONNECTINGTO = "Connecting to ";
	WITHNAME = "using name ";

	WAIT_DISCONNECTING = "Disconnecting...";
	WAIT_ENDOFROUND = "End of Round...";
	MSG_NEWROUND = "Next round started...";
	MSG_DISCONNECT = " left the game";
	MSG_LOOSE_PRE = "";
	MSG_LOOSE_MID = "s Snake number ";
	MSG_LOOSE_POST = " didnt make it";
	MSG_PLEASECHAT = "Enter your chat message...";

	CHAT_SAYS = " says ";

	CAPT_MAINMENU  = "IPCurve v0.6";
	CAPT_CLIENTMENU = "Connect to Server";
	CAPT_CONFIGMENU = "Settings";
	CAPT_LOBBY = "Lobby";

	CAPT_EXIT = "Quit";
	CAPT_CLIENT = "Connect to Server";
	CAPT_CONFIG = "Settings";
	CAPT_NEWGAME = "New Game";
	CAPT_DONE = "Done";
	CAPT_CONNECT = "Connect";
	CAPT_CANCEL = "Cancel";

	CAPT_CSERVERIP = "Server IP";
	CAPT_CSERVERPORT = "Port";
	CAPT_USERNAME = "NickName";

	CAPT_SSERVERPORT = "ServerPort";
	CAPT_STIMEOUT = "RoundTimeout";
	CAPT_SGAMESPEED = "GameSpeed";
	CAPT_SSPEEDMULTI = "SpeedMultiplier";
	CAPT_SENABLESPECIALS = "Special abilitys";
	CAPT_SENABLEHOLES = "Holes";
	CAPT_SHOLEDIST = "HoleDistance";
	CAPT_SHOLEDISTRND = "HoleDistanceRandom";
	CAPT_SHOLESIZE = "HoleSize";

	CAPT_LEFTKEY = "Left";
	CAPT_RIGHTKEY = "Right";
	CAPT_SPECIALKEY = "Special";

	CAPT_REDCOMP = "Red";
	CAPT_GREENCOMP = "Green";
	CAPT_BLUECOMP = "Blue";
	
	CAPT_LANGENGLISH = "English language";
	CAPT_SERVERS = "Serverlist";
	CAPT_RENEWSERVERS = "Refresh List";
	CAPT_USEMETA = "Internet Metaserver";
	CAPT_REGISTERMETA = "Register at Metaserver";
	
}

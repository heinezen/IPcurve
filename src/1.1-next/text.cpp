#include <string>
#include "text.h"
#include "utils.h"

	 int CCapt::language = LANGUAGE_ENGLISH;
	
	 std::string CCapt::TOOLTIP_NEWGAME = "";
	 std::string CCapt::TOOLTIP_JOINGAME = "";
	 std::string CCapt::TOOLTIP_SETTINGS = "";
	 std::string CCapt::TOOLTIP_EXIT = "";
	 std::string CCapt::TOOLTIP_FULLSCREEN = "";
	 std::string CCapt::TOOLTIP_TOOLTIPS = "";
	 
	 std::string CCapt::TOOLTIP_USERNAME = "";
	 std::string CCapt::TOOLTIP_S_PORT = "";
	 std::string CCapt::TOOLTIP_S_DESC = "";
	 std::string CCapt::TOOLTIP_S_PASS = "";
	 std::string CCapt::TOOLTIP_S_TIMEOUT = "";
	 std::string CCapt::TOOLTIP_S_SPEED = "";
	 std::string CCapt::TOOLTIP_S_MULTI = "";
	 std::string CCapt::TOOLTIP_S_SPECIALS = "";
	 std::string CCapt::TOOLTIP_S_HOLES = "";
	 std::string CCapt::TOOLTIP_S_HOLE_DIST = "";
	 std::string CCapt::TOOLTIP_S_HOLE_DIST_RAND = "";
	 std::string CCapt::TOOLTIP_S_HOLE_SIZE = "";
	 std::string CCapt::TOOLTIP_S_ENGLISH = "";
	 std::string CCapt::TOOLTIP_S_METASERVER = "";
	 std::string CCapt::TOOLTIP_S_TURNANGLE = "";
	 
	 
	 std::string CCapt::TOOLTIP_C_COLORCHANGE = "";
	 std::string CCapt::TOOLTIP_C_KEY_LEFT = "";
	 std::string CCapt::TOOLTIP_C_KEY_RIGHT = "";
	 std::string CCapt::TOOLTIP_C_KEY_SPECIAL = "";
	 std::string CCapt::TOOLTIP_C_SPECIAL = "";
	 std::string CCapt::TOOLTIP_SPECIAL_DESC[ABILITY_COUNT] = {"","","","","","","","","","",""};


	
	 std::string CCapt::PLAYER_UNUSED = "";
	 std::string CCapt::PLAYER_MYCONTROL = "";
	 std::string CCapt::PLAYER_CONTROLLEDBY = "";

	 std::string CCapt::CONNECTINGTO = "";
	 std::string CCapt::PORT = "";
	 std::string CCapt::WITHNAME = "";

	 std::string CCapt::WAIT_DISCONNECTING = "";
	 std::string CCapt::WAIT_ENDOFROUND = "";
	 std::string CCapt::MSG_NEWROUND = "";
	 std::string CCapt::MSG_DISCONNECT = "";
	 std::string CCapt::MSG_LOOSE = "";
	 std::string CCapt::MSG_PLEASECHAT = "";

	 std::string CCapt::CHAT_SAYS = "";

	 std::string CCapt::CAPT_MAINMENU  = "";
	 std::string CCapt::CAPT_CLIENTMENU = "";
	 std::string CCapt::CAPT_CONFIGMENU = "";
	 std::string CCapt::CAPT_LOBBY = "";

	 std::string CCapt::CAPT_ERROR = "";

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
	 std::string CCapt::CAPT_FULLSCREEN = "";
	 std::string CCapt::CAPT_TOOLTIPS = "";
	 std::string CCapt::CAPT_GAMEPROFILES = "";

	 std::string CCapt::CAPT_SSERVERPORT = "";
	 std::string CCapt::CAPT_SSERVERDESC = "";
	 std::string CCapt::CAPT_SSERVERPASS = "";
	 std::string CCapt::CAPT_STIMEOUT = "";
	 std::string CCapt::CAPT_SGAMESPEED = "";
	 std::string CCapt::CAPT_SSPEEDMULTI = "";
	 std::string CCapt::CAPT_SENABLESPECIALS = "";
	 std::string CCapt::CAPT_SENABLEHOLES = "";
	 std::string CCapt::CAPT_SHOLEDIST = "";
	 std::string CCapt::CAPT_SHOLEDISTRND = "";
	 std::string CCapt::CAPT_SHOLESIZE = "";
	 std::string CCapt::CAPT_STURNANGLE = "";
	 
	 std::string CCapt::CAPT_LOADPROFILE_CONFIRM = "";
	 std::string CCapt::TEXT_LOADPROFILE_CONFIRM = "";
	 std::string CCapt::CAPT_SAVEPROFILE_CONFIRM = "";
	 std::string CCapt::TEXT_SAVEPROFILE_CONFIRM = "";
	 std::string CCapt::CAPT_DELETEPROFILE_CONFIRM = "";
	 std::string CCapt::TEXT_DELETEPROFILE_CONFIRM = "";
	 std::string CCapt::CAPT_ADDPROFILE = "";
	 std::string CCapt::TOOLTIP_ADDPROFILE = "";
	 std::string CCapt::TOOLTIP_GAMEPROFILES = "";

	 std::string CCapt::CAPT_LEFTKEY = "";
	 std::string CCapt::CAPT_RIGHTKEY = "";
	 std::string CCapt::CAPT_SPECIALKEY = "";

	 std::string CCapt::CAPT_REDCOMP = "";
	 std::string CCapt::CAPT_GREENCOMP = "";
	 std::string CCapt::CAPT_BLUECOMP = "";
	 
	 std::string CCapt::CAPT_LANGENGLISH = "";
	 std::string CCapt::CAPT_RENEWSERVERS = "";
	 std::string CCapt::CAPT_REGISTERMETA = "";
	 std::string CCapt::CAPT_COLORCHANGE = "";
	 std::string CCapt::CAPT_KEY_LEFT = "";
	 std::string CCapt::CAPT_KEY_RIGHT = "";
	 std::string CCapt::CAPT_KEY_SPECIAL = "";
	 std::string CCapt::CAPT_SPECIAL = "";
	 std::string CCapt::CAPT_TAB_GENERAL = "";
	 std::string CCapt::CAPT_TAB_GAME = "";
	 std::string CCapt::CAPT_TAB_SRV_NETWORK = "";
	 std::string CCapt::CAPT_TAB_PLAYERS = "";

	 std::string CCapt::CAPT_POPUP_OKAY = "";
	 std::string CCapt::CAPT_POPUP_CANCEL = "";
	 std::string CCapt::CAPT_POPUP_APPLY = "";
	 std::string CCapt::CAPT_TITLE_EMPTY_STRING = "";
	 std::string CCapt::CAPT_TEXT_EMPTY_STRING = "";
	 std::string CCapt::CAPT_SAVEPROFILE_NAME = "";
	 std::string CCapt::TEXT_SAVEPROFILE_NAME = "";
	 
	 std::string CCapt::CAPT_TAB_LAN = "";
	 std::string CCapt::CAPT_TAB_INTERNET = "";
	 std::string CCapt::CAPT_TAB_CUSTOMIP = "";
	 std::string CCapt::CAPT_SERVERNAME = "";
	 std::string CCapt::CAPT_SERVERDESC = "";
	 std::string CCapt::CAPT_SERVERHOST = "";
	 std::string CCapt::CAPT_SERVERPORT = "";
	 std::string CCapt::CAPT_SERVERPASS = "";
	 std::string CCapt::CAPT_SERVERPING = "";
	 std::string CCapt::CAPT_SERVERPLAYERS = "";

	 std::string CCapt::CAPT_SERVERLOOKUP = "";
	 std::string CCapt::TEXT_SERVERLOOKUP = "";
	 
	 std::string CCapt::YES = "";
	 std::string CCapt::NO = "";
	 
	 std::string CCapt::CAPT_LOBBY_HELP = "";
	 
	 std::string CCapt::CAPT_ENTERPW = "";
	 std::string CCapt::TEXT_ENTERPW = "";
	 std::string CCapt::CAPT_RET_TO_CHAT = "";
	 std::string CCapt::CAPT_PLAYERS = "";
	 std::string CCapt::CAPT_CHAT = "";
	 std::string CCapt::CAPT_GAME_OVER = "";

 void CCapt::setGerman(){
	language = LANGUAGE_GERMAN;
	
	PLAYER_UNUSED = "Spieler %0 (%1) mit %2 übernehmen";
	PLAYER_MYCONTROL = "Spieler %0 (%1) übernommen - mit %2 freigeben";
	PLAYER_CONTROLLEDBY= "Spieler %0 (%1) wird kontrolliert durch %2";
	CONNECTINGTO = "Verbinde zu ";
	PORT = "Port ";
	WITHNAME = "als Spieler ";

	WAIT_DISCONNECTING = "Verbindung wird getrennt...";
	WAIT_ENDOFROUND = "Ende der Runde...";
	MSG_NEWROUND = "Neue Runde gestartet...";
	MSG_DISCONNECT = "%0 hat das Spiel verlassen";
	MSG_LOOSE = "%0s Spieler %1 ist raus";
	MSG_PLEASECHAT = "Nachricht eingeben...";

	CHAT_SAYS = ": ";
		
	CAPT_MAINMENU  = "IPCurve v1.1";
	CAPT_CLIENTMENU = "Zu Server verbinden";
	CAPT_CONFIGMENU = "Einstellungen";
	CAPT_LOBBY = "Lobby";

	CAPT_ERROR = "Fehler";
	
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

	CAPT_SSERVERPORT = "Server Port";
	CAPT_SSERVERDESC = "Serverbeschreibung";
	CAPT_SSERVERPASS = "Passwort";
	
	CAPT_STIMEOUT = "Rundenzeit";
	CAPT_SGAMESPEED = "Spielgeschwindigkeit";
	CAPT_SSPEEDMULTI = "Darstellungsqualitaet";
	CAPT_SENABLESPECIALS = "Spezialeigenschaften";
	CAPT_SENABLEHOLES = "Loecher";
	CAPT_SHOLEDIST = "Lochabstand";
	CAPT_SHOLEDISTRND = "Lochabst. Zufallsanteil";
	CAPT_SHOLESIZE = "Lochgroesse";
	CAPT_STURNANGLE = "Drehwinkel";

	CAPT_LEFTKEY = "Links";
	CAPT_RIGHTKEY = "Rechts";
	CAPT_SPECIALKEY = "Spezial";

	CAPT_REDCOMP = "Rot";
	CAPT_GREENCOMP = "Grun";
	CAPT_BLUECOMP = "Blau";
	
	CAPT_LANGENGLISH = "English language";
	CAPT_RENEWSERVERS = "Liste erneuern";
	CAPT_REGISTERMETA = "Beim Metaserver registrieren";
	CAPT_COLORCHANGE = "Farbe, Spieler %0";
	CAPT_KEY_LEFT = "Links-Taste, Spieler %0";
	CAPT_KEY_RIGHT = "Rechts-Taste, Spieler %0";
	CAPT_KEY_SPECIAL = "Spezial-Taste, Spieler %0";
	CAPT_SPECIAL = "Spezialeigenschaft, Spieler %0";
	CAPT_FULLSCREEN = "Vollbildmodus";
	CAPT_TOOLTIPS = "Tooltipps";
	CAPT_GAMEPROFILES = "Spielprofile";
		 
	CAPT_TAB_GENERAL = "Allgemein";
	CAPT_TAB_GAME = "Server: Spieleinstellungen";
	CAPT_TAB_SRV_NETWORK = "Server: Netzwerk";
	CAPT_TAB_PLAYERS = "Client: Spieler";

	CAPT_POPUP_OKAY = "Schon gut";
	CAPT_POPUP_CANCEL = "Lieber nicht";
	CAPT_POPUP_APPLY = "So sei es";
	CAPT_TITLE_EMPTY_STRING = "Nichts eingegeben";
	CAPT_TEXT_EMPTY_STRING = "Bitte gib erst etwas ein";

	TOOLTIP_NEWGAME = "Ein neues Spiel starten. Andere Spieler können mit 'Zu Server verbinden' daran teilnehmen.";
	TOOLTIP_JOINGAME = "An einem Spiel im lokalen Netzwerk (LAN) oder im Internet teilnehmen.";
	TOOLTIP_SETTINGS = "Die Spieleinstellungen und die Steuertasten der Spieler ansehen oder verändern.";
	TOOLTIP_EXIT = "";
	 
	TOOLTIP_FULLSCREEN = "Vollbildmodus aktivieren oder deaktivieren. Wenn der Vollbildmodus deaktiviert ist, wird IPCurve in einem 1024x768 Pixel grossen Fenster laufen. Nach dem Ändern dieser Einstellung muss IPCurve neu gestartet werden.";
	TOOLTIP_TOOLTIPS = "Diese Tooltipps aktivieren oder deaktivieren. Nach dem Ändern dieser Einstellung muss IPCurve neu gestartet werden.";

	TOOLTIP_USERNAME = "Dein Nickname. Andere Spieler sehen ihn, wenn sie über LAN/Internet an deinem Spiel teilnehmen.";
	TOOLTIP_S_PORT = "Der Port, an welchem der Server laufen soll. Eine eventuelle Firewall darf diesen Port nicht filtern. Meist ist 8080 eine gute Wahl.";
	TOOLTIP_S_DESC = "Eine kurze Beschreibung deines Servers. Andere Spieler sehen sie in der Serverliste.";
	TOOLTIP_S_PASS = "Ein Passwort, das jeder kennen muss, der an deinem Spiel teilnehmen will. Kann leer gelasen werden, woduch das Passwort deaktiviert wird.";
	TOOLTIP_S_TIMEOUT = "Gibt die Zeit (in Sekunden) an, nach deren Ablauf eine Runde automatisch abgebrochen wird. 0 dekativiert das Zeitlimit.";
	TOOLTIP_S_SPEED = "Gibt die Spielgeschwindigkeit an. Eine höhere Geschwindigkeit erhöht allerdings die Anforderungen an den Computer und an die Verbindung.";
	TOOLTIP_S_MULTI = "Gibt die Darstellungsqualität an. Falls die Steuerung nicht richtig reagiert, empfiehlt sich eine tiefere Einstellung.";
	TOOLTIP_S_SPECIALS = "Wenn aktiviert, kann sich jeder Spieler eine 'Spezialeigenschaft' aussuchen (Computerspieler suchen sich ihre Eigenschaft automatisch bei Spielbeginn aus).";
	TOOLTIP_S_HOLES = "Aktiviert/Deaktiviert Kurvenlöcher in unregelmässigen Abständen.";
	TOOLTIP_S_HOLE_DIST = "Der Grundabstand der Kurvenlöcher. Der tatsächliche Abstand ergibt sich aus Grundabstand und Zufallsanteil.";
	TOOLTIP_S_HOLE_DIST_RAND = "Der Zufallsanteil des Lochabstands. Je höher diese Einstellung, desto unregelmässiger treten die Löcher auf.";
	TOOLTIP_S_HOLE_SIZE = "Die Grösse der Kurvenlöcher.";
	TOOLTIP_S_ENGLISH = "Switch to english language. Restart the game after changing this setting.";
	TOOLTIP_S_METASERVER = "Dein Spiel beim 'Metaserver' registrieren. Andere Spieler können ein registriertes Spiel übers Internet sehen und sich daran beteiligen. Wenn du nur im Lokalen Netzwerk spielen willst, deaktiviere diese Option.";
	TOOLTIP_S_TURNANGLE = "Der Winkel, in dem die Kurven drehen können. Je höher diese Einstellung, desto engere Kurven kann man fahren.";

	TOOLTIP_C_COLORCHANGE = "Die Farbe von Spieler %0 (Rot-, Grün- und Blauanteil)";
	TOOLTIP_C_KEY_LEFT = "Die nach-Links-Taste von Spieler %0. Beliebige Taste drücken, um Zuordnung zu ändern";
	TOOLTIP_C_KEY_RIGHT = "Die nach-Rechts-Taste von Spieler %0. Beliebige Taste drücken, um Zuordnung zu ändern";
	TOOLTIP_C_KEY_SPECIAL = "Die Spezial-Taste von Spieler %0 - diese wird zum Auslösen der Spezialfähigkeit gebraucht. Beliebige Taste drücken, um Zuordnung zu ändern";
	TOOLTIP_C_SPECIAL = "Die Spezialeigenschaft von Spieler %0 - durch Klick ändern \n \n AKTUELLE EIGENSCHAFT \n %1";
	TOOLTIP_SPECIAL_DESC[0] = "Achtung, LEERE! \n Pech. Keine Eigenschaft.";
	TOOLTIP_SPECIAL_DESC[1] = "Achtung, die Wand! \n Erlaubt dir, in die Spielfeld-Wände zu fahren. Die Kurve wird dann am gegenüberliegenden Ende des Spielfelds fortgesetzt.";
	TOOLTIP_SPECIAL_DESC[2] = "Achtung, Tempo! \n Beschleunigt deine Kurve für kurze Zeit auf doppeltes Tempo.";
	TOOLTIP_SPECIAL_DESC[3] = "Achtung, die enge Kurve! \n Erlaubt es, für kurze Zeit wesentlich engere Kurven zu fahren.";
	TOOLTIP_SPECIAL_DESC[4] = "Achtung, Lücke! \n Erlaubt es, Löcher in anderen Kurven zu erzeugen. Fahre gerade auf die Zielkurve zu und löse die Eigenschaft aus.";
	TOOLTIP_SPECIAL_DESC[5] = "Achtung, Sprung! \n Ein Sprung vorwärts - auch über andere Kurven hinweg.";
	TOOLTIP_SPECIAL_DESC[6] = "Achtung, Schranke! \n Baut eine kurze Schranke im rechten Winkel zu deiner Kurve. Andere Spieler sollten sich davor in acht nehmen.";
	TOOLTIP_SPECIAL_DESC[7] = "Achtung, die kontinuierliche Kurve! \n Verhindert bei Bedarf Löcher in der eigenen Kurve. Unbegrenzt oft an- und abschaltbar.";
	TOOLTIP_SPECIAL_DESC[8] = "Achtung, die Ecke! \n Erlaubt das abbiegen im rechten Winkel. Erst die Eigenschaft einsetzen, dann eine der Richtungstasten drücken.";
	TOOLTIP_SPECIAL_DESC[9] = "Achtung, die fette Kurve! \n Verdickt deine Kurve für kurze Zeit.";
	TOOLTIP_SPECIAL_DESC[10] = "Achtung, da ist jemand! \n Macht deine Kurve für kurze Zeit unsichtbar. Ineffizient gegen Computerspieler.";

	CAPT_LOADPROFILE_CONFIRM = "Profil laden";
	TEXT_LOADPROFILE_CONFIRM = "Das Profil '%0' wirklich laden (die aktuell eingestellten Werte gehen verloren)?";
	CAPT_DELETEPROFILE_CONFIRM = "Profil loeschen";
	TEXT_DELETEPROFILE_CONFIRM = "Soll das Profil '%0' wirklich GELÖSCHT werden?";
	CAPT_SAVEPROFILE_CONFIRM = "Profil speichern";
	TEXT_SAVEPROFILE_CONFIRM = "Das Profil '%0' wird mit den aktuellen Werten überschrieben!";
	CAPT_ADDPROFILE = "Profil erzeugen";
	TOOLTIP_ADDPROFILE = "Die aktuellen Einstellungen in einem Profil abspeichern";
	TOOLTIP_GAMEPROFILES = "Verfügbare Spielprofile. \n Linke Maustaste: Profil laden \n Mittlere Maustaste: Profil löschen \n Rechte Maustaste: Profil speichern";
	CAPT_SAVEPROFILE_NAME = "Profil speichern";
	TEXT_SAVEPROFILE_NAME = "Bitte den Namen des Profils eingeben...";

	CAPT_TAB_LAN = "LAN";
	CAPT_TAB_INTERNET = "Internet";
	CAPT_TAB_CUSTOMIP = "Eigene IP";
	CAPT_SERVERNAME = "Name";
	CAPT_SERVERDESC = "Beschreibung";
	CAPT_SERVERHOST = "Server";
	CAPT_SERVERPORT = "Port";
	CAPT_SERVERPASS = "Passwort";
	CAPT_SERVERPING = "Ping";
	CAPT_SERVERPLAYERS = "Spieler";
	CAPT_SERVERLOOKUP = "Serversuche";
	TEXT_SERVERLOOKUP = "Suche nach Servern, bitte habe einen Moment Geduld...";
	YES = "Ja";
	NO = "Nein";
	CAPT_ENTERPW = "Passwort";
	TEXT_ENTERPW = "Bitte gib das Passwort ein, um an diesem Spiel teilzunehmen";

	CAPT_LOBBY_HELP = "(ESC: Zurück zum Hauptmenü, LEERTASTE: Spiel starten)";
	CAPT_RET_TO_CHAT = "RETURN drücken, um Chat-Nachricht einzugeben";
	CAPT_PLAYERS = "Spielerliste:";
	CAPT_CHAT = "Chatnachrichten:";
	CAPT_GAME_OVER = "Ausgekurvt";
}

void CCapt::setEnglish(){
	language = LANGUAGE_ENGLISH;

	PLAYER_UNUSED = "Player %0 (%1) is unused - press %2 to grab";
	PLAYER_MYCONTROL = "Player %0 (%1) is grabbed - press %2 to release";
	PLAYER_CONTROLLEDBY = "Player %0 (%1) is being controlled by %2";

	CONNECTINGTO = "Connecting to ";
	PORT = "Port ";
	WITHNAME = "as Player ";

	WAIT_DISCONNECTING = "Disconnecting...";
	WAIT_ENDOFROUND = "End of Round...";
	MSG_NEWROUND = "Next round started...";
	MSG_DISCONNECT = "%0 left the game";
	MSG_LOOSE = "%0s player %1 is out";
	MSG_PLEASECHAT = "Enter your chat message...";

	CHAT_SAYS = ": ";

	CAPT_MAINMENU  = "IPCurve v1.1";
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

	CAPT_ERROR = "Error";

	CAPT_CSERVERIP = "Server IP";
	CAPT_CSERVERPORT = "Port";
	CAPT_USERNAME = "NickName";

	CAPT_SSERVERPORT = "Server Port";
	CAPT_SSERVERDESC = "Server Description";
	CAPT_SSERVERPASS = "Password";
	CAPT_STIMEOUT = "Round Time";
	CAPT_SGAMESPEED = "Game Speed";
	CAPT_SSPEEDMULTI = "Display Quality";
	CAPT_SENABLESPECIALS = "Special abilities";
	CAPT_SENABLEHOLES = "Holes";
	CAPT_SHOLEDIST = "Hole Distance";
	CAPT_SHOLEDISTRND = "HoleDistanceRandom";
	CAPT_SHOLESIZE = "HoleSize";
	CAPT_STURNANGLE = "Turning angle";

	CAPT_TAB_GENERAL = "General";
	CAPT_TAB_GAME = "Server: Gameplay";
	CAPT_TAB_SRV_NETWORK = "Server: Network";
	CAPT_TAB_PLAYERS = "Client: Players";

	CAPT_LEFTKEY = "Left";
	CAPT_RIGHTKEY = "Right";
	CAPT_SPECIALKEY = "Special";

	CAPT_REDCOMP = "Red";
	CAPT_GREENCOMP = "Green";
	CAPT_BLUECOMP = "Blue";

	CAPT_COLORCHANGE = "Color, Player %0";
	CAPT_KEY_LEFT = "Left Key, Player %0";
	CAPT_KEY_RIGHT = "Right Key, Player %0";
	CAPT_KEY_SPECIAL = "Special Key, Player %0";
	CAPT_SPECIAL = "Special Ability, Player %0";
	CAPT_FULLSCREEN = "Fullscreen";
	CAPT_TOOLTIPS = "Tooltips";
	CAPT_GAMEPROFILES = "Game Profiles";

		 	
	CAPT_LANGENGLISH = "English language";
	CAPT_RENEWSERVERS = "Refresh List";
	CAPT_REGISTERMETA = "Register at Metaserver";
	
	CAPT_POPUP_OKAY = "All right";
	CAPT_POPUP_CANCEL = "Forget it";
	CAPT_POPUP_APPLY = "Okay";

	CAPT_TITLE_EMPTY_STRING = "Empty string";
	CAPT_TEXT_EMPTY_STRING = "Please enter a non-empty string first";

	TOOLTIP_NEWGAME = "Starts a new game. Other players will be able to join over the LAN or over the Internet.";
	TOOLTIP_JOINGAME = "Join a LAN/Internet game.";
	TOOLTIP_SETTINGS = "Change or view the game settings and the players' keys.";
	TOOLTIP_EXIT = "";
	 
	TOOLTIP_USERNAME = "Your nickname. Other players will see it if you host or join a LAN/Internet game.";
	TOOLTIP_S_PORT = "The port the game server is bound to. A firewall must not filter this port. Usually 8080 is all right.";
	TOOLTIP_S_DESC = "A short description of your servers, seen by other players in the server list.";
	TOOLTIP_S_PASS = "A password everyone who wants to join your game has to know. Leaving this empty disables the password.";
	TOOLTIP_S_TIMEOUT = "The maximum duration of one round. After this time (in seconds) has passed, the round is aborted. 0 disables the limit.";
	TOOLTIP_S_SPEED = "The game speed. A higher setting increases the CPU and the connection load.";
	TOOLTIP_S_MULTI = "The display quality. A lower settings decreases the CPU and the connection load - so try to lower this if you encounter control problems.";
	TOOLTIP_S_SPECIALS = "If enabled, each player may choose one special ability. (Bots will choose their ability automatically)";
	TOOLTIP_S_HOLES = "Enables or disables holes in the curves.";
	TOOLTIP_S_HOLE_DIST = "The basic distance of the holes. The real distance is calculated out of the basic value and the random part.";
	TOOLTIP_S_HOLE_DIST_RAND = "The random part of the hole distance. A higher setting makes the holes distributed less regular.";
	TOOLTIP_S_HOLE_SIZE = "The size of the holes.";
	TOOLTIP_S_ENGLISH = "Auf deutsche Sprache umschalten. Nach dem ändern dieser Einstellung muss das Spiel neu gestartet werden.";
	TOOLTIP_S_METASERVER = "Register your games at the internet metaserver. Other players can list all registered games online, making them able to participate. If you only want local or LAN games, disable this option.";
	TOOLTIP_S_TURNANGLE = "The turning angle of the curves. The higher this setting, the tighter are the curve's turns.";
	
	TOOLTIP_FULLSCREEN = "Enable or disable fullscreen video mode. If disabled, IPCurve will spawn a window of 1024x768 pixels size. Restart IPCurve after changing this setting.";
	TOOLTIP_TOOLTIPS = "Enable or disable these tooltips.  Restart IPCurve after changing this setting.";

	TOOLTIP_C_COLORCHANGE = "The color of player %0 (red-, green- and blue components)";
	TOOLTIP_C_KEY_LEFT = "The left key of player %0. Press any key to reassign";
	TOOLTIP_C_KEY_RIGHT = "The right key of player %0. Press any key to reassign";
	TOOLTIP_C_KEY_SPECIAL = "The special key of player %0 - used to fire the special ability. Press any key to reassign";
	TOOLTIP_C_SPECIAL = "The special ability of player %0 - click to change \n \n CURRENT ABILITY \n %1";
	TOOLTIP_SPECIAL_DESC[0] = "Achtung, NOTHINGNESS! \n Bad luck. No ability.";
	TOOLTIP_SPECIAL_DESC[1] = "Achtung, the wall! \n Allows you to drive trough the walls. The curve is continued at the other end of the playground";
	TOOLTIP_SPECIAL_DESC[2] = "Achtung, speed! \n Accelerates your curve to double speed for a short time.";
	TOOLTIP_SPECIAL_DESC[3] = "Achtung, the tight curve! \n Allows you to turn around very tightly for a short time.";
	TOOLTIP_SPECIAL_DESC[4] = "Achtung, hole! \n Creates holes in other curves. Drive straight towards your 'victim' and use this ability.";
	TOOLTIP_SPECIAL_DESC[5] = "Achtung, jump! \n A jump forward - even over other curves.";
	TOOLTIP_SPECIAL_DESC[6] = "Achtung, barrier! \n Builds a short barrier in a right angle to your curve. Other players, beware!";
	TOOLTIP_SPECIAL_DESC[7] = "Achtung, the continuous curve! \n Prevents (if wanted) holes in your curve. Can be toggled an unlimited number of times.";
	TOOLTIP_SPECIAL_DESC[8] = "Achtung, the edge! \n Allows turning around in angles of 90 degrees. First use the ability, then press one of your direction keys.";
	TOOLTIP_SPECIAL_DESC[9] = "Achtung, the fat curve! \n Supersize your curve for a short time.";
	TOOLTIP_SPECIAL_DESC[10] = "Achtung, someone there! \n Makes your curve invisible for some time. No effect on bots.";

	CAPT_LOADPROFILE_CONFIRM = "Load Profile";
	TEXT_LOADPROFILE_CONFIRM = "Really load the game profile '%0'? This will overwrite the current settings!";
	CAPT_DELETEPROFILE_CONFIRM = "Delete Profile";
	TEXT_DELETEPROFILE_CONFIRM = "Really DELETE the game profile '%0'?";
	CAPT_SAVEPROFILE_CONFIRM = "Save Profile";
	TEXT_SAVEPROFILE_CONFIRM = "Really overwrite the game profile '%0' with the current settings?";
	CAPT_ADDPROFILE = "Create profile";
	TOOLTIP_ADDPROFILE = "Save the current settings to a custom game profile";
	TOOLTIP_GAMEPROFILES = "Availible game profiles. \n Left click: Load profile \n Middle click: Delete profile \n Right click: Save profile";
	CAPT_SAVEPROFILE_NAME = "Save profile";
	TEXT_SAVEPROFILE_NAME = "Please enter the profile's name";

	CAPT_TAB_LAN = "LAN";
	CAPT_TAB_INTERNET = "Internet";
	CAPT_TAB_CUSTOMIP = "Custom IP";
	CAPT_SERVERNAME = "Name";
	CAPT_SERVERDESC = "Description";
	CAPT_SERVERHOST = "Server";
	CAPT_SERVERPORT = "Port";
	CAPT_SERVERPASS = "Password";
	CAPT_SERVERPING = "Ping";
	CAPT_SERVERPLAYERS = "Players";
	CAPT_SERVERLOOKUP = "Server lookup";
	TEXT_SERVERLOOKUP = "Looking for servers, one moment please...";
	YES = "Yes";
	NO = "No";
	CAPT_ENTERPW = "Password";
	TEXT_ENTERPW = "Please supply the password to join this game";
	
	CAPT_LOBBY_HELP = "(ESC: Back to the main menu, SPACE: Start game)";
	CAPT_RET_TO_CHAT = "Press RETURN to enter a chat message";
	CAPT_PLAYERS = "Players:";
	CAPT_CHAT = "Chat messages:";
	CAPT_GAME_OVER = "Game Over";
}

std::string CCapt::getClientErrorText(ClientError err)
{
	if(language == LANGUAGE_ENGLISH) {
		switch(err) {
			case ERR_META_NOT_FOUND:
				return "Could not establish a connection to the IPCurve Metaserver. Check your internet access.";
			case ERR_SERVER_NOT_FOUND:
				return "This server is unreachable. It might be down, so check the IP or refresh the server list.";
			case ERR_WRONG_PASSWORD:
				return "You supplied a wrong game password.";
			case ERR_SERVER_TIMEOUT:
				return "Connection to server timed out.";
			case NO_ERR:
				return "";
		}
	} else {
		switch(err) {
			case ERR_META_NOT_FOUND:
				return "Der IPCurve Metaserver konnte nicht erreicht werden. Besteht eine Internetverbindung?";
			case ERR_SERVER_NOT_FOUND:
				return "Dieser Server ist unerreichbar. Überprüfe die IP oder aktualisiere die Serverliste.";
			case ERR_WRONG_PASSWORD:
				return "Falsches Spiel-Passwort";
			case ERR_SERVER_TIMEOUT:
				return "Verbindung zum Server abgebrochen, Zeitüberschreitung";
			case NO_ERR:
				return "";
		}
	}
	return ""; 
}

std::string CCapt::getRankText(int rank, int score, int player, std::string controller, bool show_controller)
{
	std::string text = "";
	if(language == LANGUAGE_ENGLISH) {
		if(show_controller) {
			text = "%0. %1's player %2, %3 %4";
			text = aux::str_arg(aux::str_arg(aux::str_arg(aux::str_arg(aux::str_arg(text,
					aux::str_fromInt(rank)),
					controller),
					aux::str_fromInt(player)),
					aux::str_fromInt(score)),
					score==1?"point":"points");
		} else {
			text = "%0. Player %2, %3 %4";
			text = aux::str_arg(aux::str_arg(aux::str_arg(aux::str_arg(text,
					aux::str_fromInt(rank)),
					aux::str_fromInt(player)),
					aux::str_fromInt(score)),
					score==1?"point":"points");
		}
	} else {
		if(show_controller) {
			text = "%0. %1s Spieler %2, %3 %4";
			text = aux::str_arg(aux::str_arg(aux::str_arg(aux::str_arg(aux::str_arg(text,
					aux::str_fromInt(rank)),
					controller),
					aux::str_fromInt(player)),
					aux::str_fromInt(score)),
					score==1?"Punkt":"Punkte");
		} else {
			text = "%0. Spieler %2, %3 %4";
			text = aux::str_arg(aux::str_arg(aux::str_arg(aux::str_arg(text,
					aux::str_fromInt(rank)),
					aux::str_fromInt(player)),
					aux::str_fromInt(score)),
					score==1?"Punkt":"Punkte");
		}
	}
	return text;
}

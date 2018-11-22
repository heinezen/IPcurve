#include <stdio.h>
#include <string>
#include <iostream>
#include "clientgui.h"
#include "server.h"
#include "metaserver.h"
#include "config.h"
#include "settings.h"
#include "gamemenus.h"
#include "text.h"
#include "utils.h"

int main(int argc, char* argv[])
{
/*	if (enet_initialize() != 0)	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return 0;
	}*/
//	atexit(enet_deinitialize);
	CCapt::setEnglish();

	std::string path = argv[0];
	size_t slash_pos = path.rfind(PATH_DELIM);
	if(slash_pos == std::string::npos)
		path = "";
	else
		path = path.substr(0,slash_pos);
	std::string screenpath = path;	
	#ifdef __APPLE__
		// MacOS X Bundle Paths
		slash_pos = path.rfind(PATH_DELIM);
		if(slash_pos == std::string::npos){
			printf("Error getting real path. This might cause *serious* problems, including deadlocks!\n");
			//return 1;
		}else{
			path = path.substr(0,slash_pos);
			path += "/Resources";
			printf("path: %s\n",path.c_str());
		}
		screenpath = path;
		slash_pos = screenpath.rfind(PATH_DELIM);
		if(slash_pos == std::string::npos){
			printf("Error getting real path. This might cause *serious* problems, including deadlocks!\n");
		}else{
			screenpath = screenpath.substr(0,slash_pos);
            printf("screenshot path = %s \n",screenpath.c_str());
		}
		slash_pos = screenpath.rfind(PATH_DELIM);
		if(slash_pos == std::string::npos){
			printf("Error getting real path. This might cause *serious* problems, including deadlocks!\n");
		}else{
			screenpath = screenpath.substr(0,slash_pos);
            printf("screenshot path = %s \n",screenpath.c_str());
		}
        slash_pos = screenpath.rfind(PATH_DELIM);
		if(slash_pos == std::string::npos){
			printf("Error getting real path. This might cause *serious* problems, including deadlocks!\n");
		}else{
			screenpath = screenpath.substr(0,slash_pos);
            screenpath += "/";
            printf("screenshot path = %s \n",screenpath.c_str());
		}
        
	#endif
	path += PATH_DELIM;
	aux::data_path = path;
	aux::config_path = path;
	aux::screenshot_path = screenpath;    
	std::string inp;
	if(argc<2)
		inp = "menu";
	else
		inp = argv[1];
	if(inp=="server") {
		CServer* server = new CServer();
		CConfig* config = new CConfig();
		int port = atoi(config->s_port.c_str());
		printf("starting server...\n");
		delete config;
		if(server->listen(port)){
			while(!server->isFinished())
				server->update();
		}
		delete server;
	} else if(inp=="metaserver") {
		CMetaserver* server = new CMetaserver();
		server->listen();
		while(1)
			server->update();
		delete server;
	} else if(inp=="menu") {
		MainMenu m(0,0);
		m.execute();
	} else {
		printf("name\n");
		std::string name;
		name = argv[2];
		bool fullscreen = true;
		std::string fs = "";
		if(argc>3)
			fs = argv[3];
		if(fs=="no")
			fullscreen = false;
		ClientGUI* client = new ClientGUI(0,fullscreen);
		printf("connecting...\n");
		client->execute(inp,DEFAULT_PORT,name);
		delete client;
	}
	printf("ENDOFAPP\n");
	return 0;
}

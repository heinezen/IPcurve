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

int main(int argc, char* argv[])
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return 0;
	}
	atexit(enet_deinitialize);
	CCapt::setEnglish();


	std::string inp;
	if(argc<2)
		inp = "menu";
	else
		inp = argv[1];
	if(inp=="server")
	{
		CServer* server = new CServer();
		printf("starting server...\n");
		if(server->listen(DEFAULT_PORT)){
			while(!server->isFinished())
			{
				server->update();
			}
		}
		delete server;
	}
	else if(inp=="metaserver")
	{
		CMetaserver* server = new CMetaserver();
		server->listen();
		while(1){
			server->update();
		}
		delete server;
	}
	else if(inp=="menu"){
		MainMenu m(0,0);
		m.execute();
	}else{
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
}

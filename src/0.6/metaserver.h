#ifndef METASERVER_H
#define METASERVER_H
#include <stdio.h>
#include <string>
#include <vector>
#include <enet/enet.h>
#include "config.h"

class CServerInfo{
	public:
		CServerInfo(unsigned int Ip, short Port, std::string Name);
		unsigned int ip;
		short port;
		std::string name;
};

class CMetaserver{
	public:
		CMetaserver();
		~CMetaserver();
		void listen();
		void update(int timeout = METASERVERS_TIMEOUT);
	private:
		std::vector<CServerInfo> servers;
		ENetHost* socket;
};

#endif

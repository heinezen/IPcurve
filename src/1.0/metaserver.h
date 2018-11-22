#ifndef METASERVER_H
#define METASERVER_H
#include <stdio.h>
#include <string>
#include <map>

#include <raknet/MessageIdentifiers.h>
#include <raknet/RakNetworkFactory.h>
#include <raknet/RakPeerInterface.h>
#include <raknet/RakNetStatistics.h>
#include <raknet/RakNetTypes.h>
#include <raknet/RakSleep.h>

#include "config.h"

class CServerInfo {
	public:
		CServerInfo(SystemAddress Addr, std::string Name, std::string Desc, bool pass);
		int lastPong;
		int missedPings;
		SystemAddress addr;
		std::string host;
		int port;
		std::string name;
		std::string desc;
		bool password;
		bool isLocal;
};

class CMetaserver {
	public:
		CMetaserver();
		~CMetaserver();
		void close();
		void listen();
		void update();
	private:
		std::map<SystemAddress,CServerInfo*> servers;
		RakPeerInterface* peer;
};

#endif

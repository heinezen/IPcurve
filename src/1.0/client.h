#ifndef CLIENT_H
#define CLIENT_H
#include <vector>
#include <string>
#include <stdio.h>

#include <raknet/MessageIdentifiers.h>
#include <raknet/RakNetworkFactory.h>
#include <raknet/RakPeerInterface.h>
#include <raknet/RakNetStatistics.h>
#include <raknet/RakNetTypes.h>
#include <raknet/RakSleep.h>

#include "config.h"
class ClientGUI;

enum ClientError {
	NO_ERR,
	ERR_SERVER_NOT_FOUND,
	ERR_META_NOT_FOUND,
	ERR_WRONG_PASSWORD,
	ERR_SERVER_TIMEOUT
};

struct ServerInfo{
	std::string host;
	int port;
	std::string name;
	std::string desc;
	bool pass;
};

typedef std::vector<ServerInfo> ServerList;

class CClient{
	public:
		CClient(ClientGUI* GUI = 0);
		~CClient();
		ServerList findLanHosts();
		ServerList findMetaserverHosts(std::string metaserver);
		bool connect(std::string host,int port,std::string name,std::string password = "");
		bool disconnect();
		bool connected();
		void update();
		void chat_msg(std::string);
		void lobby_take(const unsigned char* request);
		void sendKeys(const unsigned char* request);
		void request(unsigned char req);
		ClientError getError();
		bool isLocalServer();
	private:
		bool localServer;
		ClientGUI* gui;
		RakPeerInterface* peer;
		ClientError lastError;
		void processMessage(int size, void* data);

};
#endif

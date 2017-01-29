#ifndef CLIENT_H
#define CLIENT_H
#include <vector>
#include <string>
#include <stdio.h>
#include <enet/enet.h>
#include "config.h"
class ClientGUI;

class CClient{
	public:
		CClient(ClientGUI* GUI = 0);
		~CClient();
		std::vector<std::string> findLanHosts();
		std::vector<std::string> findMetaserverHosts(std::string metaserver);
		bool connect(std::string host,int port,std::string name);
		bool disconnect();
		bool connected();
		void update(int timeout = CLIENT_TIMEOUT);
		void chat_msg(std::string);
		void lobby_take(const unsigned char* request);
		void sendKeys(const unsigned char* request);
		void request(unsigned char req);
	private:
		ClientGUI* gui;
		ENetHost* socket;
		ENetPeer* peer;
		void processMessage(int size, void* data);

};
#endif

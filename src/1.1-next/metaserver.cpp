#include <stdio.h>
#include <string>
#include "config.h"
#include "message.h"
#include "metaserver.h"
#include "utils.h"

CServerInfo::CServerInfo(SystemAddress Addr, std::string Name, std::string Desc, bool pass)
{
	addr = Addr;

	password = pass;
	name = Name;
	desc = Desc;

	host = addr.ToString();
	int port_pos = host.rfind(":");
	host = host.substr(0,port_pos);
	port = addr.port;

	int period_pos = host.find(".");
	int ip_first = aux::str_toInt(host.substr(0,period_pos).c_str());
	printf("/8 net is %d \n",ip_first);
	isLocal = (ip_first == 127);

	lastPong = SDL_GetTicks();
	missedPings = 0;
}


CMetaserver::CMetaserver()
{
	peer = RakNetworkFactory::GetRakPeerInterface();
	printf("This is the IPCurve metaserver, our version ID is %x\n",VERSION);
}

CMetaserver::~CMetaserver()
{
	close();
	RakNetworkFactory::DestroyRakPeerInterface(peer);
}

void CMetaserver::close()
{
	if(peer->IsActive())
		peer->Shutdown(300);
}

void CMetaserver::listen()
{
	close();
	SocketDescriptor socketDescriptor(METASERVER_PORT,0);
	peer->SetIncomingPassword(0,0);
	bool b = peer->Startup(METASERVERS_CONNECTIONS,15,&socketDescriptor,1);
	peer->SetMaximumIncomingConnections(METASERVERS_CONNECTIONS);
	if(!b)
		printf("error opening meta-server! \n");
}

void CMetaserver::update()
{
	std::map<SystemAddress,CServerInfo*>::iterator it = servers.begin();
	std::map<SystemAddress,CServerInfo*>::iterator it2;
	while(it != servers.end()) {
		CServerInfo* info = it->second;
		if(!info) {
			it2 = it++;
			servers.erase(it2);
			continue;
		}
		int lastPongDelta = SDL_GetTicks() - info->lastPong;
		if(lastPongDelta > METASERVERS_CHECK_REPING) {
			info->lastPong = SDL_GetTicks();
			info->missedPings++;
			if(info->missedPings > METASERVERS_MAX_MISSED_PINGS) {
				printf("dropped %s as he missed 6 pings \n",info->name.c_str());
				delete info;
				it2 = it++;
				servers.erase(it2);
				continue;
			} else 
				peer->Ping(info->host.c_str(),info->port,true);
		}
		++it;
	}

	RakSleep(20);
	while(1) {
		RakSleep(3);
		Packet* packet = peer->Receive();
		if(!packet)
			break;
		int id = GetPacketIdentifier(packet);
		//printf("got a packet, ID is %d \n",id);
		if(id == ID_PONG) {
			CServerInfo* info = servers[packet->systemAddress];
			if(info) {
				info->lastPong = SDL_GetTicks();
				info->missedPings = 0;
				peer->Ping(info->host.c_str(),info->port,true);
			}
		} else if(id == MESSAGE_METASERVER_REQUEST) {
			printf("got a request. responding...\n");
			MessageMetaserverRequest* req = (MessageMetaserverRequest*)packet->data;
			if(req->version != VERSION) {
				MessageMetaserverReply rep;
				snprintf(rep.server,128,"ERROR");
				rep.port = 42;
				snprintf(rep.desc,MAX_DESC_LENGTH+1,"Visit achtungkurve.ch.vu for an update.");
				snprintf(rep.name,MAX_NICK_LENGTH+1,"Your client is OLD.");
				peer->Send((char*)&rep,sizeof(rep),HIGH_PRIORITY,RELIABLE_ORDERED,0,packet->systemAddress,false);
			} else {
				std::map<SystemAddress,CServerInfo*>::iterator it;
				for(it = servers.begin(); it != servers.end(); it++) {
					CServerInfo* info = it->second;
					if(!info)
						continue;
					MessageMetaserverReply rep;
					snprintf(rep.name,MAX_NICK_LENGTH+1,"%s",info->name.c_str());
					snprintf(rep.desc,MAX_DESC_LENGTH+1,"%s",info->desc.c_str());
					rep.port = info->port;
					rep.password = info->password;
					if(info->isLocal)
						snprintf(rep.server,128,"%s",DEFAULT_METASERVER);
					else
						snprintf(rep.server,128,"%s",info->host.c_str());
					peer->Send((char*)&rep,sizeof(rep),HIGH_PRIORITY,RELIABLE_ORDERED,0,packet->systemAddress,false);
				}
				
			}
		} else if(id == MESSAGE_METASERVER_ADDSERVER) {
			MessageMetaserverAddServer* add = (MessageMetaserverAddServer*)packet->data;
			if(add->version==VERSION) {
				SystemAddress hostAddress = packet->systemAddress;
				hostAddress.port = add->port;
				bool isDup = false;
				std::map<SystemAddress,CServerInfo*>::iterator it;
				for(it = servers.begin();it != servers.end();it++) {
					CServerInfo* info = it->second;
					if(!info)
						continue;
					if(info->addr==hostAddress) {
						printf("duplicate register from %s\n",hostAddress.ToString());
						isDup = true;
					}
				}
				if(!isDup) {
					servers[hostAddress] = new CServerInfo(hostAddress,std::string(add->serverName),std::string(add->desc),add->password);
					printf("registered %s from %s pass: %d\n",add->serverName,hostAddress.ToString(),add->password);
				}
			}
		} else if(id == MESSAGE_METASERVER_REMOVESERVER) {
			MessageMetaserverRemoveServer* rem = (MessageMetaserverRemoveServer*)packet->data;
			std::string name = rem->serverName;
			if(rem->version==VERSION) {
				std::map<SystemAddress,CServerInfo*>::iterator it;
				for(it = servers.begin();it != servers.end();it++) {
					CServerInfo* info = it->second;
					if(!info)
						continue;
					if(info->name == name)
						it->second = 0;
				}
			}
		}
		peer->DeallocatePacket(packet);
	}
}



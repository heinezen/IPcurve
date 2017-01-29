#include <stdio.h>
#include <string>
#include <enet/enet.h>
#include "config.h"
#include "message.h"
#include "metaserver.h"

CServerInfo::CServerInfo(unsigned int Ip, short Port, std::string Name)
{
	ip = Ip;
	port = Port;
	name = Name;
}

CMetaserver::CMetaserver()
{
	printf("This is IPCurve Metaserver, version %x\n",VERSION);
	socket = 0;
}

CMetaserver::~CMetaserver()
{
	if(socket!=0){
		enet_host_destroy(socket);
	}
}

void CMetaserver::listen()
{
	ENetAddress addr;
	addr.host = ENET_HOST_ANY;
	addr.port = METASERVER_PORT;
	socket = enet_host_create(&addr,METASERVERS_CONNECTIONS,0,0);
	servers.clear();
	printf("Awaiting connections...\n");
}

void CMetaserver::update(int timeout)
{
	ENetEvent event;
	while(enet_host_service (socket, &event, timeout) > 0)
	{
		if(event.type==ENET_EVENT_TYPE_RECEIVE){
			MessageBase* base = (MessageBase*)event.packet->data;
			if(base->mtype==MESSAGE_METASERVER_REQUEST){
				MessageMetaserverRequest* req = (MessageMetaserverRequest*)event.packet->data;
				MessageMetaserverReply rep;
				unsigned char parts[4];
				for(int i=0;i<servers.size();i++){
					if(req->version!=VERSION){
						sprintf(rep.server,"Your client is <br> OUT OF DATE");
						i=servers.size()-1;
					}else{
						parts[0] = ((servers[i].ip)&0xFF000000)>>24;
						parts[1] = ((servers[i].ip)&0x00FF0000)>>16;
						parts[2] = ((servers[i].ip)&0x0000FF00)>>8;
						parts[3] = ((servers[i].ip)&0x000000FF);
						sprintf(rep.server, "%s <br> %d.%d.%d.%d   %d",servers[i].name.c_str(),parts[3],parts[2],parts[1],parts[0],servers[i].port);
					}
					ENetPacket* packet = enet_packet_create(&rep,
							sizeof(rep),
							ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(event.peer, 0, packet);
				}
			}else if(base->mtype==MESSAGE_METASERVER_ADDSERVER){
				MessageMetaserverAddServer* add = (MessageMetaserverAddServer*)event.packet->data;
				if(add->version==VERSION){
					servers.push_back(CServerInfo(event.peer->address.host,add->port,std::string(add->serverName)));
				}
			}else if(base->mtype==MESSAGE_METASERVER_REMOVESERVER){
				MessageMetaserverRemoveServer* rem = (MessageMetaserverRemoveServer*)event.packet->data;
				if(rem->version==VERSION){
					std::vector<CServerInfo>::iterator it;
					for(it = servers.begin();it != servers.end();it++){
						CServerInfo inf = *it;
						if(inf.ip==event.peer->address.host){
							servers.erase(it);
						}
					}
				}
			}
			enet_packet_destroy(event.packet);
		}
	}
}



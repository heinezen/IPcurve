#include <stdio.h>
#include <string>
#include <enet/enet.h>
#include "client.h"
#include "clientgui.h"
#include "config.h"
#include "message.h"

CClient::CClient(ClientGUI* GUI):socket(0),peer(0)
{
	gui = GUI;
}

CClient::~CClient()
{
	if(socket != 0||peer != 0)
		disconnect();
}

bool CClient::connected()
{
	return (peer != 0);
}
std::vector<std::string> CClient::findMetaserverHosts(std::string metaserver)
{
	std::vector<std::string> result;
	if(socket != 0||peer != 0)
		disconnect();
	
	socket = enet_host_create(0,1,0,0);
	if(socket == 0)
		return result;
	
	ENetAddress add;
	enet_address_set_host(&add,metaserver.c_str());
	add.port = METASERVER_PORT;
	
	enet_host_connect(socket,&add,1);
	
	ENetEvent event;
	while(enet_host_service(socket, &event, METASERVER_TIMEOUT))
	{
		if(event.type==ENET_EVENT_TYPE_CONNECT){
			MessageMetaserverRequest r;
			ENetPacket* packet = enet_packet_create(&r,
					sizeof(r),
					ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(socket, 0, packet);
			enet_host_flush(socket);
		}
		if(event.type==ENET_EVENT_TYPE_RECEIVE){
			MessageBase* b = (MessageBase*)(event.packet->data);
			if(b->mtype!=MESSAGE_METASERVER_REPLY)
				continue;
			MessageMetaserverReply* h = (MessageMetaserverReply*)(event.packet->data);
			result.push_back(std::string(h->server));
			enet_packet_destroy(event.packet);
		}
	}
	
	enet_host_destroy(socket);
	
	socket = 0;
	peer = 0;
	
	return result;	
}

std::vector<std::string> CClient::findLanHosts()
{
	std::vector<std::string> result;
	if(socket != 0||peer != 0)
		disconnect();
	ENetAddress add;
	add.host = ENET_HOST_ANY;
	add.port = INFO_PORT;
	ENetHost* bch = enet_host_create(&add,
												 CLIENT_MAX_HOSTS,
												 0,
												 0);
	
	socket = enet_host_create(0,
									  CLIENT_MAX_HOSTS,
									  0,
									  0);
	if(socket == 0)
		return result;
	if(bch == 0)
		return result;
	
	ENetAddress address;
	//enet_address_set_host(&address, ENET_HOST_ANY);
	address.host = ENET_HOST_BROADCAST;
	address.port = LOOKUP_PORT;
	enet_host_connect(socket, &address, 1);
	enet_host_service(socket, 0, 1);
	unsigned char parts[4];
	char buffer[128];
	
	ENetEvent event;
	while(enet_host_service(bch, &event, LOOKUP_TIMEOUT))
	{
		//printf("test111\n");
		//enet_host_service(socket, 0, 50);
		if(event.type==ENET_EVENT_TYPE_RECEIVE){
			MessageBase* b = (MessageBase*)(event.packet->data);
			if(b->mtype!=MESSAGE_HELLO)
				continue;
			MessageHello* h = (MessageHello*)(event.packet->data);
			std::string sname = h->serverName;
			parts[0] = ((event.peer->address.host)&0xFF000000)>>24;
			parts[1] = ((event.peer->address.host)&0x00FF0000)>>16;
			parts[2] = ((event.peer->address.host)&0x0000FF00)>>8;
			parts[3] = ((event.peer->address.host)&0x000000FF);
			sprintf(buffer, "%d.%d.%d.%d   %d",parts[3],parts[2],parts[1],parts[0],h->port);
			result.push_back(sname+" <br> "+std::string(buffer)+"");
			enet_peer_reset(event.peer);
			enet_packet_destroy(event.packet);
		}
	}
	//enet_peer_reset(peer);
	enet_host_destroy(socket);
	
	enet_host_destroy(bch);
	socket = 0;
	peer = 0;
	return result;
}

bool CClient::connect(std::string host,int port,std::string uname)
{
	if(socket != 0||peer != 0)
		disconnect();
	socket = enet_host_create(0,
	                          CLIENT_CONNECTIONS,
	                          CLIENT_IN_LIMIT,
	                          CLIENT_OUT_LIMIT);
	ENetAddress address;
	ENetEvent event;

	enet_address_set_host(&address, host.c_str());
	address.port = port;

	peer = enet_host_connect(socket, &address, 2);
	if(peer == 0)
	{
		disconnect();
		return false;
	}
	if(enet_host_service(socket, &event, CONNECT_TIMEOUT) <= 0 ||
	    event.type != ENET_EVENT_TYPE_CONNECT)
	{
		enet_peer_reset(peer);
		peer = 0;
		disconnect();
	        return false;
	}
	MessageConnect conn;
	memcpy(&conn.name,uname.c_str(),uname.length()+1);
	ENetPacket* packet = enet_packet_create(&conn,
                                                sizeof(conn),
                                                ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 1, packet);
	enet_host_flush(socket);	
	
	return true;
}

void CClient::chat_msg(std::string m){
	MessageChat mess;
	memcpy(&mess.text,m.c_str(),m.length()+1);
	ENetPacket* packet = enet_packet_create(&mess,
                                                sizeof(mess),
                                                ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 1, packet);
	enet_host_flush(socket);	
}

void CClient::lobby_take(const unsigned char* request)
{
	CConfig c;
	MessageLobbyTake mess;
	for(int i=0;i<MAX_PLAYERS;i++){
		mess.takereq[i] = request[i];
		mess.special[i] = c.special[i];
	}
	ENetPacket* packet = enet_packet_create(&mess,
														  sizeof(mess),
														  ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 1, packet);
	enet_host_flush(socket);	
}

bool CClient::disconnect()
{
	if(peer != 0)
	{
		enet_peer_disconnect(peer,0);
		ENetEvent event;
		bool s = false;
		int i = 0;
		while(enet_host_service(socket,&event,DISCONNECT_TIMEOUT)>0&&!s)
		{
			switch(event.type)
			{
				case ENET_EVENT_TYPE_RECEIVE:
					enet_packet_destroy(event.packet);
				break;

				case ENET_EVENT_TYPE_DISCONNECT:
					s = true;
				break;
			}
		}
		if(!s)
			enet_peer_reset(peer);
		peer = 0;
	}
	if(socket != 0)
	{
		enet_host_destroy(socket);
		socket = 0;
		return true;
	}
	return false;
}

void CClient::processMessage(int size, void* data)
{
	if(size == 0) return;
	MessageBase* mess = (MessageBase*)data;
	if(mess->mtype==MESSAGE_ERROR){
		printf("THE SERVER sent a wrong packet");
	}
	else if(mess->mtype==MESSAGE_CONNECT){
		printf("THE SERVER sent a wrong (connect type) packet");
	}
	else if(mess->mtype==MESSAGE_LOBBYTAKE){
		printf("THE SERVER sent a wrong (lobby take type) packet");
	}
	else if(mess->mtype==MESSAGE_CLIENTSYNC){
		printf("THE SERVER sent a wrong (client sync type) packet");
	}
	else if(mess->mtype==MESSAGE_REQUEST){
		printf("THE SERVER sent a wrong (client request type) packet");
	}
	else if(mess->mtype==MESSAGE_CHAT){
		MessageChat* chat = (MessageChat*)data;
//		std::string mess(chat->text);
		if(gui) gui->chat(chat->text);
	}
	else if(mess->mtype==MESSAGE_LOBBYSTATE){
		MessageLobbyState* state = (MessageLobbyState*)data;
		if(gui) gui->lobby_sync(state);
	}
	else if(mess->mtype==MESSAGE_SERVERSYNC){
		MessageServerSync* state = (MessageServerSync*)data;
		if(gui) gui->gameSync(state);
	}
	else if(mess->mtype==MESSAGE_GAMEINIT){
		MessageGameInit* e = (MessageGameInit*)data;
		if(gui) gui->game_init(e);
	}
	else if(mess->mtype==MESSAGE_LINE){
		MessageLine* m = (MessageLine*)data;
		if(gui) gui->DrawLine(m->x1,m->y1,m->x2,m->y2,m->w,m->col);
	}
	else if(mess->mtype==MESSAGE_SCORE){
		MessageScore* e = (MessageScore*)data;
		if(gui) gui->updateScoreboard(e->scores,e->loads);
	}
	else if(mess->mtype==MESSAGE_EVENT){
		MessageEvent* e = (MessageEvent*)data;
		if(e->event == EVENT_WELCOME){
			if(gui) gui->start_lobby();
		}
		else if(e->event == EVENT_STARTGAME){
			if(gui) gui->start_game();
		}
		else if(e->event == EVENT_STARTROUND){
			if(gui) gui->start_round();
		}
		else if(e->event == EVENT_ENDROUND){
			if(gui) gui->end_round();
		}
		else if(e->event == EVENT_ENDGAME){
			if(gui) gui->end_game();
		}
	}
}

void CClient::sendKeys(const unsigned char* request)
{
	MessageClientSync mess;
	memcpy(&mess.keys[0],request,sizeof(mess.keys));
	ENetPacket* packet = enet_packet_create(&mess,
														  sizeof(mess),
														  ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 1, packet);
//	enet_host_flush(socket);		
}


void CClient::update(int timeout)
{
	ENetEvent event;
	while(enet_host_service(socket, &event, timeout) > 0)
	{
		if(gui) {
			if(gui->getState()==CSTATE_ABORT)
				return;
		}
		switch(event.type)
		{
			case ENET_EVENT_TYPE_RECEIVE:
				processMessage(event.packet->dataLength,
				               event.packet->data);
				enet_packet_destroy(event.packet);
			break;
			case ENET_EVENT_TYPE_DISCONNECT:
				peer = 0;
				disconnect();
				return;
			break;
		}
	}
}

void CClient::request(unsigned char req)
{
	MessageRequest mess;
	mess.request = req;
	ENetPacket* packet = enet_packet_create(&mess,
														  sizeof(mess),
														  ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 1, packet);
	enet_host_flush(socket);	
}

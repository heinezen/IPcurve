#include <stdio.h>
#include <string>

#include "client.h"
#include "clientgui.h"
#include "config.h"
#include "message.h"

CClient::CClient(ClientGUI* GUI)
{
	peer = RakNetworkFactory::GetRakPeerInterface();
	gui = GUI;
	lastError = NO_ERR;
	localServer = false;
	//DumpMessageCodes();
}

CClient::~CClient()
{
	disconnect();
	RakNetworkFactory::DestroyRakPeerInterface(peer);
}

ClientError CClient::getError()
{
	ClientError result = lastError;
	lastError = NO_ERR;
	return result;
}

bool CClient::connected()
{
	return peer->IsConnected(peer->GetSystemAddressFromIndex(0));
}

void CClient::checkServers(ServerList& servers)
{
	if(servers.size() == 0)
		return;
	if(servers[0].host == "ERROR")
		return;
	RakPeerInterface* infoPeer = RakNetworkFactory::GetRakPeerInterface();
	SocketDescriptor descriptor(INFO_PORT,0);
	infoPeer->Startup(1,30,&descriptor,1);
	
	MessageInfoRequest req;
	req.timestamp = SDL_GetTicks();
	printf("sending info queries... \n");
	ServerList::iterator it;
	for(it = servers.begin(); it != servers.end(); it++)
		infoPeer->AdvertiseSystem(it->host.c_str(),it->port,(char*)&req,sizeof(req));

	unsigned int end_time = SDL_GetTicks() + SERVER_INFO_TIMEOUT;
	while(SDL_GetTicks() < end_time) {
		RakSleep(5);
		Packet* p = infoPeer->Receive();
		if(!p)
			continue;
		if(GetPacketIdentifier(p) == ID_ADVERTISE_SYSTEM) {
			if(p->length < (sizeof(MessageInfoReply)+1))
				continue;
			MessageInfoReply* rep = (MessageInfoReply*)(p->data+1);
			if(rep->mtype != MESSAGE_INFO_REPLY)
				continue;
			std::string host = GetHost(p->systemAddress);
			int port = p->systemAddress.port;
			printf("got a reply from %s port %d. delay %d, players %d\n",host.c_str(),port,SDL_GetTicks()-rep->timestamp,rep->players);
			bool all_done = true;
			for(it = servers.begin(); it != servers.end(); it++) {
				if(it->host == host && it->port == port) {
					it->ping = (SDL_GetTicks()-rep->timestamp)/2;
					it->players = rep->players;
				}
				if(it->ping == -1)
					all_done = false;
			}
			if(all_done)
				break;
		}
		infoPeer->DeallocatePacket(p);
	}
	
	infoPeer->Shutdown(50);
	RakNetworkFactory::DestroyRakPeerInterface(infoPeer);
}

ServerList CClient::findMetaserverHosts(std::string metaserver)
{
	ServerList result;
	lastError = ERR_META_NOT_FOUND;

	if(metaserver=="")
		return result;
	RakPeerInterface* metaPeer = RakNetworkFactory::GetRakPeerInterface();
	SocketDescriptor descriptor(0,0);
	metaPeer->Startup(1,30,&descriptor,1);
	bool b = metaPeer->Connect(metaserver.c_str(),METASERVER_PORT,0,0);
	if(!b) {
		printf("no metaserver reachable, sorry.\n");
	} else {
		unsigned int end_time = SDL_GetTicks() + METASERVER_TIMEOUT;
		while(SDL_GetTicks() < end_time) {
			RakSleep(30);
			Packet* p = metaPeer->Receive();
			if(!p)
				continue;
			if(GetPacketIdentifier(p) == ID_CONNECTION_REQUEST_ACCEPTED) {
				MessageMetaserverRequest req;
				metaPeer->Send((char*)&req,sizeof(req),HIGH_PRIORITY,RELIABLE_ORDERED,0,p->systemAddress,false);
				lastError = NO_ERR;
				end_time += METASERVER_TIMEOUT;
			} else if(GetPacketIdentifier(p) == MESSAGE_METASERVER_REPLY) {
				MessageMetaserverReply* rep = (MessageMetaserverReply*)p->data;
				ServerInfo info;
				info.host = rep->server;
				info.port = rep->port;
				info.name = rep->name;
				info.desc = rep->desc;
				info.pass = rep->password;
				info.players = -1;
				info.ping = -1;
				result.push_back(info);
			}
			metaPeer->DeallocatePacket(p);
		}
	}
	metaPeer->Shutdown(METASERVER_TIMEOUT);
	RakNetworkFactory::DestroyRakPeerInterface(metaPeer);
	return result;
/*	if(socket != 0||peer != 0)
		disconnect();
	
	socket = enet_host_create(0,1,0,0);
	if(socket == 0)
		return result;
	
	ENetAddress add;
	enet_address_set_host(&add,metaserver.c_str());
	add.port = METASERVER_PORT;
	
	enet_host_connect(socket,&add,1);
	
	lastError = ERR_META_NOT_FOUND;
	
	ENetEvent event;
	while(enet_host_service(socket, &event, METASERVER_TIMEOUT)) {
		if(event.type==ENET_EVENT_TYPE_CONNECT) {
			printf("querying metaserver...\n");
			MessageMetaserverRequest r;
			ENetPacket* packet = enet_packet_create(&r,
													sizeof(r),
													ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(event.peer,0,packet);
			enet_host_flush(socket);
			lastError = NO_ERR;
		}
		if(event.type==ENET_EVENT_TYPE_RECEIVE) {
			printf("parsing metaserver answer ...\n");			
			MessageBase* b = (MessageBase*)(event.packet->data);
			if(b->mtype != MESSAGE_METASERVER_REPLY)
				continue;
			MessageMetaserverReply* h = (MessageMetaserverReply*)(event.packet->data);
			ServerInfo info;
			info.host = h->server;
			info.port = h->port;
			info.name = h->name;
			info.desc = h->desc;
			info.pass = h->password;
			result.push_back(info);
			enet_packet_destroy(event.packet);
		}
	}
	
	enet_host_destroy(socket);
	
	socket = 0;
	peer = 0;
	
	return result;*/	
}

ServerList CClient::findLanHosts()
{
	ServerList result;
	RakPeerInterface* lookupPeer = RakNetworkFactory::GetRakPeerInterface();
	SocketDescriptor socketDescriptor(0,0);
	lookupPeer->Startup(1,30,&socketDescriptor,1);
	lookupPeer->Ping("255.255.255.255",LOOKUP_PORT,true);
	lookupPeer->Ping("localhost",LOOKUP_PORT,true);
	unsigned int end_time = SDL_GetTicks() + LOOKUP_TIMEOUT;
	while(SDL_GetTicks() < end_time) {
		RakSleep(30);
		Packet* p = lookupPeer->Receive();
		if(!p)
			continue;
		if(GetPacketIdentifier(p) == ID_PONG) {
			RakNetTime time;
			memcpy((char*)&time, p->data+1, sizeof(RakNetTime));
			printf("Got pong from %s\n", p->systemAddress.ToString());
			// CONNECT THERE, PARSE REPLY
			RakPeerInterface* infoPeer = RakNetworkFactory::GetRakPeerInterface();
			SocketDescriptor descriptor(0,0);
			infoPeer->Startup(1,30,&descriptor,1);
			std::string host = GetHost(p->systemAddress);
			infoPeer->Connect(host.c_str(),LOOKUP_PORT,0,0);
			unsigned int end_time_info = SDL_GetTicks() + INFO_TIMEOUT;
			while(SDL_GetTicks() < end_time_info) {
				RakSleep(10);
				Packet* packet = infoPeer->Receive();
				if(!packet)
					continue;
				if(GetPacketIdentifier(packet) == MESSAGE_HELLO) {
					MessageHello* msg = (MessageHello*)packet->data;
					ServerInfo info;
					info.name = msg->serverName;
					info.desc = msg->desc;
					info.host = host.c_str();
					info.port = msg->port;
					info.pass = msg->password;
					info.players = -1;
					info.ping = -1;
					result.push_back(info);
					break;
				}
				infoPeer->DeallocatePacket(packet);
			}
			infoPeer->Shutdown(200);
			RakNetworkFactory::DestroyRakPeerInterface(infoPeer);

			//lookupPeer->Connect(p->systemAddress.binaryAddress,LOOKUP_PORT,0,0);
			end_time += INFO_TIMEOUT + 200;
		}
		lookupPeer->DeallocatePacket(p);

	}
	lookupPeer->Shutdown(DISCONNECT_TIMEOUT);
	RakNetworkFactory::DestroyRakPeerInterface(lookupPeer);
	return result;
/*	if(socket != 0||peer != 0)
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
	//address.host = 0x0100007f;
	address.host = ENET_HOST_BROADCAST;
	address.port = LOOKUP_PORT;
	enet_host_connect(socket, &address, 1);
	enet_host_service(socket, 0, 1);
	unsigned char parts[4];
	char buf[128];
	ENetEvent event;
	while(enet_host_service(bch, &event, LOOKUP_TIMEOUT)) {
		//printf("test111\n");
		//enet_host_service(socket, 0, 50);
		if(event.type==ENET_EVENT_TYPE_RECEIVE){
			MessageBase* b = (MessageBase*)(event.packet->data);
			if(b->mtype!=MESSAGE_HELLO)
				continue;
			MessageHello* h = (MessageHello*)(event.packet->data);
			ServerInfo info;
			info.name = h->serverName;
			info.desc = h->desc;
			//std::string sname = h->serverName;
			parts[0] = ((event.peer->address.host)&0xFF000000)>>24;
			parts[1] = ((event.peer->address.host)&0x00FF0000)>>16;
			parts[2] = ((event.peer->address.host)&0x0000FF00)>>8;
			parts[3] = ((event.peer->address.host)&0x000000FF);
			snprintf(buf,128,"%d.%d.%d.%d",parts[3],parts[2],parts[1],parts[0]);
			info.host = buf;
			info.port = h->port;
			info.pass = h->password;
			result.push_back(info);
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
*/
}

bool CClient::isLocalServer()
{
	return localServer;
}

bool CClient::connect(std::string host,int port,std::string uname,std::string password)
{
	if(connected())
		disconnect();
	SocketDescriptor socketDescriptor(0,0);
	peer->Startup(1,10,&socketDescriptor,1);
	bool b = peer->Connect(host.c_str(),port,password.c_str(),password.length()+1);	
	if(b) {
		printf("ok, connecting\n");
	} else {
		printf("failed!\n");
	}
	bool done = false;
	lastError = NO_ERR;
	while(!done) {
		RakSleep(30);
		
		Packet* p = peer->Receive();

	
		if (p==0)
			continue;
			
		unsigned char packetIdentifier = GetPacketIdentifier(p);

		switch (packetIdentifier) {
			case ID_CONNECTION_BANNED: // Banned from this server
				printf("We are banned from this server.\n");
				lastError = ERR_WRONG_PASSWORD;
				disconnect();
				return false;
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed\n");
				lastError = ERR_SERVER_NOT_FOUND;
				disconnect();
				return false;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full, srry.\n");
				lastError = ERR_SERVER_NOT_FOUND;
				disconnect();
				return false;
			case ID_INVALID_PASSWORD:
				printf("Wrong pass!\n");
				lastError = ERR_WRONG_PASSWORD;
				disconnect();
				return false;

			case ID_CONNECTION_LOST:
				printf("Timeout!\n");
				lastError = ERR_SERVER_TIMEOUT;
				disconnect();
				return false;

			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("Connection Accepted!\n");
				done = true;
				break;
		}


		// We're done with the packet
		peer->DeallocatePacket(p);
	}
	printf("sending a nice hello now\n");
	MessageConnect conn;
	snprintf(conn.name,MAX_NICK_LENGTH+1,"%s",uname.c_str());
	peer->Send((char*)&conn,sizeof(conn),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
	if ((peer->GetSystemAddressFromIndex(0).binaryAddress & 0x000000FF) == 127)
		localServer = true;
/*	socket = enet_host_create(0,
	                          CLIENT_CONNECTIONS,
	                          CLIENT_IN_LIMIT,
	                          CLIENT_OUT_LIMIT);
	ENetAddress address;
	ENetEvent event;

	enet_address_set_host(&address, host.c_str());
	address.port = port;
	
	// if server is in 127.0.0.0/8, the server is local
	if ((address.host & 0x000000FF) == 127)
		localServer = true;
	
	if(localServer)
		printf("using a local server \n");
	
	peer = enet_host_connect(socket, &address, 2);
	if(peer == 0) {
		lastError = ERR_SERVER_NOT_FOUND;
		disconnect();
		return false;
	}
	if(enet_host_service(socket, &event, CONNECT_TIMEOUT) <= 0 ||
	    event.type != ENET_EVENT_TYPE_CONNECT) {
		enet_peer_reset(peer);
		peer = 0;
		lastError = ERR_SERVER_NOT_FOUND;
		disconnect();
	        return false;
	}
	MessageConnect conn;
	snprintf(conn.name,MAX_NICK_LENGTH+1,"%s",uname.c_str());
	snprintf(conn.pass,MAX_PASS_LENGTH+1,"%s",password.c_str());
	ENetPacket* packet = enet_packet_create(&conn,
											sizeof(conn),
											ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer,1,packet);
	enet_host_flush(socket);	
	*/
	return true;
}

void CClient::chat_msg(std::string m){
	MessageChat mess;
	snprintf(mess.text,256,"%s",m.c_str());
	peer->Send((char*)&mess,sizeof(mess),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
}

void CClient::lobby_take(const unsigned char* request)
{
	CConfig c;
	MessageLobbyTake mess;
	for(int i=0;i<MAX_PLAYERS;i++) {
		mess.takereq[i] = request[i];
		mess.special[i] = c.special[i];
	}
	peer->Send((char*)&mess,sizeof(mess),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
}

bool CClient::disconnect()
{
/*	if(peer != 0) {
		enet_peer_disconnect(peer,0);
		ENetEvent event;
		bool s = false;
		while(enet_host_service(socket,&event,DISCONNECT_TIMEOUT)>0&&!s) {
			switch(event.type) {
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
	if(socket != 0)	{
		enet_host_destroy(socket);
		socket = 0;
		return true;
	}*/
	peer->CloseConnection(UNASSIGNED_SYSTEM_ADDRESS,true);
	peer->Shutdown(DISCONNECT_TIMEOUT);
	return true;
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
		//printf("chat msg: %s\n",chat->text);
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
//	printf("cli: message-type: %d\n",mess->mtype);
}

void CClient::sendKeys(const unsigned char* request)
{
	MessageClientSync mess;
	memcpy(&mess.keys[0],request,sizeof(mess.keys));
	peer->Send((char*)&mess,sizeof(mess),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
//	enet_host_flush(socket);		
}


void CClient::update()
{
	RakSleep(5);
	while(1) {
		//RakSleep(5);
		//printf("cli: loop %d \n",++a);
		Packet* packet = peer->Receive();
		if(!packet)
			return;
		processMessage(packet->length,packet->data);
	//printf("cli: packet type %d\n",GetPacketIdentifier(packet));
		peer->DeallocatePacket(packet);
	}
/*	ENetEvent event;
	while(enet_host_service(socket, &event, timeout) > 0) {
		if(gui) {
			if(gui->getState()==CSTATE_ABORT)
				return;
		}
		switch(event.type) {
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
	}*/
}

void CClient::request(unsigned char req)
{
	MessageRequest mess;
	mess.request = req;
	peer->Send((char*)&mess,sizeof(mess),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
}

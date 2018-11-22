#include <stdio.h>
#include <string>
#include "server.h"
#include "config.h"
#include "message.h"
#include "game.h"
#include "text.h"
#include "snake.h"
#include "settings.h"
#include "utils.h"

CServer::CServer()
{
	
	peer = RakNetworkFactory::GetRakPeerInterface();
	lookupPeer = RakNetworkFactory::GetRakPeerInterface();
	game = 0;
	finished = false;
	CConfig cfg;
	roundTimeout = cfg.s_timeout;
	spdMulti = cfg.s_spdMulti;
	roundWantedDelay = (100-cfg.s_gameSpeed/10)*spdMulti/2.3;
	allowSpecials = cfg.s_enableSpecials;
	enableHoles = cfg.s_enableHoles;
	holeDist = cfg.s_holeDist;
	holeDistRnd = cfg.s_holeRnd;
	holeSize = cfg.s_holeSize;
	servernick = cfg.nick;
	serverpass = cfg.s_password;
	turnAngle = cfg.s_turnAngle;
	metaserver = cfg.s_registermeta?cfg.metaserver:"";
	memcpy(&(botAlg[0]),&(cfg.botAlg[0]),sizeof(botAlg));
	
	serverdesc = cfg.s_desc;
}

CServer::~CServer()
{
	CConfig cfg;
	memcpy(&(cfg.botAlg[0]),&(botAlg[0]),sizeof(botAlg));
	if(game != 0)
		endGame();
	if(peer->IsActive())
		close();
	if(lookupPeer->IsActive())
		stopLookup();
	RakNetworkFactory::DestroyRakPeerInterface(peer);
	RakNetworkFactory::DestroyRakPeerInterface(lookupPeer);
}

void CServer::sendToMetaserver(char* mess, int mess_size)
{
	printf("registering my game at %s \n",metaserver.c_str());
	if(metaserver=="")
		return;

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
				printf("connection accepted by the metaserver, sending request %d \n", mess[0]);
				metaPeer->Send(mess,mess_size,HIGH_PRIORITY,RELIABLE_ORDERED,0,p->systemAddress,false);
			}
			metaPeer->DeallocatePacket(p);
		}
	}
	printf("registration done!\n");
	metaPeer->Shutdown(METASERVER_TIMEOUT);
	RakNetworkFactory::DestroyRakPeerInterface(metaPeer);

}

void CServer::stopLookup()
{
	MessageMetaserverRemoveServer m;
	snprintf(m.serverName,MAX_NICK_LENGTH+1,"%s",servernick.c_str());
	sendToMetaserver((char*)&m,sizeof(m));

	if(lookupPeer->IsActive())
		lookupPeer->Shutdown(DISCONNECT_TIMEOUT);
}

bool CServer::isFinished()
{
	return finished;
}

void CServer::endGame()
{
	if(!game)
		return;
	game->endGame();
	printf("end of game\n");
	throwEvent(EVENT_ENDROUND);
	throwEvent(EVENT_ENDGAME);
	std::map<SystemAddress,PlayerInfo*>::iterator it;
	for(it = player_info.begin();it != player_info.end();it++) {
		PlayerInfo* info = it->second;
		if(info)
			game->kickPlayer(info);
	}
/*	for(unsigned int i=0;i<socket->peerCount;i++) {
		if(socket->peers[i].data==0)
			continue;
		game->kickPlayer((PlayerInfo*)(socket->peers[i].data));
	}*/
	delete game;
	game = 0;
	printf("!!! ending game!!! \n");
	finished = true;
}

void CServer::reqEndGame()
{
	if(game) {
		std::map<SystemAddress,PlayerInfo*>::iterator it;
		for(it = player_info.begin();it != player_info.end();it++){
			PlayerInfo* info = it->second;
			if(info)
				game->kickPlayer(info);
		}
		game->endGame();
		throwEvent(EVENT_ENDROUND);
		throwEvent(EVENT_ENDGAME);
	}
	finished = true;
	printf("!!! ending game request !!! \n");
}


bool CServer::listen(int port)
{
	printf("opening server... \n");
	gameport = port;
	close();
	SocketDescriptor socketDescriptor(port,0);
	peer->SetIncomingPassword(serverpass.c_str(),serverpass.length()+1);
	bool b = peer->Startup(MAX_CONNECTIONS,10,&socketDescriptor,1);
	peer->SetMaximumIncomingConnections(MAX_CONNECTIONS);
	if(!b) {
		printf("error opening server! \n");
		close();
		return false;
	}
	
	SocketDescriptor socketDescriptor2(LOOKUP_PORT,0);
	lookupPeer->SetIncomingPassword(0,0);
	b = lookupPeer->Startup(MAX_CONNECTIONS,30,&socketDescriptor2,1);
	lookupPeer->SetMaximumIncomingConnections(MAX_CONNECTIONS);
	if(!b)
		printf("error opening ping discovery server! \n");
		
	MessageMetaserverAddServer m;
	snprintf(m.serverName,MAX_NICK_LENGTH+1,"%s",servernick.c_str());
	snprintf(m.desc,MAX_DESC_LENGTH+1,"%s",serverdesc.c_str());
	m.port = gameport;
	m.password = ((serverpass.length())>0);
	sendToMetaserver((char*)&m,sizeof(m));

	for(int k=0;k<MAX_PLAYERS;k++) {
		controller[k] = 0xFFFF;
	}

	return peer->IsActive();
}

bool CServer::close()
{
	stopLookup();
	if(peer->IsActive())
		peer->Shutdown(DISCONNECT_TIMEOUT);
	return true;
}

void CServer::processMessage(Packet* packet)
{
	if(packet->length==0)
		return;

	PlayerInfo* info = player_info[packet->systemAddress];

	void* data = packet->data;
	MessageBase* mess = (MessageBase*)data;
	if(mess->mtype==MESSAGE_ERROR) {
		if(info)
			printf("%s sent me an errorous packet\n",info->name.c_str());
	} else if(mess->mtype==MESSAGE_CONNECT) {
		MessageConnect* con = (MessageConnect*)data;
		if(game) {
			peer->CloseConnection(packet->systemAddress,true);
			return;
		}

		bool id[MAX_CONNECTIONS];
		for(int i=0;i<MAX_CONNECTIONS;i++)
			id[i] = true;
		std::map<SystemAddress,PlayerInfo*>::iterator it;
		for(it = player_info.begin();it != player_info.end();it++){
			PlayerInfo* info = it->second;
			if(info)
				id[info->id] = false;
		}
		int tid = -1;
		for(int x=0;x<MAX_CONNECTIONS;x++){
			if(id[x]){
				tid = x;
				break;
			}
		}
		if(tid != -1){
			player_info[packet->systemAddress] = new PlayerInfo("Anonymous",packet->systemAddress,tid);
		}else{
			printf("!!!!! BUG IN server.cpp, no free ID's! This should be caught in RakNet !!!!!!\n");
			peer->CloseConnection(packet->systemAddress,true);
		}
		info = player_info[packet->systemAddress];
		if(!info)
			return;
		info->name = con->name;
		if(info->name == "")
			info->name = "Anonymous";
		broadcast_chat_msg("*** "+info->name+" CONNECTED ***");
		throwEvent(EVENT_WELCOME,info->address);
		lobby_sync();
		printf("%s connected\n",info->name.c_str());
	} else if(mess->mtype==MESSAGE_CHAT) {
		if(!info)
			return;
		MessageChat* chat = (MessageChat*)data;
		std::string mess(chat->text);
		broadcast_chat_msg(info->name+CCapt::CHAT_SAYS+mess);
	} else if(mess->mtype==MESSAGE_CLIENTSYNC) {
		if(!info)
			return;
		MessageClientSync* sync = (MessageClientSync*)data;
		if(game)
			game->processMessage(sync,info);
	} else if(mess->mtype==MESSAGE_LOBBYTAKE) {
		if(!info)
			return;
		MessageLobbyTake* sync = (MessageLobbyTake*)data;
		if(game == 0){
			for(int i=0; i<MAX_PLAYERS; i++){
				if(controller[i]==0xFFFF&&sync->takereq[i]==3){
					special[i]=allowSpecials?sync->special[i]:ABILITY_NONE;
					controller[i]=info->id;
				}else if(controller[i]==info->id&&sync->takereq[i]==3){
					special[i]=allowSpecials?ABILITY_BOTSCHOICE:ABILITY_NONE;
					controller[i]=-botAlg[i];
				}else if((controller[i]==info->id||controller[i]<0)&&(sync->takereq[i]==1)){
					special[i]=ABILITY_NONE;
					controller[i]=0xFFFF;
				}else if(controller[i]<0&&sync->takereq[i]==3){
					controller[i]--;
					if(controller[i]<-BOTALG_MAX)
						controller[i] = -1;
					botAlg[i] = -controller[i];
				}

			}
			lobby_sync();
		}
	} else if(mess->mtype==MESSAGE_REQUEST) {
		if(info==0) return;
		MessageRequest* mess = (MessageRequest*)data;
		unsigned char req=mess->request;
		if(req==REQUEST_STARTGAME){
			if(game==0)
				startGame();
		}
		if(req==REQUEST_STARTROUND){
			if(game!=0&&game->getState()==STATE_ENDOFROUND)
				startRound();
		}
	} else if(mess->mtype==ID_DISCONNECTION_NOTIFICATION || mess->mtype==ID_CONNECTION_LOST) {
		if(!info) 
			return;
		broadcast_chat_msg(aux::str_arg(CCapt::MSG_DISCONNECT,info->name));
		if(!game) {
			for(int i=0;i<MAX_PLAYERS;i++) {
				if(controller[i]==info->id)
					controller[i]=0xFFFF;
			}
		}else{
			game->kickPlayer(info);
			if(game->getState()==STATE_ENDOFGAME) {
				reqEndGame();
			}
		}
		printf("%s disconected.\n", info->name.c_str());
		delete info;
		player_info[packet->systemAddress] = 0;
		if(!game)
			lobby_sync();
	} else if(mess->mtype==ID_ADVERTISE_SYSTEM) {
		if(packet->length < sizeof(MessageInfoRequest)+1)
			return;
		MessageInfoRequest* req = (MessageInfoRequest*)(packet->data+1);
		if(req->mtype != MESSAGE_INFO_REQUEST)
			return;
		int player_count = 0;
		std::map<SystemAddress,PlayerInfo*>::iterator it;
		for(it = player_info.begin();it != player_info.end();it++)
			if(it->second)
				player_count++;
		printf("got an info query. there are %d players here \n",player_count);
		MessageInfoReply rep;
		rep.timestamp = req->timestamp;
		rep.players = player_count;
		peer->AdvertiseSystem(GetHost(packet->systemAddress).c_str(),packet->systemAddress.port,(char*)&rep,sizeof(rep));
	}
//	printf("srv: message type: %d \n",mess->mtype);
/*	else if(mess->mtype==MESSAGE_METASERVER_PING){
		if(metaserverPing != -1)
			metaserverPing = 0;
		if(game != 0)
			return;
		MessageMetaserverPing ping;
		ENetPacket* packet = enet_packet_create(&ping,
												sizeof(ping),
												ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet);
		enet_host_flush(socket);
	}*/
}

void CServer::lobby_sync()
{
	MessageLobbyState lobby;
	for(int k=0;k<MAX_PLAYERS;k++) {
		lobby.controllers[k][0] = 0;
		lobby.yourcontrol[k] = false;
	}
	for(int k=0;k<MAX_CONNECTIONS;k++)
		lobby.players[k][0] = 0;
		
	std::map<SystemAddress,PlayerInfo*>::iterator it;
	for(it = player_info.begin();it != player_info.end();it++) {
		if(!it->second)
			continue;
		PlayerInfo* p = it->second;
		snprintf(lobby.players[p->id],MAX_NICK_LENGTH+1,"%s",p->name.c_str());
		for(int k=0;k<MAX_PLAYERS;k++) {
			if(controller[k]<0){
				std::string str = "";
				if(controller[k]==-BOTALG_NORMAL)
					str = "normal";
				if(controller[k]==-BOTALG_ENHANCED)
					str = "enhanced";
				if(controller[k]==-BOTALG_FORCEFIELD)
					str = "mr. force";
				if(controller[k]==-BOTALG_DUMB)
					str = "dumb";
				snprintf(lobby.controllers[k],MAX_NICK_LENGTH+1,"CURVEBOT, %s", str.c_str());
			}
			if(controller[k]==p->id) {
				snprintf(lobby.controllers[k],MAX_NICK_LENGTH+1,"%s",p->name.c_str());
			}
		}
	}
	for(it = player_info.begin();it != player_info.end();it++) {
		if(!it->second)
			continue;
		PlayerInfo* p =it->second;
		SystemAddress addr = p->address;
		for(int k=0;k<MAX_PLAYERS;k++) {
			if(controller[k]==p->id) {
				lobby.yourcontrol[k] = true;
			} else {
				lobby.yourcontrol[k] = false;
			}
		}
		peer->Send((char*)&lobby,sizeof(lobby),HIGH_PRIORITY,RELIABLE_ORDERED,0,addr,false);

	}
}

void CServer::broadcast_chat_msg(std::string m)
{
	//printf("chat message: %s",m.c_str());
	MessageChat mess;
	snprintf(mess.text,256,"%s",m.c_str());
	peer->Send((char*)&mess,sizeof(mess),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
}

void CServer::update(int delay)
{
	if(delay < 5)
		delay = 5;
	RakSleep(delay);
	while(1) {
		//printf("srv: loop %d \n",++a);
		//RakSleep(5);
		Packet* packet = peer->Receive();
		if(!packet)
			break;
		processMessage(packet);
	//printf("srv: packet type %d\n",GetPacketIdentifier(packet));
		peer->DeallocatePacket(packet);
	}
	if(lookupPeer->IsActive()) {
		while(1) {
			Packet* packet = lookupPeer->Receive();
			if(!packet)
				break;
			if(GetPacketIdentifier(packet) == ID_NEW_INCOMING_CONNECTION) {
				printf("Got a query. Sending a hello as a reply to %s\n", packet->systemAddress.ToString());
				MessageHello mess;
				snprintf(mess.serverName,MAX_NICK_LENGTH+1,"%s",servernick.c_str());
				snprintf(mess.desc,MAX_DESC_LENGTH+1,"%s",serverdesc.c_str());
				mess.port = gameport;
				mess.password = (serverpass.length() > 0);
				lookupPeer->Send((char*)&mess,sizeof(mess),HIGH_PRIORITY,RELIABLE_ORDERED,0,packet->systemAddress,false);
			}
			peer->DeallocatePacket(packet);
		}
	}
/*	ENetEvent event;
	if(metaserverPing > -1)
		metaserverPing += delay;
	if(game != 0)
		metaserverPing = -1;
	if(metaserverPing>METASERVER_PING_TIMEOUT) {
		printf("connection to meta timed out, so let's re-register\n");
		MessageMetaserverAddServer m;
		snprintf(m.serverName,MAX_NICK_LENGTH+1,"%s",servernick.c_str());
		m.port = gameport;
		m.password = (serverpass.length() > 0);
		sendToMetaserver(&m);
		metaserverPing = 0;
	}
	
	while(enet_host_service (socket, &event, delay) > 0) {
//		if(finished)
//			return;
		switch(event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				printf("A new client connected from %x:%u.\n", 
				       event.peer->address.host,
				       event.peer->address.port);
				event.peer->data = 0;
			break;
			case ENET_EVENT_TYPE_RECEIVE:
				if(event.peer->data == 0){
					processMessage(event.packet->dataLength,
										event.packet->data,
										0,
										event.peer);
				}else{
					processMessage(event.packet->dataLength,
					               event.packet->data,
										(PlayerInfo*)event.peer->data,
										event.peer);
				}
				enet_packet_destroy(event.packet);
			break;
			case ENET_EVENT_TYPE_DISCONNECT:
				if(event.peer->data == 0) 
					break;
				broadcast_chat_msg(aux::str_arg(CCapt::MSG_DISCONNECT,((PlayerInfo*)event.peer->data)->name));
				if(game == 0){
					for(int i=0;i<MAX_PLAYERS;i++) {
						if(controller[i]==((PlayerInfo*)event.peer->data)->id)
							controller[i]=0xFFFF;
					}
				}else{
					game->kickPlayer((PlayerInfo*)event.peer->data);
					if(game->getState()==STATE_ENDOFGAME) {
						reqEndGame();
					}
				}
				printf("%s disconected.\n", ((PlayerInfo*)event.peer->data)->name.c_str());
				delete ((PlayerInfo*)event.peer->data);
				event.peer->data = 0;
				if(game == 0)
					lobby_sync();
			break;
		}
	}
	if(lookupSocket) {
		while(enet_host_service(lookupSocket, &event, 1) > 0) {
			if(event.type==ENET_EVENT_TYPE_RECEIVE) {
				enet_packet_destroy(event.packet);
			}
		}
		for(unsigned int i=0;i<lookupSocket->peerCount;i++) {
			if(lookupSocket->peers[i].address.host != 0) {
				ENetPeer* peer = &lookupSocket->peers[i];
				printf("A client from %x:%u sent a broadcast request\n", 
						 peer->address.host,
						 peer->address.port);
				ENetHost* infoChannel = enet_host_create(0,1,0,0);
				ENetAddress address;
				address.host = peer->address.host;
				address.port = INFO_PORT;
				ENetPeer* infoPeer = enet_host_connect(infoChannel, &address, 1);
				ENetEvent ev;
				while(enet_host_service(infoChannel, &ev, 100)>0){
					if(ev.type==ENET_EVENT_TYPE_CONNECT){
						MessageHello mess;
						snprintf(mess.serverName,MAX_NICK_LENGTH+1,"%s",servernick.c_str());
						snprintf(mess.desc,MAX_DESC_LENGTH+1,"%s",serverdesc.c_str());
//						memcpy(&mess.serverName,servernick.c_str(),servernick.length()+1);
						mess.port = gameport;
						mess.password = (serverpass.length() > 0);
						ENetPacket* packet = enet_packet_create(&mess,
																sizeof(mess),
																ENET_PACKET_FLAG_RELIABLE);
						enet_host_broadcast(infoChannel, 0, packet);
						enet_host_flush(infoChannel);
					}
					if(ev.type==ENET_EVENT_TYPE_RECEIVE){
						enet_packet_destroy(event.packet);
					}
				}
				enet_peer_reset(infoPeer);
				enet_host_destroy(infoChannel);
				enet_peer_disconnect(peer,0);
				peer->address.host = 0;
			}
		}
//			printf("test\n");
	}*/
}

void CServer::startGame()
{
	if(game)
		return;

	bool some_one_in = false;
	for(int i=0;i<MAX_PLAYERS;i++) {
		if(controller[i] != 0xFFFF) {
			some_one_in = true;
			break;
		}
	}
	if(!some_one_in)
		return;
	stopLookup();
	game = new Game(this);
	
	for(int i=0;i<MAX_PLAYERS;i++){
		Snake* snake = 0;
		if(controller[i]==0xFFFF)
			continue;
		if(controller[i]<0){
			if(controller[i]==-BOTALG_NORMAL){
				snake = new NormalBotSnake(0,game,spdMulti,special[i],enableHoles?holeSize:0,holeDist,holeDistRnd,STDWIDTH,turnAngle);
			}
			if(controller[i]==-BOTALG_ENHANCED){
				snake = new EnhancedBotSnake(0,game,spdMulti,special[i],enableHoles?holeSize:0,holeDist,holeDistRnd,STDWIDTH,turnAngle);
			}
			if(controller[i]==-BOTALG_FORCEFIELD){
				snake = new ForcefieldBotSnake(0,game,spdMulti,special[i],enableHoles?holeSize:0,holeDist,holeDistRnd,STDWIDTH,turnAngle);
			}
			if(controller[i]==-BOTALG_DUMB){
				snake = new DumbBotSnake(0,game,spdMulti,special[i],enableHoles?holeSize:0,holeDist,holeDistRnd,STDWIDTH,turnAngle);
			}
		}else{
			PlayerInfo* info = 0;
			std::map<SystemAddress,PlayerInfo*>::iterator it;
			for(it = player_info.begin();it != player_info.end();it++) {
				if(!it->second)
					continue;
				PlayerInfo* t = it->second;
				if(t->id==controller[i]){
					info = t;
					break;
				}
			}
			if(info==0)
				continue;
			snake = new Snake(info,game,spdMulti,special[i],enableHoles?holeSize:0,holeDist,holeDistRnd,STDWIDTH,turnAngle);
		}
		game->addPlayer(snake,i);
	}
	
	MessageGameInit settings;
	
	throwEvent(EVENT_STARTGAME);
	
	settings.xsize = game->getWidth();
	settings.ysize = game->getHeight();
	peer->Send((char*)&settings,sizeof(settings),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
	
	startRound();
}

void CServer::startRound()
{
	throwEvent(EVENT_STARTROUND);
	printf("wanted delay: %d\n",roundWantedDelay);
	game->startRound(roundTimeout,roundWantedDelay);
	if(game->getState()==STATE_ENDOFGAME){
		endGame();
	}else{
		throwEvent(EVENT_ENDROUND);
	}
}

void CServer::throwEvent(int event, SystemAddress rec){
	MessageEvent mess;
	mess.event = event;
	if(rec == UNASSIGNED_SYSTEM_ADDRESS)
		peer->Send((char*)&mess,sizeof(mess),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
	else
		peer->Send((char*)&mess,sizeof(mess),HIGH_PRIORITY,RELIABLE_ORDERED,0,rec,false);
}

void CServer::sendScoreboard(signed char* scores, signed char* loads)
{
	MessageScore mess;
	for(int i=0;i<MAX_PLAYERS;i++){
		mess.scores[i] = scores[i];
		mess.loads[i] = loads[i];
	}
	peer->Send((char*)&mess,sizeof(mess),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
}

void CServer::sync(MessageServerSync* m)
{
	//////////////////////// MAKING THIS UNRELIABLE _MIGHT_ IMPROVE PERFORMANCE!
	peer->Send((char*)m,sizeof(*m),HIGH_PRIORITY,UNRELIABLE,0,UNASSIGNED_SYSTEM_ADDRESS,true);
}

void CServer::DrawLine(short x1, short y1, short x2, short y2, unsigned char w, unsigned char val)
{
	MessageLine mess;
	mess.x1 = x1;
	mess.y1 = y1;
	mess.x2 = x2;
	mess.y2 = y2;
	mess.w = w;
	mess.col = val;
	peer->Send((char*)&mess,sizeof(mess),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);

}

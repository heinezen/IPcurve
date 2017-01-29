#include <stdio.h>
#include <string>
#include <enet/enet.h>
#include "server.h"
#include "config.h"
#include "message.h"
#include "game.h"
#include "text.h"
#include "snake.h"
#include "settings.h"

CServer::CServer()
{
	
	socket = 0;
	lookupSocket = 0;
	game = 0;
	finished = false;
	CConfig cfg;
	roundTimeout = cfg.s_timeout;
	roundWantedDelay = (100-cfg.s_gameSpeed/10);
	spdMulti = cfg.s_spdMulti;
	allowSpecials = cfg.s_enableSpecials;
	enableHoles = cfg.s_enableHoles;
	holeDist = cfg.s_holeDist;
	holeDistRnd = cfg.s_holeRnd;
	holeSize = cfg.s_holeSize;
	servernick = cfg.nick;
	metaserver = cfg.s_registermeta?cfg.metaserver:"";
	memcpy(&(botAlg[0]),&(cfg.botAlg[0]),sizeof(botAlg));
}

CServer::~CServer()
{
	CConfig cfg;
	memcpy(&(cfg.botAlg[0]),&(botAlg[0]),sizeof(botAlg));
	if(game != 0)
		endGame();
	if(socket != 0)
		close();
	if(lookupSocket != 0)
		stopLookup();
}

void CServer::sendToMetaserver(MessageBase* mess)
{
	if(metaserver=="")
		return;
	MessageMetaserverAddServer* add = mess->mtype==MESSAGE_METASERVER_ADDSERVER?
	                                  (MessageMetaserverAddServer*)mess:0;
	MessageMetaserverRemoveServer* remove = mess->mtype==MESSAGE_METASERVER_REMOVESERVER?
			                                  (MessageMetaserverRemoveServer*)mess:0;
	ENetHost* sock = enet_host_create(0,1,0,0);
	if(sock == 0)
		return;
	
	ENetAddress addr;
	enet_address_set_host(&addr,metaserver.c_str());
	addr.port = METASERVER_PORT;
	
	enet_host_connect(sock,&addr,1);
	
	ENetEvent event;
	while(enet_host_service(sock, &event, METASERVER_TIMEOUT))
	{
		if(event.type==ENET_EVENT_TYPE_CONNECT){
			ENetPacket* packet = 0;
			if(add){
				packet = enet_packet_create(add,
						sizeof(*add),
						ENET_PACKET_FLAG_RELIABLE);
			}else{
				packet = enet_packet_create(remove,
						sizeof(*remove),
						ENET_PACKET_FLAG_RELIABLE);
			}
			enet_host_broadcast(sock, 0, packet);
			enet_host_flush(sock);
			enet_peer_disconnect(event.peer,0);
		}
		if(event.type==ENET_EVENT_TYPE_RECEIVE){
			enet_packet_destroy(event.packet);
		}
	}
	
	enet_host_destroy(sock);
	
}

void CServer::stopLookup(){
	MessageMetaserverRemoveServer m;
	sendToMetaserver(&m);
	if(lookupSocket == 0)
		return;
	enet_host_destroy(lookupSocket);
	lookupSocket = 0;
}

bool CServer::isFinished(){
	return finished;
}

void CServer::endGame()
{
	if(game == 0) return;
	game->endGame();
	printf("end of game\n");
	throwEvent(EVENT_ENDROUND);
	throwEvent(EVENT_ENDGAME);
	for(int i=0;i<socket->peerCount;i++){
		if(socket->peers[i].data==0)
			continue;
		game->kickPlayer((PlayerInfo*)(socket->peers[i].data));
	}
	delete game;
	game = 0;
	finished = true;
}
void CServer::reqEndGame()
{
	if(game == 0) return;
	game->endGame();
	throwEvent(EVENT_ENDROUND);
	throwEvent(EVENT_ENDGAME);
	for(int i=0;i<socket->peerCount;i++){
		if(socket->peers[i].data==0)
			continue;
		game->kickPlayer((PlayerInfo*)(socket->peers[i].data));
	}
	finished = true;
}

bool CServer::listen(int port)
{
	gameport = port;
	if(socket != 0)
		close();
	ENetAddress address;
	//id_count = 0;
	address.host = ENET_HOST_ANY;
	address.port = port;
	socket = enet_host_create(&address,
										MAX_CONNECTIONS,
										SERVER_IN_LIMIT,
										SERVER_OUT_LIMIT);
	
	
	for(int k=0;k<MAX_PLAYERS;k++){
		controller[k] = 0xFFFF;
	}
	if(socket!=0){
		ENetAddress add;
		add.host = ENET_HOST_ANY;
		add.port = LOOKUP_PORT;
		lookupSocket = enet_host_create(&add,10,0,0);
		
		MessageMetaserverAddServer m;
		memcpy(&m.serverName,servernick.c_str(),servernick.length()+1);
		m.port = gameport;
		sendToMetaserver(&m);
	}
	return (socket != 0);
}

bool CServer::close()
{
	stopLookup();
	if(socket != 0)
	{
		enet_host_destroy(socket);
		socket = 0;
		printf("server closed!\n");
		return true;
	}
	return false;
}

void CServer::processMessage(int size, void* data, PlayerInfo* info, ENetPeer* peer)
{
	if(size == 0) return;
	MessageBase* mess = (MessageBase*)data;
	if(mess->mtype==MESSAGE_ERROR){
		if(info==0) return;
		printf("%s sent me an errorous packet\n",info->name.c_str());
	}
	else if(mess->mtype==MESSAGE_CONNECT){
	if(game != 0){
			enet_peer_disconnect(peer,0);
			return;
	}else{
			bool id[MAX_CONNECTIONS];
			for(int i=0;i<MAX_CONNECTIONS;i++)
				id[i] = true;
			for(int j=0;j<socket->peerCount;j++){
				if(socket->peers[j].data==0) continue;
				PlayerInfo* info = (PlayerInfo*)(socket->peers[j].data);
				id[info->id] = false;
			}
			int tid = -1;
			for(int x=0;x<MAX_CONNECTIONS;x++){
				if(id[x]){
					tid = x;
					break;
				}
			}
			if(tid!=-1){
				peer->data = new PlayerInfo("testUser",peer,tid);
			}else{
				enet_peer_disconnect(peer,0);
			}
		}
		if(peer->data==0)
			return;
		info = (PlayerInfo*)peer->data;
		MessageConnect* con = (MessageConnect*)data;
		info->name = con->name;
		if(info->name=="")
			info->name="MisterX";
		broadcast_chat_msg("*** "+info->name+" CONNECTED ***");
		throwEvent(EVENT_WELCOME,info->peer);
		lobby_sync();
		printf("%s connected\n",info->name.c_str());
	}
	else if(mess->mtype==MESSAGE_CHAT){
		if(info==0) return;
		MessageChat* chat = (MessageChat*)data;
		std::string mess(chat->text);
		broadcast_chat_msg(info->name+CCapt::CHAT_SAYS+mess);
	}
	else if(mess->mtype==MESSAGE_CLIENTSYNC){
		if(info==0) return;
		MessageClientSync* sync = (MessageClientSync*)data;
		if(game != 0)
			game->processMessage(sync,info);
	}
	else if(mess->mtype==MESSAGE_LOBBYTAKE){
		if(info==0) return;
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
	}
	else if(mess->mtype==MESSAGE_REQUEST){
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
	}
}

void CServer::lobby_sync(){
	MessageLobbyState lobby;
	for(int k=0;k<MAX_PLAYERS;k++){
		lobby.controllers[k][0] = 0;
		lobby.yourcontrol[k] = false;
	}
	for(int i=0;i<socket->peerCount;i++){
		if(socket->peers[i].data==0)
			continue;
		PlayerInfo* p = (PlayerInfo*)(socket->peers[i].data);
		for(int k=0;k<MAX_PLAYERS;k++){
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
				sprintf(&(lobby.controllers[k][0]),"CURVEBOT, %s", str.c_str());
			}
			if(controller[k]==p->id){
				memcpy(&(lobby.controllers[k][0]),p->name.c_str(),p->name.length()+1);
			}
		}
	}
	for(int j=0;j<socket->peerCount;j++){
		ENetPeer* peer = &socket->peers[j];
		if(peer->data==0)
			continue;
		PlayerInfo* p = (PlayerInfo*)(peer->data);
		for(int k=0;k<MAX_PLAYERS;k++){
			if(controller[k]==p->id){
				lobby.yourcontrol[k] = true;
			}else{
				lobby.yourcontrol[k] = false;
			}
		}
		ENetPacket* packet = enet_packet_create(&lobby,
															  sizeof(lobby),
															  ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet);
	}
	enet_host_flush(socket);
}
void CServer::broadcast_chat_msg(std::string m){
	MessageChat mess;
	memcpy(&mess.text,m.c_str(),m.length()+1);
	ENetPacket* packet = enet_packet_create(&mess,
                                                sizeof(mess),
                                                ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(socket, 0, packet);
	enet_host_flush(socket);	
}

void CServer::update(int delay)
{
	if(delay<5)
		delay = 5;
	ENetEvent event;
	while(enet_host_service (socket, &event, delay) > 0)
	{
//		if(finished)
//			return;
		switch(event.type)
		{
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
				broadcast_chat_msg(((PlayerInfo*)event.peer->data)->name+CCapt::MSG_DISCONNECT);
				if(game == 0){
					for(int i=0;i<MAX_PLAYERS;i++){
						if(controller[i]==((PlayerInfo*)event.peer->data)->id)
							controller[i]=0xFFFF;
					}
					lobby_sync();
				}else{
					game->kickPlayer((PlayerInfo*)event.peer->data);
					if(game->getState()==STATE_ENDOFGAME){
						reqEndGame();
					}
				}
				printf ("%s disconected.\n", ((PlayerInfo*)event.peer->data)->name.c_str());
				delete ((PlayerInfo*)event.peer->data);
				event.peer->data = 0;
			break;
		}
	}
	if(lookupSocket){
		while(enet_host_service(lookupSocket, &event, 1) > 0){
			if(event.type==ENET_EVENT_TYPE_RECEIVE){
				enet_packet_destroy(event.packet);
			}
		}
		for(int i=0;i<lookupSocket->peerCount;i++){
			if(lookupSocket->peers[i].address.host!=0){
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
						memcpy(&mess.serverName,servernick.c_str(),servernick.length()+1);
						mess.port = gameport;
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
	}
}

void CServer::startGame()
{
	if (game) return;
	stopLookup();
	game = new Game(this);
	
	for(int i=0;i<MAX_PLAYERS;i++){
		Snake* snake = 0;
		if(controller[i]==0xFFFF)
			continue;
		if(controller[i]<0){
			if(controller[i]==-BOTALG_NORMAL){
				snake = new NormalBotSnake(0,game,spdMulti,special[i],enableHoles?holeSize:0,holeDist,holeDistRnd);
			}
			if(controller[i]==-BOTALG_ENHANCED){
				snake = new EnhancedBotSnake(0,game,spdMulti,special[i],enableHoles?holeSize:0,holeDist,holeDistRnd);
			}
			if(controller[i]==-BOTALG_FORCEFIELD){
				snake = new ForcefieldBotSnake(0,game,spdMulti,special[i],enableHoles?holeSize:0,holeDist,holeDistRnd);
			}
			if(controller[i]==-BOTALG_DUMB){
				snake = new DumbBotSnake(0,game,spdMulti,special[i],enableHoles?holeSize:0,holeDist,holeDistRnd);
			}
		}else{
			PlayerInfo* info = 0;
			for(int j=0;j<socket->peerCount;j++){
				if(socket->peers[j].data==0)
					continue;
				PlayerInfo* t = (PlayerInfo*)(socket->peers[j].data);
				if(t->id==controller[i]){
					info = t;
					break;
				}
			}
			if(info==0)
				continue;
			snake = new Snake(info,game,spdMulti,special[i],enableHoles?holeSize:0,holeDist,holeDistRnd);
		}
		game->addPlayer(snake,i);
	}
	
	MessageGameInit settings;
	
	throwEvent(EVENT_STARTGAME);
	
	settings.xsize = game->getWidth();
	settings.ysize = game->getHeight();
	ENetPacket* packet = enet_packet_create(&settings,
														 sizeof(settings),
														 ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(socket, 0, packet);
	enet_host_flush(socket);	
	
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

void CServer::throwEvent(int event, ENetPeer* rec){
	MessageEvent mess;
	mess.event = event;
	ENetPacket* packet = enet_packet_create(&mess,
											 sizeof(mess),
											 ENET_PACKET_FLAG_RELIABLE);
	if(rec==0)
		enet_host_broadcast(socket, 0, packet);
	else
		enet_peer_send(rec, 0, packet);
	enet_host_flush(socket);	
}

void CServer::sendScoreboard(signed char* scores, signed char* loads)
{
	MessageScore mess;
	for(int i=0;i<MAX_PLAYERS;i++){
		mess.scores[i] = scores[i];
		mess.loads[i] = loads[i];
	}
	ENetPacket* packet = enet_packet_create(&mess,
														  sizeof(mess),
														  ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(socket, 0, packet);
	enet_host_flush(socket);	
}

void CServer::sync(MessageServerSync* m)
{
	ENetPacket* packet = enet_packet_create(m,
											 sizeof(*m),
											 0/*ENET_PACKET_FLAG_RELIABLE*/);
	enet_host_broadcast(socket, 0, packet);
	enet_host_flush(socket);	
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

	ENetPacket* packet = enet_packet_create(&mess,
														  sizeof(mess),
														  ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(socket, 0, packet);
	enet_host_flush(socket);	
}

#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <stdio.h>
#include <enet/enet.h>
#include "config.h"

class PlayerInfo{
	public:
		PlayerInfo(std::string nam, ENetPeer* pee, int id);
		std::string name;
		ENetPeer* peer;
		int id;
};

#endif

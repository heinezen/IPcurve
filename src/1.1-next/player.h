#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <stdio.h>
#include <raknet/RakNetTypes.h>
#include "config.h"

class PlayerInfo{
	public:
		PlayerInfo(std::string nam, SystemAddress addr, int id);
		std::string name;
		SystemAddress address;
		int id;
};

#endif

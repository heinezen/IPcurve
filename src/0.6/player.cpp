#include <stdio.h>
#include <string>
#include <enet/enet.h>
#include "player.h"
#include "config.h"

PlayerInfo::PlayerInfo(std::string nam, ENetPeer* pee, int ID)
{
	name = nam;
	peer = pee;
	id = ID;
}

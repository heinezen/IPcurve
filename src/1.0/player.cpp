#include <stdio.h>
#include <string>
#include "player.h"
#include "config.h"

PlayerInfo::PlayerInfo(std::string nam, SystemAddress addr, int ID)
{
	name = nam;
	address = addr;
	id = ID;
}

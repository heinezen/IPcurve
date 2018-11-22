#include <vector>
#include <stdio.h>
#include <string>
#include <iostream>
#include "utils.h"
#include "config.h"

struct GameProfile {
	std::string name;
	int timeout;
	int gameSpeed;
	int enableSpecials;
	int enableHoles;
	int holeDist;
	int holeRnd;
	int turnAngle;
	int holeSize;
};

typedef std::vector<GameProfile> GameProfileList;

class GameProfiler {
	public:
		GameProfiler();
		~GameProfiler();
		GameProfileList* getProfiles();
		GameProfile* getProfile(std::string name);
		void deleteProfile(std::string name);
		void addProfile(const GameProfile& profile);
	private:
		GameProfileList profiles;
};

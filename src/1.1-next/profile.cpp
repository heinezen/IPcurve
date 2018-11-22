#include "profile.h"
#include <rudeconfig/config.h>
//#include <fstream>
/*struct GameProfile {
	int timeout;
	int gameSpeed;
	int enableSpecials;
	int enableHoles;
	int holeDist;
	int holeRnd;
	double turnAngle;
	double holeSize;
};*/

//typedef std::vector<GameProfile> GameProfileList;
//using namespace std;

GameProfiler::GameProfiler()
{
	rude::Config cfg;
	cfg.load((aux::config_path+PROFILE_FILE).c_str());
	int count = cfg.getIntValue("count");
	for(int i=0; i<count; i++) {
		std::string section = "profile"+aux::str_fromInt(i);
		GameProfile p;
		cfg.setSection(section.c_str());
		p.name = cfg.getStringValue("name");
		p.timeout = cfg.getIntValue("timeout");
		p.gameSpeed = cfg.getIntValue("speed");
		p.enableSpecials = cfg.getBoolValue("specials");
		p.enableHoles = cfg.getBoolValue("holes");
		p.holeDist = cfg.getIntValue("holeDistance");
		p.holeRnd = cfg.getIntValue("holeDistanceRandom");
		p.holeSize = cfg.getIntValue("holeSize");
		p.turnAngle = cfg.getDoubleValue("turnAngle");
		if(p.name != "")
			profiles.push_back(p);
	}
}

GameProfiler::~GameProfiler()
{
	rude::Config cfg;
	cfg.clear();
	cfg.setIntValue("count",profiles.size());
	for(int i=0; i<profiles.size(); i++) {
		std::string section = "profile"+aux::str_fromInt(i);
		cfg.setSection(section.c_str());
		GameProfile p = profiles[i];
		cfg.setStringValue("name",p.name.c_str());
		cfg.setIntValue("timeout",p.timeout);
		cfg.setIntValue("speed",p.gameSpeed);
		cfg.setBoolValue("specials",p.enableSpecials);
		cfg.setBoolValue("holes",p.enableHoles);
		cfg.setIntValue("holeDistance",p.holeDist);
		cfg.setIntValue("holeDistanceRandom",p.holeRnd);
		cfg.setIntValue("holeSize",p.holeSize);
		cfg.setDoubleValue("turnAngle",p.turnAngle);
	}
	cfg.save((aux::config_path+PROFILE_FILE).c_str());
}

GameProfileList* GameProfiler::getProfiles()
{
	return &profiles;
}

GameProfile* GameProfiler::getProfile(std::string name)
{
	GameProfileList::iterator it = profiles.begin();
	while(it != profiles.end()) {
		if(name == (*it).name)
			return &(*it);
		++it;
	}
	return 0;
}

void GameProfiler::deleteProfile(std::string name)
{
	GameProfileList::iterator it = profiles.begin();
	while(it != profiles.end()) {
		if(name == (*it).name)
			it = profiles.erase(it);
		else
			++it;
	}
}

void GameProfiler::addProfile(const GameProfile& profile)
{
	profiles.push_back(profile);
}

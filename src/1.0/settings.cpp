#include <stdio.h>
#include <string>
#include "config.h"
#include "settings.h"
#include "utils.h"



using namespace std;

bool CConfig::getBool(rude::Config& cfg, bool def, std::string name, std::string section)
{
	cfg.setSection(section.c_str());
	if(cfg.exists(name.c_str()))
		return cfg.getBoolValue(name.c_str());
	return def;
}

int CConfig::getInt(rude::Config& cfg, int def, std::string name, std::string section)
{
	cfg.setSection(section.c_str());
	if(cfg.exists(name.c_str()))
		return cfg.getIntValue(name.c_str());
	return def;
}

double CConfig::getDouble(rude::Config& cfg, double def, std::string name, std::string section)
{
	cfg.setSection(section.c_str());
	if(cfg.exists(name.c_str()))
		return cfg.getDoubleValue(name.c_str());
	return def;
}

std::string CConfig::getString(rude::Config& cfg, std::string def, std::string name, std::string section)
{
	cfg.setSection(section.c_str());
	if(cfg.exists(name.c_str()))
		return std::string(cfg.getStringValue(name.c_str()));
	return def;
}

void CConfig::setBool(rude::Config& cfg, bool val, std::string name, std::string section)
{
	cfg.setSection(section.c_str());
	cfg.setBoolValue(name.c_str(),val);
}

void CConfig::setInt(rude::Config& cfg, int val, std::string name, std::string section)
{
	cfg.setSection(section.c_str());
	cfg.setIntValue(name.c_str(),val);
}

void CConfig::setDouble(rude::Config& cfg, double val, std::string name, std::string section)
{
	cfg.setSection(section.c_str());
	cfg.setDoubleValue(name.c_str(),val);
}

void CConfig::setString(rude::Config& cfg, std::string val, std::string name, std::string section)
{
	cfg.setSection(section.c_str());
	cfg.setStringValue(name.c_str(),val.c_str());
}

		
CConfig::CConfig()
{
	rude::Config cfg;
	cfg.load((aux::config_path+CONFIG_FILE).c_str());

	metaserver = getString(cfg,DEFAULT_METASERVER,"metaserver","network");

	c_fullscreen = getBool(cfg,true,"fullscreen","video");

	nick = getString(cfg,"Anonymous","nick","client");
	c_ip = getString(cfg,"localhost","lastConnectedIP","client");
	c_port = getString(cfg,"2323","lastConnectedPort","client");
	if(c_port == "")
		c_port = "2323";
		

	c_tooltips = getBool(cfg,true,"tooltips","menu");
	language = getInt(cfg,LANGUAGE_ENGLISH,"language","menu");

	s_desc = getString(cfg,"No description yet!","description","server");
	s_password = getString(cfg,"","password","server");
	
	s_port = getString(cfg,"2323","port","server");
	if(s_port == "")
		s_port = "2323";
	s_spdMulti = getDouble(cfg,STDSPEED,"quality","server");
	s_registermeta = getBool(cfg,false,"registerMetaserver","server");

	s_timeout = getInt(cfg,0,"timeout","gameplay");	
	s_gameSpeed = getInt(cfg,(100-(WANTED_DELAY_STD))*10,"gameSpeed","gameplay");
	s_enableSpecials = getBool(cfg,true,"specials","gameplay");
	
	s_enableHoles = getBool(cfg,true,"holes","gameplay");
	s_holeDist = getInt(cfg,STDHOLEDIST,"holeDistance","gameplay");
	s_holeRnd = getInt(cfg,STDHOLEDISTRND,"holeDistanceRandom","gameplay");
	s_holeSize = getInt(cfg,STDHOLESIZE,"holeSize","gameplay");
	s_turnAngle = getDouble(cfg,STDRAD,"turnAngle","gameplay");
	
	
	for(int i=0; i<MAX_PLAYERS; i++) {
		std::string player = "player" + aux::str_fromInt(i);
		keys[i][0] = (SDLKey)getInt(cfg,stdkeys[i][0],"keyLeft",player);
		keys[i][1] = (SDLKey)getInt(cfg,stdkeys[i][1],"keyRight",player);
		keys[i][2] = (SDLKey)getInt(cfg,stdkeys[i][2],"keySpecial",player);
		colors[i][0] = (unsigned char)getInt(cfg,stdcolors[i][0],"colorR",player);
		colors[i][1] = (unsigned char)getInt(cfg,stdcolors[i][1],"colorG",player);
		colors[i][2] = (unsigned char)getInt(cfg,stdcolors[i][2],"colorB",player);
		special[i] = getInt(cfg,ABILITY_NONE,"ability",player);
		botAlg[i] = getInt(cfg,BOTALG_NORMAL,"botType",player);
	}
	
}

CConfig::~CConfig()
{
	rude::Config cfg;
	cfg.clear();

	setString(cfg,metaserver,"metaserver","network");

	setBool(cfg,c_fullscreen,"fullscreen","video");

	setString(cfg,nick,"nick","client");
	setString(cfg,c_ip,"lastConnectedIP","client");
	setString(cfg,c_port,"lastConnectedPort","client");		

	setBool(cfg,c_tooltips,"tooltips","menu");
	setInt(cfg,language,"language","menu");

	setString(cfg,s_desc,"description","server");
	setString(cfg,s_password,"password","server");
	
	setString(cfg,s_port,"port","server");
	setDouble(cfg,s_spdMulti,"quality","server");
	setBool(cfg,s_registermeta,"registerMetaserver","server");

	setInt(cfg,s_timeout,"timeout","gameplay");	
	setInt(cfg,s_gameSpeed,"gameSpeed","gameplay");
	setBool(cfg,s_enableSpecials,"specials","gameplay");
	
	setBool(cfg,s_enableHoles,"holes","gameplay");
	setInt(cfg,s_holeDist,"holeDistance","gameplay");
	setInt(cfg,s_holeRnd,"holeDistanceRandom","gameplay");
	setInt(cfg,s_holeSize,"holeSize","gameplay");
	setDouble(cfg,s_turnAngle,"turnAngle","gameplay");
	
	
	for(int i=0; i<MAX_PLAYERS; i++) {
		std::string player = "player" + aux::str_fromInt(i);
		setInt(cfg,(int)keys[i][0],"keyLeft",player);
		setInt(cfg,(int)keys[i][1],"keyRight",player);
		setInt(cfg,(int)keys[i][2],"keySpecial",player);
		setInt(cfg,colors[i][0],"colorR",player);
		setInt(cfg,colors[i][1],"colorG",player);
		setInt(cfg,colors[i][2],"colorB",player);
		setInt(cfg,(int)special[i],"ability",player);
		setInt(cfg,(int)botAlg[i],"botType",player);
	}
	cfg.save((aux::config_path+CONFIG_FILE).c_str());
	
}

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "config.h"
#include "settings.h"
#include <SDL/SDL.h>

using namespace std;

CConfig::CConfig()
{
	int i,j,inp;
	nick = "Anonymous";
	metaserver = DEFAULT_METASERVER;
	c_ip = "localhost";
	c_port = "8080";
	c_usemeta = 0;
	s_port = "8080";
	s_timeout = 0;
	s_gameSpeed = (100-(WANTED_DELAY_STD))*10;
	s_spdMulti = STDSPEED;
	s_enableSpecials = true;
	
	s_enableHoles = true;
	s_holeDist = STDHOLEDIST;
	s_holeRnd = STDHOLEDISTRND;
	s_holeSize = STDHOLESIZE;
	s_registermeta = 0;
	
	language = LANGUAGE_ENGLISH;
	
	memcpy(&keys,&stdkeys,sizeof(stdkeys));
	memcpy(&colors,&stdcolors,sizeof(colors));
	for(i=0;i<MAX_PLAYERS;i++)
		special[i] = ABILITY_NONE;
	for(i=0;i<MAX_PLAYERS;i++)
		botAlg[i] = BOTALG_NORMAL;
	
	fstream stream(CONFIG_FILE,ios_base::in);
	if(stream){
		stream >> nick;
		stream >> c_ip;
		stream >> c_port;
		stream >> s_port;
		for(i=0;i<MAX_PLAYERS;i++){
			for(j=0;j<3;j++){
				stream >> inp;
				keys[i][j] = (SDLKey)inp;
			}
		}
		for(i=0;i<MAX_PLAYERS;i++){
			for(j=0;j<3;j++){
				stream >> inp;
				colors[i][j] = (unsigned char)inp;
			}
		}
		for(i=0;i<MAX_PLAYERS;i++){
			stream >> inp;
			special[i] = (unsigned char)inp;
		}
		stream >> s_timeout;
		stream >> s_gameSpeed;
		stream >> s_spdMulti;
		stream >> s_enableSpecials;
		
		stream >> s_enableHoles;
		stream >> s_holeDist;
		stream >> s_holeRnd;
		stream >> s_holeSize;
		stream >> language;
		stream >> metaserver;
		stream >> c_usemeta;
		stream >> s_registermeta;
		for(i=0;i<MAX_PLAYERS;i++){
			stream >> inp;
			if(inp==BOTALG_NOBOT||inp>BOTALG_MAX)
				inp = BOTALG_NORMAL;
			botAlg[i] = (unsigned char)inp;
		}
		stream.close();
	}
}

CConfig::~CConfig()
{
	int i,j;
	fstream stream(CONFIG_FILE,ios_base::out|ios_base::trunc);
	stream << nick << endl;
	stream << c_ip << endl;
	stream << c_port << endl;
	stream << s_port << endl;
	for(i=0;i<MAX_PLAYERS;i++){
		for(j=0;j<3;j++){
			int inp = (int)keys[i][j];
			stream << inp << endl;
		}
	}
	for(i=0;i<MAX_PLAYERS;i++){
		for(j=0;j<3;j++){
			int inp = (int)colors[i][j];
			stream << inp << endl;
		}
	}
	for(i=0;i<MAX_PLAYERS;i++){
		int inp = (int)special[i];
		stream << inp << endl;
	}
	stream << s_timeout << endl;
	stream << s_gameSpeed << endl;
	stream << s_spdMulti << endl;
	stream << s_enableSpecials << endl;
	
	stream << s_enableHoles << endl;
	stream << s_holeDist << endl;
	stream << s_holeRnd << endl;
	stream << s_holeSize << endl;
	stream << language << endl;
	stream << metaserver << endl;
	stream << c_usemeta << endl;
	stream << s_registermeta << endl;
	for(i=0;i<MAX_PLAYERS;i++){
		int inp = (int)botAlg[i];
		stream << inp << endl;
	}
	stream.close();
}

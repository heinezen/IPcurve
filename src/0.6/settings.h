#ifndef SETTINGS_H
#define SETTINGS_H
#include <stdio.h>
#include <string>
#include <iostream>
#include <SDL/SDL.h>
#include "config.h"

class CConfig{
	public:
		CConfig();
		~CConfig();
		// NICK NAME
		std::string nick;
		std::string metaserver;
		// CLIENT CONFIG
		std::string c_ip;
		std::string c_port;
		SDLKey keys[MAX_PLAYERS][3];
		unsigned char colors[MAX_PLAYERS][3];
		unsigned char special[MAX_PLAYERS];
		unsigned char botAlg[MAX_PLAYERS];
		int c_usemeta;
		
		// SERVER CONFIG
		std::string s_port;
		int s_timeout;
		int s_gameSpeed;
		int s_enableSpecials;
		double s_spdMulti;
		int s_enableHoles;
		int s_holeDist;
		int s_holeRnd;
		double s_holeSize;
		int s_registermeta;
		
		// LANGUAGE
		int language;
};

#endif

#ifndef SETTINGS_H
#define SETTINGS_H
#include <stdio.h>
#include <string>
#include <SDL/SDL.h>
#include "config.h"
#include <rudeconfig/config.h>

class CConfig{
	public:
		CConfig();
		~CConfig();
		// NICK NAME + METASERVER
		std::string nick;
		std::string metaserver;
		std::string s_desc;
		std::string s_password;
		
		// CLIENT CONFIG
		std::string c_ip;
		std::string c_port;
		SDLKey keys[MAX_PLAYERS][3];
		unsigned char colors[MAX_PLAYERS][3];
		unsigned char special[MAX_PLAYERS];
		unsigned char botAlg[MAX_PLAYERS];
		bool c_fullscreen;
		bool c_tooltips;
		bool c_opengl;
		
		// SERVER CONFIG
		std::string s_port;
		int s_timeout;
		int s_gameSpeed;
		bool s_enableSpecials;
		double s_spdMulti;
		bool s_enableHoles;
		int s_holeDist;
		int s_holeRnd;
		double s_turnAngle;
		double s_holeSize;
		bool s_registermeta;
				
		// LANGUAGE
		int language;
	private:
		bool getBool(rude::Config& cfg, bool def, std::string name, std::string section);
		int getInt(rude::Config& cfg, int def, std::string name, std::string section);
		double getDouble(rude::Config& cfg, double def, std::string name, std::string section);
		std::string getString(rude::Config& cfg, std::string def, std::string name, std::string section);
		void setBool(rude::Config& cfg, bool val, std::string name, std::string section);
		void setInt(rude::Config& cfg, int val, std::string name, std::string section);
		void setDouble(rude::Config& cfg, double val, std::string name, std::string section);
		void setString(rude::Config& cfg, std::string val, std::string name, std::string section);
};

#endif

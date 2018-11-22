#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

// DO VARARGS for str_format

std::string aux::str_keyName(const SDLKey& key)
{
	std::string str = "";
	if(key>=SDLK_WORLD_42&&key<SDLK_WORLD_52){
		str = "mouse " + aux::str_fromInt(key-SDLK_WORLD_42);
	}else if(key>=SDLK_KP0&&key<=SDLK_KP_EQUALS){
		str = "kp ";
		str += SDL_GetKeyName(key);
	}else{
		str = SDL_GetKeyName(key);
	}
	aux::str_toUpper(str);
	return str;
}

void aux::str_toUpper(std::string& str)
{
	std::transform(str.begin(),str.end(),str.begin(),(int(*)(int))std::toupper);
}

void aux::str_toLower(std::string& str)
{
	std::transform(str.begin(),str.end(),str.begin(),(int(*)(int))std::toupper);
}

int aux::str_toInt(const std::string& str)
{
	int number = atoi(str.c_str());
	return number;
}

std::string aux::str_fromInt(int number)
{
	char buf[25];
	snprintf(buf,25,"%d",number);
	std::string str = buf;
	return str;
}

std::string aux::str_arg(const std::string& str, const std::string& arg)
{
	std::string target = str.c_str();
	int token_no = 0;
	bool token_found = false;
	while(token_no <= 9 && !token_found) {
		std::string token = "%"+str_fromInt(token_no++);
		size_t token_pos;
		while((token_pos = target.find(token)) != std::string::npos) {
			target.replace(token_pos,2,arg);
			token_found = true;
		}
	}
	return target;
	//va_list args;
	//va_start(args);
	//char buf[2048];
	//snprintf(buf,2048,format.c_str(),args);
	//return std::string(buf);
}

void aux::gfx_FillRect(SDL_Surface* target, int x, int y, int w, int h, unsigned int color)
{
	SDL_Rect r;
	r.x = x; r.y = y;
	r.w = w; r.h = h;
	SDL_FillRect(target,&r,color);
}

void aux::gfx_DrawRect(SDL_Surface* target, int x, int y, int w, int h, unsigned int color)
{
	gfx_FillRect(target,x,y,1,h,color);
	gfx_FillRect(target,x+w-1,y,1,h,color);
	gfx_FillRect(target,x,y,w,1,color);
	gfx_FillRect(target,x,y+h-1,w,1,color);
}

void aux::str_replaceSpaces(std::string& str)
{
	size_t token_pos;
	while((token_pos = str.find(" ")) != std::string::npos)
		str.replace(token_pos,1,"_");
}

void aux::str_revertSpaces(std::string& str)
{
	size_t token_pos;
	while((token_pos = str.find("_")) != std::string::npos)
		str.replace(token_pos,1," ");
}


std::string aux::data_path;
std::string aux::config_path;
std::string aux::screenshot_path;

#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <cctype>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

typedef unsigned char byte;

class aux {
	public:
		/* String handling routines */
		static void str_replaceSpaces(std::string& str);
		static void str_revertSpaces(std::string& str);
		static void str_toUpper(std::string& str);
		static void str_toLower(std::string& str);
		static std::string str_fromInt(int number);
		static int str_toInt(const std::string& str);
		static std::string str_keyName(const SDLKey& key);
		static std::string str_arg(const std::string& str, const std::string& arg);
		/* SDL Helper functions */
		static void gfx_FillRect(SDL_Surface* target, int x, int y, int w, int h, unsigned int color);
		static void gfx_DrawRect(SDL_Surface* target, int x, int y, int w, int h, unsigned int color);
		/* The app's paths */
		static std::string data_path;
		static std::string config_path;
		static std::string screenshot_path;
};
#endif

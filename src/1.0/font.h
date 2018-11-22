#ifndef FONT_H
#define FONT_H

#include "utils.h"
#include <string>
#include <vector>

typedef std::vector<std::string> string_list;

class CFont{
	public:
		CFont(std::string fontName, int size);
		~CFont();
		void render_to(SDL_Surface* target, int x, int y, byte r, byte g, byte b, const std::string& text); 
		void render_to(SDL_Surface* target, int x, int y, SDL_Color c, const std::string& text);
		SDL_Surface* render(byte r, byte g, byte b, const std::string& text);
		SDL_Surface* render(SDL_Color c, const std::string& text);
		int getHeight();
		int textWidth(std::string text);
		string_list wordWrap(const std::string& text, int w); 
	private:
		TTF_Font* font;
};

#endif

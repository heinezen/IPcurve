#ifndef FONT_H
#define FONT_H

#include "utils.h"
#include <string>
#include <vector>
#include <SDL/SDL_ttf.h>

typedef std::vector<std::string> string_list;

#define MIN_GLYPH ' '
#define MAX_GLYPH 254 

class CFont{
	public:
		CFont(std::string fontName, int size, bool use_opengl);
		~CFont();
		void render_to(SDL_Surface* target, int x, int y, byte r, byte g, byte b, const std::string& text); 
		void render_to(SDL_Surface* target, int x, int y, SDL_Color c, const std::string& text);
		SDL_Surface* render(byte r, byte g, byte b, const std::string& text);
		SDL_Surface* render(SDL_Color c, const std::string& text);
		int getHeight();
		int textWidth(std::string text);
		string_list wordWrap(const std::string& text, int w); 
	private:
		struct glyph {
			int minx, maxx;
			int miny, maxy;
			int advance;
			SDL_Surface* pic;
			GLuint tex;
			GLfloat texMinX, texMinY;
			GLfloat texMaxX, texMaxY;
		};

		int height;
		int ascent;
		int descent;
		int lineSkip;
		glyph glyphs[MAX_GLYPH + 1];

		std::string fontName;
		int pointSize;

		TTF_Font* ttfFont;

//		float fgRed, fgGreen, fgBlue;
//		SDL_Color foreground;

		bool opengl;

		void loadChar(unsigned char c);
		void initFont();
		void deinitFont();
		void glDrawText(unsigned char* text, int x, int y, float r, float g, float b);
//		void textSize(char *text, SDL_Rect *r);
};
#endif

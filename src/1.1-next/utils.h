#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <cctype>
#include <string>
#include <SDL/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>

#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)

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
		static void gfx_FillRect(SDL_Surface* target, int x, int y, int w, int h, unsigned int color,bool opengl);
		static void gfx_DrawRect(SDL_Surface* target, int x, int y, int w, int h, unsigned int color,bool opengl);
		/* OpenGL Helper functions */
		static GLuint SDL_GL_LoadTexture(SDL_Surface* surface, GLfloat* texcoord);
		static void InitOpenGL(int w, int h);
		static void SDL_GL_Blit(GLuint texture, const SDL_Rect& dest, GLfloat* texcoord);
		/* The app's paths */
		static std::string data_path;
		static std::string config_path;
		static std::string screenshot_path;
};
#endif

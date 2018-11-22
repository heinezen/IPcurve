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

void aux::gfx_FillRect(SDL_Surface* target, int x, int y, int w, int h, unsigned int color,bool opengl)
{
	if(opengl) {
		float r = ((color & 0xFF0000)>>16)/255.0;
		float g = ((color & 0x00FF00)>>8)/255.0;
		float b = ((color & 0x0000FF))/255.0;
		glDisable(GL_TEXTURE_2D);
		glColor3f(r,g,b);
		glBegin(GL_QUADS);
		glVertex2f(x,y);
		glVertex2f(x+w+1,y);
		glVertex2f(x+w+1,y+h+1);
		glVertex2f(x,y+h+1);
		glEnd();
	} else {
		SDL_Rect r;
		r.x = x; r.y = y;
		r.w = w; r.h = h;
		SDL_FillRect(target,&r,color);
	}
}

void aux::gfx_DrawRect(SDL_Surface* target, int x, int y, int w, int h, unsigned int color,bool opengl)
{
	if(opengl) {
		float r = ((color & 0xFF0000)>>16)/255.0;
		float g = ((color & 0x00FF00)>>8)/255.0;
		float b = ((color & 0x0000FF))/255.0;
		glDisable(GL_TEXTURE_2D);
		glColor3f(r,g,b);
		glLineWidth(1);
		glBegin(GL_LINE_STRIP);
		glVertex2f(x,y);
		glVertex2f(x+w+1,y);
		glVertex2f(x+w+1,y+h+1);
		glVertex2f(x,y+h+1);
		glVertex2f(x,y);
		glEnd();
	} else {
		gfx_FillRect(target,x,y,1,h,color,false);
		gfx_FillRect(target,x+w-1,y,1,h,color,false);
		gfx_FillRect(target,x,y,w,1,color,false);
		gfx_FillRect(target,x,y+h-1,w,1,color,false);
	}
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

int power_of_two(int input) {
	int value = 1;
	while (value < input)
		value <<= 1;
	return value;
}
 
GLuint aux::SDL_GL_LoadTexture(SDL_Surface* surface, GLfloat* texcoord)
{

	/* Use the surface width and height expanded to powers of 2 */
	int w = power_of_two(surface->w);
	int h = power_of_two(surface->h);
	texcoord[0] = 0.0f;                        /* Min X */
	texcoord[1] = 0.0f;                        /* Min Y */
	texcoord[2] = (GLfloat)surface->w / w;        /* Max X */
	texcoord[3] = (GLfloat)surface->h / h;        /* Max Y */
	SDL_Surface* image = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
					0x000000FF,
					0x0000FF00,
					0x00FF0000,
					0xFF000000
#else
					0xFF000000,
					0x00FF0000,
					0x0000FF00,
					0x000000FF
#endif
					);
	if(!image)
		return 0;
		
	/* Save the alpha blending attributes */
	Uint32 saved_flags = surface->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
	Uint8 saved_alpha = surface->format->alpha;
	if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
			SDL_SetAlpha(surface, 0, 0);
	}

	/* Copy the surface into the GL texture image */
	SDL_Rect area;
	area.x = 0;
	area.y = 0;
	area.w = surface->w;
	area.h = surface->h;
	SDL_BlitSurface(surface, &area, image, &area);

	/* Restore the alpha blending attributes */
	if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
			SDL_SetAlpha(surface, saved_flags, saved_alpha);
	}

	/* Create an OpenGL texture for the image */
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
	SDL_FreeSurface(image);
	
	return texture;
}

void aux::InitOpenGL(int w,int h)
{

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8); 
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,w,h,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void aux::SDL_GL_Blit(GLuint texture, const SDL_Rect& dest, GLfloat* texcoord)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
	glColor3f(1,1,1);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(texcoord[0],texcoord[1]);
		glVertex2f(dest.x,dest.y);
		glTexCoord2f(texcoord[2],texcoord[1]);
		glVertex2f(dest.x+dest.w,dest.y);
		glTexCoord2f(texcoord[0],texcoord[3]);
		glVertex2f(dest.x,dest.y+dest.h);
		glTexCoord2f(texcoord[2],texcoord[3]);
		glVertex2f(dest.x+dest.w,dest.y+dest.h);
	glEnd();
}

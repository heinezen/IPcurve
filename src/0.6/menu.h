#ifndef MENU_H
#define MENU_H
#include <stdio.h>
#include <string>
#include <vector>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "config.h"
#include "text.h"

#define DESTROYHOOK(a) (void(*)(CControl*))(a)

class CControl{
	public:
		CControl(CControl* o,SDL_Surface* t = 0);
		~CControl();
		void pmouseDown(int x, int y, int button);
		void pmouseUp(int x, int y, int button);
		void pmouseMove(int x, int y);
		void pmouseEnter();
		void pmouseLeave();
		void pkeyDown(SDL_keysym* sym);
		void addChild(CControl* child);
		void repaint();
		int getX();
		int getY();
		int getW();
		int getH();
		int globalToLocalX(int xx);
		int globalToLocalY(int yy);
		bool isInBounds(int xx, int yy);
		bool isMouseIn();
		void requestClose();
		void invalidate();
		SDL_Surface* getTarget();
		CControl* getOwner();
		std::vector<CControl*> childs;
	protected:
		virtual void paint(SDL_Surface* target);
		virtual void mouseEnter();
		virtual void mouseLeave();
		virtual void mouseDown(int x, int y, int button);
		virtual void mouseUp(int x, int y, int button);
		virtual void mouseMove(int x, int y);
		virtual void keyDown(SDL_keysym* sym);
		void (*onDestroy)(CControl*);
		bool closing();
		int x, y, w, h;
	private:
		bool mouseIn;
		bool ownSDL;
		bool closeReq;
		SDL_Surface* target;
		CControl* owner;
};

class CMenu:public CControl{
	public:
		CMenu(CControl* o,SDL_Surface* t = 0);
		virtual void execute();
		TTF_Font* titleFont;
	protected:
		std::string caption;
		virtual void paint(SDL_Surface* target);
		virtual void handleEvents();
		virtual void keyDown(SDL_keysym* sym);
};

class CPushButton:public CControl{
	public:
		CPushButton(CControl* o, int x, int y, std::string Caption, void (*clicked)(CControl*,void*) = 0, void* data = 0, int fontsize = FONT_MENUBUTTONSIZE, SDL_Surface* t = 0);
		TTF_Font* font;
		std::string getCaption();
	protected:
		std::string caption;
		bool highlight;
		void (*onClick)(CControl*,void*);
		void* userdata;
		virtual void paint(SDL_Surface* target);
		virtual void mouseEnter();
		virtual void mouseLeave();
		virtual void mouseDown(int x, int y, int button);
};

class CTextEdit:public CControl{
	public:
		CTextEdit(CControl* o, int x, int y, int w, std::string Text, bool number=false, int fontsize = FONT_MENUSIZE, SDL_Surface* t = 0);
		std::string getText();
		void setText(std::string txt);
		TTF_Font* font;
	protected:
		std::string text;
		bool highlight;
		bool numbers;
		virtual void paint(SDL_Surface* target);
		virtual void mouseEnter();
		virtual void mouseLeave();
		virtual void keyDown(SDL_keysym* sym);
};

class CKeyEdit:public CControl{
	public:
		CKeyEdit(CControl* o, int x, int y, int w, SDLKey k, int fontsize = FONT_MENUSIZE, SDL_Surface* t = 0);
		SDLKey getKey();
		TTF_Font* font;
	protected:
		SDLKey key;
		bool highlight;
		virtual void paint(SDL_Surface* target);
		virtual void mouseEnter();
		virtual void mouseLeave();
		virtual void keyDown(SDL_keysym* sym);
		virtual void mouseDown(int x,int y, int button);
};

class CColorEdit:public CControl{
	public:
		CColorEdit(CControl* o, int x, int y, int w, SDL_Color col, int fontsize = FONT_MENUSIZE, SDL_Surface* t = 0);
		SDL_Color getColor();
		TTF_Font* font;
	protected:
		SDL_Color color;
		virtual void paint(SDL_Surface* target);
		virtual void mouseDown(int x,int y, int button);
};

class CNumberEdit:public CControl{
	public:
		CNumberEdit(CControl* o, int x, int y, int w, int num, std::string Caption, int fontsize = FONT_MENUSIZE, SDL_Surface* t = 0);
		int getValue();
		TTF_Font* font;
	protected:
		bool highlight;
		std::string caption;
		int value;
		virtual void paint(SDL_Surface* target);
		virtual void mouseEnter();
		virtual void mouseLeave();
		virtual void mouseDown(int x, int y, int button);
};

class CImageList:public CControl{
	public:
		CImageList(CControl* o, int x, int y, int h, int num, std::string bitmap, SDL_Surface* t = 0);
		int getImage();
		SDL_Surface* img;
	protected:
		int image;
		int numImages;
		virtual void paint(SDL_Surface* target);
		virtual void mouseDown(int x, int y, int button);
};

class CCheckBox:public CControl{
	public:
		CCheckBox(CControl* o, int x, int y, std::string capt, bool chk, void (*clicked)(CControl*,void*) = 0, void* userdata = 0, int fontsize = FONT_MENUSIZE, SDL_Surface* t = 0);
		bool isChecked();
		TTF_Font* font;
		SDL_Surface* img;
	protected:
		void (*onClick)(CControl*,void*);
		void* userdata;
		bool checked;
		bool highlight;
		std::string caption;
		virtual void mouseEnter();
		virtual void mouseLeave();
		virtual void paint(SDL_Surface* target);
		virtual void mouseDown(int x, int y, int button);
};

class CList :public CControl{
	public:
		CList(CControl* o, int x, int y, int w, void (*clicked)(CControl*,std::string,void*) = 0, void* data = 0, int fontsize = FONT_MENUSIZE, SDL_Surface* t = 0);
		TTF_Font* font;
		std::vector<std::string> getElements();
		void clearElements();
		void addElement(std::string element);
		void removeElement(std::string element);
	protected:
		std::vector<std::string> elements;
		int highlight;
		int objheight;
		int oldh;
		void (*onClick)(CControl*,std::string,void*);
		void* userdata;
		virtual void paint(SDL_Surface* target);
		virtual void mouseLeave();
		virtual void mouseMove(int x, int y);
		virtual void mouseDown(int x, int y, int button);
};

#endif

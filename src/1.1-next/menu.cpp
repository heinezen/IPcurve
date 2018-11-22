#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "config.h"
#include "settings.h"
#include "menu.h"
#include "text.h"
#include "utils.h"

// If defined, fullscreen video mode is disabled. This is used to prevent deadlocks while debugging new features
// #define NO_FULLSCREEN 1


/** CControl Base Class **/

CControl::CControl(CControl* o,SDL_Surface* t)
{
	CConfig cfg;
	tooltip_timer = 0;
	tooltip = 0;
	owner = o;
	target = t;
	closeReq = false;
	ownSDL = false;
	if(!target) {
		if(!owner) {
			ownSDL = true;
			SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
			//#ifdef WIN32
			//	target = SDL_SetVideoMode(SCREENWIDTH,SCREENHEIGHT,SCREENBPP,SDL_SWSURFACE|FULLSCREENFLAG);
			//#else
			opengl = cfg.c_opengl;
			#ifndef NO_FULLSCREEN
			if(cfg.c_fullscreen)
				target = SDL_SetVideoMode(SCREENWIDTH,SCREENHEIGHT,SCREENBPP,(opengl?SDL_HWSURFACE:SDL_SWSURFACE)|SDL_FULLSCREEN|(opengl?SDL_OPENGL:0));
			else
			#endif
				target = SDL_SetVideoMode(SCREENWIDTH,SCREENHEIGHT,SCREENBPP,(opengl?SDL_HWSURFACE:SDL_SWSURFACE)|(opengl?SDL_OPENGL:0));
			//#endif
			if(opengl)
				aux::InitOpenGL(SCREENWIDTH,SCREENHEIGHT);
			TTF_Init();
			resetSDL();
			SDL_WM_SetCaption("IPCurve",0);
		}else{
			target = owner->getTarget();
			opengl = owner->usingOpenGL();
		}
	} else {
		opengl = (target->flags & SDL_OPENGL) > 0;
		printf("opengl: %d\n",opengl);
	}
	if(owner) 
		owner->addChild(this);
	mouseIn = false;
	onKeyDown = 0;
	//onDestroy = 0;
}

CControl::~CControl()
{
	hideAllTooltips();
	if(tooltip_timer)
		delete tooltip_timer;
	//if(onDestroy)
	//	onDestroy(this);
	std::vector<CControl*>::iterator it;
	for(it = childs.begin(); it != childs.end(); it++){
		CControl* control = *it;
		if(control)
			delete control;
	}
	childs.clear();
	if(ownSDL){
		SDL_EnableUNICODE(0);
		TTF_Quit();
		SDL_Quit();
	}
}

bool CControl::usingOpenGL()
{
	return opengl;
}

CControl* CControl::getRoot()
{
	CControl* root = this;
	while(root->getOwner())
		root = root->getOwner();
	return root;
}

void CControl::setOnKeyDown(CKeyCallback OnKeyDown, void* data)
{
	onKeyDown = OnKeyDown;
	keyDown_data = data;
}

void CControl::showTooltip()
{
	if(tooltip_timer)
		tooltip_timer->stop();
	if(tooltip)
		tooltip->show(x,y+h);
}

void CControl::hideTooltip()
{
	if(tooltip_timer)
		tooltip_timer->stop();
	if(tooltip)
		tooltip->hide();
}

void CControl::hideAllTooltips()
{
	hideTooltip();
	std::vector<CControl*>::iterator it;
	for(it = childs.begin(); it != childs.end(); it++)
		(*it)->hideAllTooltips();
}

void makeTooltipCB(void* obj)
{
	CControl* control = (CControl*)obj;
	control->showTooltip();
}

void CControl::setTooltip(std::string title, std::string text)
{
	CConfig cfg;
	if(!cfg.c_tooltips)
		return;
	bool show_it = false;
	if(tooltip_timer)
		delete tooltip_timer;
	if(tooltip) {
		show_it = tooltip->isVisible();
		deleteChild(tooltip);
		delete tooltip;
	}
	if(opengl)
		getRoot()->repaint(false);
	if(text.length()>0) {
		tooltip = new CTooltip(this,300,title,text);
		tooltip_timer = new CTimer(TOOLTIP_DELAY,makeTooltipCB,this,1,1);
	}
	if(show_it)
		showTooltip();
}

void CControl::addChild(CControl* child)
{
	childs.push_back(child);
}

void CControl::deleteChild(CControl* child)
{
	std::vector<CControl*>::iterator it = std::find(childs.begin(),childs.end(),child);
	if(it != childs.end())
		childs.erase(it);
}

/* EVENT HANDLERS */
void CControl::mouseDown(int x, int y, int button){}
void CControl::mouseUp(int x, int y, int button){}
void CControl::mouseMove(int x, int y){}
void CControl::mouseEnter(){}
void CControl::mouseLeave(){}
void CControl::paint(SDL_Surface* target){}
void CControl::keyDown(SDL_keysym *sym){}


/* INTERNAL EVENT HANDLERS */
void CControl::requestClose()
{
	closeReq = true;
	if(owner != 0)
		owner->requestClose();
}

void CControl::invalidate(bool do_repaint)
{
	//SDL_EnableUNICODE(1);
	//SDL_EnableKeyRepeat(100,10);
	hideAllTooltips();
// CLEAR ALL HIGHLIGHTS
	pmouseMove(-42,-42);
	if(do_repaint)
		repaint();
}

void CControl::resetSDL()
{
	SDL_ShowCursor(1);
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(200,70);
}

void CControl::cleanSDL()
{
	SDL_EnableUNICODE(0);
	SDL_EnableKeyRepeat(0,0);
}

SDL_Rect CControl::getArea()
{
	SDL_Rect area;
	area.x = x;
	area.y = y;
	area.w = w;
	area.h = h;
	return area;
}

SDL_Rect CControl::getSize()
{
	SDL_Rect area;
	area.x = 0;
	area.y = 0;
	area.w = w;
	area.h = h;
	return area;
}

void CControl::repaint(bool flip)
{
	paint(target);
	std::vector<CControl*>::iterator it;
	for(it = childs.begin(); it != childs.end(); it++){
		CControl* control = *it;
		if(control==0) continue;
		control->repaint(false);
	}
	if(opengl && flip)
		SDL_GL_SwapBuffers();
}

void CControl::pmouseEnter(int x, int y)
{
	if(tooltip)
		tooltip_timer->start();
	mouseIn = true;
	mouseEnter();
}

void CControl::pmouseLeave()
{
	if(tooltip) {
		tooltip->hide();
		tooltip_timer->stop();
	}
	mouseIn = false;
	mouseLeave();
}

void CControl::pkeyDown(SDL_keysym* sym)
{
	std::vector<CControl*>::iterator it;
	for(it = childs.begin(); it != childs.end(); it++){
		CControl* control = *it;
		if(control==0) continue;
		if(control->isMouseIn())
			control->pkeyDown(sym);
	}
	keyDown(sym);
	if(onKeyDown)
		onKeyDown(this,sym,keyDown_data);
}

void CControl::pmouseDown(int x, int y, int button)
{
	std::vector<CControl*>::iterator it;
	for(it = childs.begin(); it != childs.end(); it++){
		CControl* control = *it;
		if(control==0) continue;
		if(control->isMouseIn())
			control->pmouseDown(x,y,button);
	}
	mouseDown(globalToLocalX(x),globalToLocalY(y),button);
}

void CControl::pmouseUp(int x, int y, int button)
{
	std::vector<CControl*>::iterator it;
	for(it = childs.begin(); it != childs.end(); it++){
		CControl* control = *it;
		if(control==0) continue;
		if(control->isMouseIn())
			control->pmouseUp(x,y,button);
	}
	mouseUp(globalToLocalX(x),globalToLocalY(y),button);
}

void CControl::pmouseMove(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
	std::vector<CControl*>::iterator it;
	for(it = childs.begin(); it != childs.end(); it++){
		CControl* control = *it;
		if(control==0) continue;
		if(control->isMouseIn()&&!(control->isInBounds(x,y)))
			control->pmouseLeave();
		if(!(control->isMouseIn())&&control->isInBounds(x,y))
			control->pmouseEnter(x,y);
		if(control->isMouseIn())
			control->pmouseMove(x,y);
	}
	mouseMove(globalToLocalX(x),globalToLocalY(y));
}

/* GET'S FOR VARIOUS PRIVATE MEMBERS */

SDL_Surface* CControl::getTarget()
{
	return target;
}

CControl* CControl::getOwner()
{
	return owner;
}

int CControl::getX()
{
	return x;
}

int CControl::getY()
{
	return y;
}

int CControl::getW()
{
	return w;
}

int CControl::getH()
{
	return h;
}

int CControl::globalToLocalX(int xx)
{
	return xx-x;
}

int CControl::globalToLocalY(int yy)
{
	return yy-y;
}

bool CControl::isInBounds(int xx, int yy)
{
	return ((xx-x)>0&&(xx-x)<w&&(yy-y)>0&&(yy-y)<h);
}

bool CControl::isMouseIn()
{
	return mouseIn;
}

bool CControl::closing()
{
	return closeReq;
}

void CControl::setPos(int x, int y)
{
	this->x = x;
	this->y = y;
}


/** CMenu Class **/
void CMenu::execute()
{
	menu_init();
	repaint();
	while(!closing()){
		if(opengl && showTitle) {
			aux::gfx_FillRect(0,0,0,SCREENWIDTH,titleFont.getHeight(),0,true);
			titleFont.render_to(0,0,0,255,abs(gl_shift-255),0,caption);
			SDL_GL_SwapBuffers();
			SDL_Delay(50);
			gl_shift = (gl_shift+2)%510;
		}
		std::vector<CControl*>::iterator it;
		for(it = repaintQueue.begin(); it != repaintQueue.end();it++)
			(*it)->repaint();
		repaintQueue.clear();
		handleEvents();
	}
	menu_exit();
}

CMenu::CMenu(CControl* o,SDL_Surface* t) : CControl(o,t),
	titleFont(FONT_TITLEFONT,FONT_TITLESIZE,opengl)
{
	gl_shift = 255;
	showTitle = true;
	caption = "Untitled Menu";
}

void CMenu::handleEvents()
{
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		switch(e.type){
			case SDL_QUIT:
				requestClose();
			break;
			case SDL_KEYDOWN:
				pkeyDown(&e.key.keysym);
			break;	
			case SDL_MOUSEBUTTONDOWN:
				pmouseDown(e.button.x,e.button.y,e.button.button);
			break;	
			case SDL_MOUSEBUTTONUP:
				pmouseUp(e.button.x,e.button.y,e.button.button);
			break;	
			case SDL_MOUSEMOTION:
				pmouseMove(e.motion.x,e.motion.y);
			break;	
		}
	}
}

void CMenu::paint(SDL_Surface* target)
{
	if(opengl) {
		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();
		titleFont.render_to(0,0,0,255,abs(gl_shift-255),0,caption);
	} else {
		SDL_FillRect(target,0,0);
		titleFont.render_to(target,0,0,255,127,0,caption);
		SDL_Flip(target);
	}
}
		
void CMenu::keyDown(SDL_keysym* sym)
{
	if(sym->sym==ABORT_KEY)
		requestClose();
}

void CMenu::paintElement(CControl* ctrl)
{
	repaintQueue.push_back(ctrl);
}

void CMenu::unpaintElement(CControl* ctrl)
{
	std::vector<CControl*>::iterator it = std::find(repaintQueue.begin(),repaintQueue.end(),ctrl);
	if(it != repaintQueue.end())
		repaintQueue.erase(it);
}


void CMenu::menu_init() {}
void CMenu::menu_exit() {}
		
/** CPushButton Class **/

CPushButton::CPushButton(CControl* o, int x, int y, std::string Caption, int fontsize, SDL_Surface* t) : CControl(o,t),
	font(FONT_MENUBUTTONFONT,fontsize,opengl)
{
	//font = TTF_OpenFont((aux::data_path+FONT_MENUBUTTONFONT).c_str(),fontsize);
	caption = Caption;
	onClick = 0;
	highlight = false;
	this->x = x;
	this->y = y;
	this->h = font.getHeight();
	this->w = font.textWidth(caption);
	color = 0;
	//onDestroy = DESTROYHOOK(CPushButtonDestroy);
}

void CPushButton::setOnClick(CEventCallback OnClick, void* data)
{
	onClick = OnClick;
	userdata = data;
}

CPushButton::~CPushButton()
{
	if(color)
		delete color;
}

void CPushButton::setColor(SDL_Color* col)
{
	if(color)
		delete color;
	color = col;
}

void CPushButton::paint(SDL_Surface* target)
{
	/*SDL_Color c;
	c.r = 
	c.g = 
	c.b = 
	SDL_Surface* tex;
	SDL_Rect r;
	tex = RENDERMENUFONT(font,caption.c_str(),c);
	if(tex == 0) {
		printf("Can't render font\n");
		return;
	}
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	SDL_FillRect(target,&r,0);
	SDL_BlitSurface(tex,0,target,&r);
	SDL_FreeSurface(tex);*/
	SDL_Color c;
	c.r = 0;
	c.g = 0;
	c.b = 255;
	if(onClick) {
		c.r = 255;
		c.g = highlight?127:0;
		c.b = 0;
	}
	if(color)
		c = *color;
	
/*	if(!opengl) {
		SDL_Rect r = getArea();
		SDL_FillRect(target,&r,0);
	}*/
	aux::gfx_FillRect(target,x,y,w,h,0,opengl);
	font.render_to(target,x,y,c,caption);
	if(!opengl)
		SDL_UpdateRect(target,x,y,w,h);//Flip(target);
}

void CPushButton::mouseEnter()
{
	highlight = true;
	repaint();
}
std::string CPushButton::getCaption()
{
	return caption;
}

void CPushButton::mouseLeave()
{
	highlight = false;
	repaint();
}

void CPushButton::mouseDown(int x, int y, int button)
{
	if(onClick!=0)
		onClick(this,userdata);
}

/** CTextEdit Class **/
CTextEdit::CTextEdit(CControl* o, int x, int y, int w, std::string Text, bool number, int fontsize, SDL_Surface* t) : CControl(o,t),
	font(FONT_CONTROLFONT,fontsize,opengl)
{
	text = Text;
	//font = TTF_OpenFont((aux::data_path+FONT_CONTROLFONT).c_str(),fontsize);
	highlight = false;
	numbers = number;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = font.getHeight();//TTF_FontHeight(font);
	//onDestroy = DESTROYHOOK(CTextEditDestroy);
}

std::string CTextEdit::getText()
{
	return text;
}

void CTextEdit::paint(SDL_Surface* target)
{
	//SDL_Rect r = getArea();
	int col = SDL_MapRGB(target->format,0,0,highlight?255:127);
	aux::gfx_FillRect(target,x,y,w,h,col,opengl);
	font.render_to(target,x+2,y,255,highlight?127:0,0,text);
	if(!opengl)
		SDL_UpdateRect(target,x,y,w,h);//Flip(target);
}

void CTextEdit::mouseEnter()
{
	highlight = true;
	repaint();
}

void CTextEdit::setText(std::string txt)
{
	text = txt;
	repaint();
}

void CTextEdit::mouseLeave()
{
	highlight = false;
	repaint();
}

void CTextEdit::keyDown(SDL_keysym* sym)
{
	if(sym->sym==SDLK_BACKSPACE){
		text = text.substr(0,text.length()-1);
		repaint();
	}else if(sym->unicode>0x1F&&sym->unicode<0xFF){
		if(numbers&&(sym->unicode<'0'||sym->unicode>'9'))
			return;
		std::string ntext = text.c_str();
		ntext += (char)sym->unicode;
		if(font.textWidth(ntext)>w)
			return;
		text += (char)sym->unicode;
		repaint();
	}
}

/** CKeyEdit Class **/
CKeyEdit::CKeyEdit(CControl* o, int x, int y, int w, SDLKey k, int fontsize, SDL_Surface* t) : CControl(o,t),
	font(FONT_CONTROLFONT,fontsize,opengl)
{
	key = k;
	//font = TTF_OpenFont((aux::data_path+FONT_CONTROLFONT).c_str(),fontsize);
	highlight = false;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = font.getHeight();//TTF_FontHeight(font);
	//onDestroy = DESTROYHOOK(CKeyEditDestroy);
}

SDLKey CKeyEdit::getKey()
{
	return key;
}

void CKeyEdit::paint(SDL_Surface* target)
{
	/*int col = SDL_MapRGB(target->format,0,0,highlight?255:127);
	std::string text = aux::str_keyName(key);
	SDL_Color c;
	c.r = 255;
	c.g = highlight?127:0;
	c.b = 0;
	SDL_Surface* tex;
	SDL_Rect r;
	r.x = x; r.y = y; r.w = w; r.h = h;
	SDL_FillRect(target,&r,col);
	r.x += 2; r.w -= 4; // Font Offset
	if(text.length()>0){
		tex = RENDERMENUFONT(font,text.c_str(),c);
		if(tex == 0) {
			printf("Can't render font\n");
			return;
		}
		SDL_BlitSurface(tex,0,target,&r);
		SDL_FreeSurface(tex);
	}*/
	int col = SDL_MapRGB(target->format,0,0,highlight?255:127);
	aux::gfx_FillRect(target,x,y,w,h,col,opengl);
	font.render_to(target,x+2,y,255,highlight?127:0,0,aux::str_keyName(key));
	if(!opengl)
		SDL_UpdateRect(target,x,y,w,h);//Flip(target);
}

void CKeyEdit::mouseEnter()
{
	highlight = true;
	repaint();
}

void CKeyEdit::mouseLeave()
{
	highlight = false;
	repaint();
}

void CKeyEdit::keyDown(SDL_keysym* sym)
{
	key = sym->sym;
	repaint();
}

void CKeyEdit::mouseDown(int x,int y, int button)
{
	key = (SDLKey)(SDLK_WORLD_42+button);
	repaint();
}

/** CColorEdit Class **/

CColorEdit::CColorEdit(CControl* o, int x, int y, int w, SDL_Color col, int fontsize, SDL_Surface* t) : CControl(o,t),
	font(FONT_MENUFONT,fontsize,opengl)
{
	color = col;
	//font = TTF_OpenFont((aux::data_path+FONT_MENUFONT).c_str(),fontsize);
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = font.getHeight();//TTF_FontHeight(font);
	onChange = 0;
	//onDestroy = DESTROYHOOK(CColorEditDestroy);
}

void CColorEdit::setOnChange(CEventCallback OnChange, void* data)
{
	onChange = OnChange;
	userdata = data;
}

SDL_Color CColorEdit::getColor()
{
	return color;
}

void CColorEdit::paint(SDL_Surface* target)
{
	int col = SDL_MapRGB(target->format,0,0,127);
	int col2 = SDL_MapRGB(target->format,color.r,color.g,color.b);
	
	SDL_Rect r,cr;
	
	r.y = y; cr.y = r.y+2;
	r.w = (w-20)/3; cr.w = r.w-4;
	r.h = h; cr.h = r.h-4;
	
	r.x = x; cr.x = r.x+2;
	r.w = (w-20)/3; cr.w = ((r.w-4)*color.r)/255;
	aux::gfx_FillRect(target,r.x,r.y,r.w,r.h,col,opengl);
	aux::gfx_FillRect(target,cr.x,cr.y,cr.w,cr.h,col2,opengl);
	//tex = RENDERMENUFONT(font,CCapt::CAPT_REDCOMP.c_str(),c);
	//if(tex == 0) {
	//	printf("Can't render font\n");
	//	return;
	//}
	//SDL_BlitSurface(tex,0,target,&r);
	//SDL_FreeSurface(tex);
	font.render_to(target,r.x,r.y,0,0,0,CCapt::CAPT_REDCOMP);
	
	r.x = x+((w-20)/3)+10; cr.x = r.x+2;
	r.w = (w-20)/3; cr.w = ((r.w-4)*color.g)/255;
	aux::gfx_FillRect(target,r.x,r.y,r.w,r.h,col,opengl);
	aux::gfx_FillRect(target,cr.x,cr.y,cr.w,cr.h,col2,opengl);
//	tex = RENDERMENUFONT(font,CCapt::CAPT_GREENCOMP.c_str(),c);
//	if(tex == 0) {
//		printf("Can't render font\n");
//		return;
//	}
//	SDL_BlitSurface(tex,0,target,&r);
//	SDL_FreeSurface(tex);
	font.render_to(target,r.x,r.y,0,0,0,CCapt::CAPT_GREENCOMP);
	
	r.x = x+2*((w-20)/3)+20; cr.x = r.x+2;
	r.w = (w-20)/3; cr.w = ((r.w-4)*color.b)/255;
	aux::gfx_FillRect(target,r.x,r.y,r.w,r.h,col,opengl);
	aux::gfx_FillRect(target,cr.x,cr.y,cr.w,cr.h,col2,opengl);
//	tex = RENDERMENUFONT(font,CCapt::CAPT_BLUECOMP.c_str(),c);
//	if(tex == 0) {
//		printf("Can't render font\n");
//		return;
//	}
//	SDL_BlitSurface(tex,0,target,&r);
//	SDL_FreeSurface(tex);
	font.render_to(target,r.x,r.y,0,0,0,CCapt::CAPT_BLUECOMP);
	
	if(!opengl)
		SDL_UpdateRect(target,x,y,w,h);//Flip(target);
}

void CColorEdit::mouseDown(int x,int y, int button)
{
	int lx = 2;
	int rx = ((w-20)/3)-4;
	if(x>=lx&&x<=rx){
		color.r = (x-lx)*255/(rx-lx);
	}
	lx = ((w-20)/3)+12;
	rx = 2*((w-20)/3)+8;
	if(x>=lx&&x<=rx){
		color.g = (x-lx)*255/(rx-lx);
	}
	lx = 2*((w-20)/3)+22;
	rx = w-4;
	if(x>=lx&&x<=rx){
		color.b = (x-lx)*255/(rx-lx);
	}
	if(onChange)
		onChange(this,userdata);
	repaint();
}

/** CNumberEdit Class **/
CNumberEdit::CNumberEdit(CControl* o, int x, int y, int w, int num, std::string Caption, int fontsize, SDL_Surface* t) : CControl(o,t),
	font(FONT_MENUFONT,fontsize,opengl)
{
	value = num;
	if(value>1000)
		value=1000;
	if(value<0)
		value=0;
	caption = Caption;
	highlight = false;
//	font = TTF_OpenFont((aux::data_path+FONT_MENUFONT).c_str(),fontsize);
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = font.getHeight();//TTF_FontHeight(font);
	//onDestroy = DESTROYHOOK(CNumberEditDestroy);
}

int CNumberEdit::getValue()
{
	return value;
}

void CNumberEdit::paint(SDL_Surface* target)
{
	int col = SDL_MapRGB(target->format,0,0,highlight?255:127);
	int col2 = SDL_MapRGB(target->format,0,highlight?255:127,0);
	//SDL_Color c;
	//c.r = 255;
	//c.g = highlight?127:0;
	//c.b = 0;
	
	//SDL_Surface* tex;
	
	SDL_Rect r,cr;
	
	r.x = x; cr.x = r.x+2;
	r.y = y; cr.y = r.y+2;
	r.w = w; cr.w = ((r.w-4)*value)/1000;
	r.h = h; cr.h = r.h-4;
	
	
	aux::gfx_FillRect(target,r.x,r.y,r.w,r.h,col,opengl);
	aux::gfx_FillRect(target,cr.x,cr.y,cr.w,cr.h,col2,opengl);
	/*tex = RENDERMENUFONT(font,caption.c_str(),c);
	if(tex == 0) {
		printf("Can't render font\n");
		return;
	}
	SDL_BlitSurface(tex,0,target,&r);
	SDL_FreeSurface(tex);
	*/
	font.render_to(target,x,y,255,highlight?127:0,0,caption);
	
	if(!opengl)
		SDL_UpdateRect(target,x,y,w,h);//Flip(target);
}

void CNumberEdit::mouseEnter()
{
	highlight = true;
	repaint();
}

void CNumberEdit::mouseLeave()
{
	highlight = false;
	repaint();
}

void CNumberEdit::mouseDown(int x,int y, int button)
{
	int lx = 2;
	int rx = w-4;
	if(x>=lx&&x<=rx){
		value = (x-lx)*1000/(rx-lx);
	}
	repaint();	
}

void CNumberEdit::setValue(int value)
{
	this->value = value;
	repaint();
}


/** CImageList Class **/
CImageList::~CImageList()
{
	if(opengl)
		glDeleteTextures(1,&img_tex);

	if(img)
		SDL_FreeSurface(img);
}

CImageList::CImageList(CControl* o, int x, int y, int h, int num, std::string bitmap, SDL_Surface* t):CControl(o,t)
{
	image = num;
	img = SDL_LoadBMP(bitmap.c_str());
	numImages = img->h/h;
	this->x = x;
	this->y = y;
	this->w = img->w;
	this->h = h;
	onChange = 0;
	if(opengl)
		img_tex = aux::SDL_GL_LoadTexture(img,img_texcoords);
	//onDestroy = DESTROYHOOK(CImageListDestroy);
}

void CImageList::setOnChange(CEventCallback OnChange, void* data)
{
	onChange = OnChange;
	userdata = data;
}

int CImageList::getImage()
{
	return image;
}

void CImageList::paint(SDL_Surface* target)
{
	if(opengl) {
		GLfloat c_texcoords[4];
		c_texcoords[0] = img_texcoords[0];
		c_texcoords[1] = h*image*(img_texcoords[3]/img->h);
		c_texcoords[2] = img_texcoords[2];
		c_texcoords[3] = h*(image+1)*(img_texcoords[3]/img->h);
		aux::SDL_GL_Blit(img_tex,getArea(),c_texcoords);
	} else {
		SDL_Rect r1,r2;
		r1.x = x; r2.x = 0;
		r1.y = y; r2.y = image*h;
		r1.h = h; r2.h = h;
		r1.w = w; r2.w = w;
		SDL_BlitSurface(img,&r2,target,&r1);
		SDL_UpdateRect(target,x,y,w,h);//Flip(target);
	}
}

void CImageList::mouseDown(int x,int y, int button)
{
	image = (image+1)%numImages;
	if(onChange)
		onChange(this,userdata);
	repaint();
	//((CMenu*)getRoot())->paintElement(this);

}


/** CCheckBox Class **/
CCheckBox::~CCheckBox()
{
	if(opengl)
		glDeleteTextures(1,&img_tex);

	if(img)
		SDL_FreeSurface(img);
}

CCheckBox::CCheckBox(CControl* o, int x, int y, std::string capt, bool chk, int fontsize, SDL_Surface* t) : CControl(o,t),
	font(FONT_MENUFONT,fontsize,opengl)
{
	onChange = 0;
//	int ww,hh;
	//onClick = clicked;
	//this->userdata = userdata;
	img = SDL_LoadBMP((aux::data_path+CHECKBOX_IMAGE_FILE).c_str());
//	font = TTF_OpenFont((aux::data_path+FONT_MENUFONT).c_str(),fontsize);
	checked = chk;
	caption = capt;
//	TTF_SizeText(font,caption.c_str(),&ww,&hh);
	this->x = x;
	this->y = y;
	this->w = img->w+font.textWidth(caption);
	this->h = (img->h/4)>font.getHeight()?(img->h/4):font.getHeight();	
	highlight = false;
	if(opengl)
		img_tex = aux::SDL_GL_LoadTexture(img,img_texcoords);
	//onDestroy = DESTROYHOOK(CCheckBoxDestroy);
}

void CCheckBox::setOnChange(CEventCallback OnChange, void* data)
{
	onChange = OnChange;
	userdata = data;
}

bool CCheckBox::isChecked()
{
	return checked;
}

void CCheckBox::mouseEnter()
{
	highlight = true;
	repaint();
}

void CCheckBox::mouseLeave()
{
	highlight = false;
	repaint();
}

void CCheckBox::paint(SDL_Surface* target)
{
	//SDL_Color c;
	//c.r = 255;
	//c.g = highlight?127:0;
	//c.b = 0;
	//SDL_Surface* tex;
	//tex = RENDERMENUFONT(font,caption.c_str(),c);
	//if(tex == 0) {
	//	printf("Can't render font\n");
	//	return;
	//}
	aux::gfx_FillRect(target,x,y,w,h,0,opengl);
	if(opengl) {
		int image = 0;
		if(!checked) image += 1;
		if(!highlight) image += 2;
		GLfloat c_texcoords[4];
		c_texcoords[0] = img_texcoords[0];
		c_texcoords[1] = h*image*(img_texcoords[3]/img->h);
		c_texcoords[2] = img_texcoords[2];
		c_texcoords[3] = h*(image+1)*(img_texcoords[3]/img->h);
		SDL_Rect r;
		r.x = x; r.y = y; r.w = img->w; r.h = img->h/4;
		aux::SDL_GL_Blit(img_tex,r,c_texcoords);
	} else {
		SDL_Rect r, r2;
		int hh = 0;
		if(!checked) hh += img->h/4;
		if(!highlight) hh += img->h/2;
		r.x = x; r.y = y; r.w = w; r.h = h;
		r2.x = 0; r2.y = hh; r2.w = img->w; r2.h = img->h/4;	
		SDL_BlitSurface(img,&r2,target,&r);
	}
	
	//r.x = x+img->w; r.y = y; r.w = w; r.h = h;
	font.render_to(target,x+img->w,y,255,highlight?127:0,0,caption);
	//SDL_BlitSurface(tex,0,target,&r);
	
	//SDL_FreeSurface(tex);
	if(!opengl)
		SDL_UpdateRect(target,x,y,w,h);//Flip(target);
}

void CCheckBox::mouseDown(int x,int y, int button)
{
	checked = !checked;
	repaint();
	if(onChange)
		onChange(this,userdata);
}

void CCheckBox::setChecked(bool checked)
{
	this->checked = checked;
	repaint();
	if(onChange)
		onChange(this,userdata);
}

/** CList Class **/
CList::~CList()
{
	clearElements();
}

CList::CList(CControl* o, int x, int y, int w, int fontsize, SDL_Surface* t) : CControl(o,t),
	font(FONT_MENUFONT,fontsize,opengl)
{
//	font = TTF_OpenFont((aux::data_path+FONT_MENUFONT).c_str(),fontsize);
	elements.clear();
	highlight = -1;
	this->x = x;
	this->y = y;
	this->h = 0;
	this->w = w;
	objheight = (font.getHeight()+2);
	//onDestroy = DESTROYHOOK(CListDestroy);
	oldh = 0;
	onClick = 0;
}

void CList::setOnElementClick(ListClickCallback clicked, void* data)
{
	onClick = clicked;
	userdata = data;
}

//		TTF_Font* font;
std::vector<std::string> CList::getElements()
{
	return elements;
}

void CList::clearElements()
{
	elements.clear();
	h = 0;
	//repaint();
}

void CList::addElement(std::string element)
{
	elements.push_back(element);
	h = objheight*elements.size();
	//repaint();
}

void CList::removeElement(std::string element)
{
	std::vector<std::string>::iterator it;
	for(it = elements.begin(); it != elements.end(); it++){
		std::string str = *it;
		if(str==element){
			elements.erase(it);
		}
	}
	h = objheight*elements.size();
	repaint();
}

//	protected:
//		std::vector<std::string> elements;
//		int highlight;
//void (*onClick)(CControl*,std::string,void*);
//void* userdata;
void CList::paint(SDL_Surface* target)
{
//	SDL_Rect r = getArea();
	aux::gfx_FillRect(target,x,y,w,oldh,0,opengl);
	//SDL_UpdateRect(target,x,y,w,oldh);
	if(!elements.empty()){
		//SDL_Color c;
		//SDL_Surface* tex;
		int j = 0;
		for(int i=0;i<elements.size();i++){
			std::string l1 = elements[i];
			//c.r = 255;
			//c.g = (highlight==i)?127:0;
			//c.b = 0;
			//tex = RENDERMENUFONT(font,l1.c_str(),c);
			//if(tex == 0) {
			//	printf("Can't render font\n");
			//	continue;
			//}
			//r.x = x;
			//r.y = y+j++*objheight/2;
			//r.w = w;
			//r.h = objheight/2;
			//SDL_BlitSurface(tex,0,target,&r);
			//SDL_FreeSurface(tex);
			font.render_to(target,x,y+j++*objheight,255,(highlight==i)?127:0,0,l1);
			//if(l2.length() > 0)
			//	font.render_to(target,x,y+j++*objheight/2,255,(highlight==i)?127:0,0,l2);
			/*if(l2 != ""){
				tex = RENDERMENUFONT(font,l2.c_str(),c);
				if(tex == 0) {
					printf("Can't render font\n");
					continue;
				}
				r.x = x;
				r.y = y+j++*objheight/2;
				r.w = w;
				r.h = objheight/2;
				SDL_BlitSurface(tex,0,target,&r);
				SDL_FreeSurface(tex);
			}*/
			
		}
	}
//	SDL_Flip(target);//Flip(target);
	if(!opengl)
		SDL_UpdateRect(target,x,y,w,oldh>h?oldh:h);
	oldh = h;
}

void CList::mouseLeave()
{
	highlight = -1;
	repaint();
}

void CList::mouseMove(int x, int y)
{
	if(highlight != y/objheight) {
		highlight = y/objheight;
		repaint();
	}
}

void CList::mouseDown(int x, int y, int button)
{
	if(onClick) {
		std::string el = elements[y/objheight];
		onClick(this,button,el,userdata);
	}
}

/** CTooltip Class **/
CTooltip::CTooltip(CControl* o, int w, std::string Title, std::string Text, int title_fontsize, int text_fontsize, SDL_Surface* t) : CControl(o,t),
		title_font(FONT_CONTROLFONT,title_fontsize,opengl),
		text_font(FONT_CONTROLFONT,text_fontsize,opengl)		
{
	visible = false;
	title = Title;
	text = Text;
	this->x = 0;
	this->y = 0;
	this->w = w;
	this->h = calc_height();

	if(!opengl) {
		back = SDL_CreateRGBSurface(SDL_SWSURFACE,this->w,this->h,32,0,0,0,0);
		buffer = SDL_CreateRGBSurface(SDL_SWSURFACE,this->w,this->h,32,0,0,0,0);
		real_paint(buffer);
	}
}

CTooltip::~CTooltip()
{
	((CMenu*)getRoot())->unpaintElement(this);
	if(!opengl)
		hide();
	if(!opengl) {
		SDL_FreeSurface(back);
		SDL_FreeSurface(buffer);
	}
}


void CTooltip::show(int x, int y)
{
	if(visible && !opengl)
		hide();
		
	if(x+w > SCREENWIDTH)
		x = SCREENWIDTH - w;
	if(y+h > SCREENHEIGHT)
		y = SCREENHEIGHT - h;
	this->x = x;
	this->y = y;
	if(!opengl) {
		SDL_Rect area = getArea();
		SDL_Rect size = getSize();
		SDL_BlitSurface(getTarget(),&area,back,&size);
	}
	visible = true;
	printf("making tooltip visible at %d,%d - size %d,%d - ogl: %d\n",x,y,w,h,opengl);
	((CMenu*)getRoot())->paintElement(this);
/*	repaint();   */
}

void CTooltip::hide()
{
	if(!visible)
		return;
	if(!opengl) {
		SDL_Rect area = getArea();
		SDL_Rect size = getSize();
		SDL_BlitSurface(back,&size,getTarget(),&area);
		SDL_UpdateRect(getTarget(),x,y,w,h);
	} else
		((CMenu*)getRoot())->paintElement(getRoot());
	visible = false;
	//repaint();
	printf("hiding tooltip\n");
}

void CTooltip::paint(SDL_Surface* target)
{
	if(!visible)
		return;
	SDL_Rect area = getArea();
	if(opengl) {
		//aux::SDL_GL_Blit(buffer_tex,area,buffer_texcoords);
		real_paint(0);
		//  SDL_GL_SwapBuffers();
	} else {
		SDL_Rect size = getSize();
		//SDL_FillRect(target,&area,SDL_MapRGB(target->format,255,255,255));
		SDL_BlitSurface(buffer,&size,target,&area);
		SDL_UpdateRect(target,x,y,w,h);
	}
}

#define TitleSpace 5
#define LeftSpace 5
#define RightSpace 5
#define TopSpace 2
#define BottomSpace 2

void CTooltip::real_paint(SDL_Surface* target)
{
	int xo = opengl?x:0;
	int yo = opengl?y:0;
	// Global Outline (Red) 
	printf("call to real_paint - ogl: %d \n",opengl);
	aux::gfx_FillRect(target,xo,yo,w,h,0xFF0000,opengl);
	// Title Bar
	int cur_y = 1;
	aux::gfx_FillRect(target,xo+1,yo+cur_y,w-2,TopSpace + title_font.getHeight() + TitleSpace/2,0xFF5500,opengl);
	title_font.render_to(target,xo+LeftSpace,yo+cur_y+TopSpace,0,0,0,title);
	cur_y += title_font.getHeight() + TitleSpace/2 + TopSpace;
	// Text Box
	aux::gfx_FillRect(target,xo+1,yo+cur_y,w-2,h-cur_y-1,0xFF9900,opengl);
	cur_y += TitleSpace/2;
	int line_h = text_font.getHeight();
	string_list lines = text_font.wordWrap(text, w - (RightSpace+LeftSpace));
	for (unsigned int i=0;i<lines.size();i++) {
		text_font.render_to(target,xo+LeftSpace,yo+cur_y,0,0,0,lines[i]);
		cur_y += line_h;
	}
}

bool CTooltip::isVisible()
{
	return visible;
}

int CTooltip::calc_height()
{
	std::string txt = text.c_str();
	int hh = TopSpace + BottomSpace;
	hh += title_font.getHeight();
	hh += TitleSpace;
	int line_h = text_font.getHeight();
	string_list lines = text_font.wordWrap(text, w - (RightSpace+LeftSpace));
	hh += lines.size()*line_h;
	return hh;
}


/** CTabControl Class **/
CTabControl::CTabControl(CControl* o, int x, int y, int w, int h, int fontsize, SDL_Surface* t) : CControl(o,t),
		font(FONT_CONTROLFONT,fontsize,opengl)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	activeTab = 0;
	onTabChanged = 0;
}

void CTabControl::pmouseDown(int x, int y, int button)
{
	mouseDown(globalToLocalX(x),globalToLocalY(y),button);
	if(activeTab<0)
		return;
	CControl* control = childs[activeTab];
	if(control==0) 
		return;
	if(control->isMouseIn())
		control->pmouseDown(x,y,button);
}

void CTabControl::pmouseUp(int x, int y, int button)
{
	mouseUp(globalToLocalX(x),globalToLocalY(y),button);
	if(activeTab<0)
		return;
	CControl* control = childs[activeTab];
	if(control==0) 
		return;
	if(control->isMouseIn())
		control->pmouseUp(x,y,button);
}

void CTabControl::pmouseMove(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
	mouseMove(globalToLocalX(x),globalToLocalY(y));
	if(activeTab<0)
		return;
	CControl* control = childs[activeTab];
	if(control==0) 
		return;
	if(control->isMouseIn()&&!(control->isInBounds(x,y)))
		control->pmouseLeave();
	if(!(control->isMouseIn())&&control->isInBounds(x,y))
		control->pmouseEnter(x,y);
	if(control->isMouseIn())
		control->pmouseMove(x,y);
}

void CTabControl::pkeyDown(SDL_keysym *sym)
{
	keyDown(sym);
	if(activeTab<0)
		return;
	CControl* control = childs[activeTab];
	if(control==0) 
		return;
	if(control->isMouseIn())
		control->pkeyDown(sym);
}

void CTabControl::repaint(bool flip)
{
	paint(getTarget());
	if(activeTab<0)
		return;
	CControl* control = childs[activeTab];
	if(control==0)
		return;
	control->repaint();
	if(opengl && flip)
		SDL_GL_SwapBuffers();
	else
		SDL_UpdateRect(getTarget(),x,y,w,h);
}

#define TAB_HEIGHT 30
#define TAB_TEXT_TOP_SPACE 2
#define TAB_TEXT_SIDE_SPACE 6

void CTabControl::paint(SDL_Surface* target)
{
	aux::gfx_DrawRect(target,x,y+TAB_HEIGHT,w,h-TAB_HEIGHT,0xFF0000,opengl);
	int cur_x = x;
	int cur = 0;
	std::vector<CControl*>::iterator it;
	for(it = childs.begin(); it != childs.end(); it++){
		CTabWidget* tab = (CTabWidget*)(*it);
		if(tab==0)
			continue;
		int cur_tab_w = font.textWidth(tab->getName()) + 2*TAB_TEXT_SIDE_SPACE;
		aux::gfx_DrawRect(target,cur_x,y,cur_tab_w,TAB_HEIGHT,0xFF0000,opengl);
		if(activeTab==cur) 
			aux::gfx_FillRect(target,cur_x+1,y+1,cur_tab_w-2,TAB_HEIGHT,0xFFFF00,opengl);
		else
			aux::gfx_FillRect(target,cur_x+1,y+1,cur_tab_w-2,TAB_HEIGHT-2,0xFF7F00,opengl);
		font.render_to(target,cur_x+TAB_TEXT_SIDE_SPACE,y+TAB_TEXT_TOP_SPACE,0,0,0,tab->getName());
		cur_x += cur_tab_w;
		cur++;
	}
}

void CTabControl::mouseDown(int x, int y, int button)
{
	if(y > TAB_HEIGHT)
		return;
	int cur_x = 0;
	int cur = 0;
	std::vector<CControl*>::iterator it;
	for(it = childs.begin(); it != childs.end(); it++){
		CTabWidget* tab = (CTabWidget*)(*it);
		if(tab==0)
			continue;
		int cur_tab_w = font.textWidth(tab->getName()) + 2*TAB_TEXT_SIDE_SPACE;
		if(cur != activeTab && x >= cur_x && x <= cur_x+cur_tab_w) {
			if(childs[activeTab])
				childs[activeTab]->invalidate(0);
			activeTab = cur;
			repaint();
			if(onTabChanged)
				onTabChanged(this,userdata);
			return;
		}
		cur_x += cur_tab_w;
		cur++;
	}
}

int CTabControl::getTabX()
{
	return x + 1;
}

int CTabControl::getTabY()
{
	return y + TAB_HEIGHT + 1;
}

int CTabControl::getTabW()
{
	return w - 2;
}

int CTabControl::getTabH()
{
	return (h - TAB_HEIGHT) - 2;
}

int CTabControl::getActiveTab()
{
	return activeTab;
}

void CTabControl::setOnTabChanged(CEventCallback cb, void* data)
{
	onTabChanged = cb;
	userdata = data;
}


CTabWidget::CTabWidget(CTabControl* o, std::string Name, SDL_Surface* t) : CControl(o,t)
{
	name = Name;
	x = o->getTabX();
	y = o->getTabY();
	w = o->getTabW();
	h = o->getTabH();
}

std::string CTabWidget::getName()
{
	return name;
}

void CTabWidget::paint(SDL_Surface* target)
{
	aux::gfx_FillRect(target,x,y,w,h,0,opengl);
	if(!opengl)
		SDL_UpdateRect(getTarget(),x,y,w,h);
}


CTextArea::CTextArea(CControl* o, int x, int y, int w, std::string text, int fontsize, SDL_Surface* t) : CControl(o,t),
	font(FONT_CONTROLFONT,fontsize,opengl)
{
	this->text = text;
	this->x = x;
	this->y = y;
	this->w = w;
	string_list lines = font.wordWrap(text,w);
	h = lines.size()*font.getHeight();
}

void CTextArea::setText(std::string text)
{
	this->text = text;
	string_list lines = font.wordWrap(text,w);
	h = lines.size()*font.getHeight();
}

void CTextArea::paint(SDL_Surface* target)
{
	string_list lines = font.wordWrap(text,w);
	aux::gfx_FillRect(target,x,y,w,h,0,opengl);
	int cur_y = 0;
	for (unsigned int i=0;i<lines.size();i++) {
		font.render_to(target,x,y+cur_y,255,127,0,lines[i]);
		cur_y += font.getHeight();
	}
	if(!opengl)
		SDL_UpdateRect(target,x,y,w,h);
}


CTable::CTable(CControl* o, int x, int y, int w, int h, TableRow captions, int fontsize, SDL_Surface* t) : CControl(o,t),
	font(FONT_CONTROLFONT,fontsize,opengl)
{
	onChoice = 0;
	this->colCount = captions.size();
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	titles = captions;
	current = -1;
	rowH = font.getHeight();
	titleH = font.getHeight()+2;
	widths = 0;
}

CTable::~CTable()
{
	clearRows();
	if(widths)
		delete widths;
}

TableRow* CTable::getCurrentRow()
{
	if(current>=0 && current<rows.size())
		return &(rows[current]);
	return 0;
}

void CTable::addRow(const TableRow& row)
{
	rows.push_back(row);
}

void CTable::setColWidths(ColWidths* w)
{
	if(widths)
		delete widths;
	widths = w;
}

void CTable::clearRows()
{
	rows.clear();
}

void CTable::setOnChoice(CEventCallback cb, void* data)
{
	onChoice = cb;
	userdata = data;
}

void CTable::paint(SDL_Surface* target)
{
	aux::gfx_DrawRect(target,x,y,w,h,0xFF0000,opengl);
	aux::gfx_FillRect(target,x+1,y+1,w-2,h-2,0x000000,opengl);
	int colW;
	int curX = 0;
	for(int c=0;c<colCount;c++) {
		if(widths)
			colW = widths->at(c);
		else
			colW = w/colCount;

		aux::gfx_FillRect(target,x+curX+1,y+1,colW-1,titleH-2,0xFF7F00,opengl);
		font.render_to(target,x+curX+3,y,0,0,0,titles[c]);
		curX += colW;
		if(curX >= w)
			continue;
		aux::gfx_FillRect(target,x+curX,y,1,h,0xFF0000,opengl);
	}
	for(int r=0;r<rows.size();r++) {
		curX = 0;
		for(int c=0;c<colCount;c++) {
			if(widths)
				colW = widths->at(c);
			else
				colW = w/colCount;
			if(current==r)
				font.render_to(target,x+curX+3,y+titleH+rowH*r,255,127,0,rows[r][c]);
			else
				font.render_to(target,x+curX+3,y+titleH+rowH*r,255,0,0,rows[r][c]);
			curX += colW;
		}
	}
	aux::gfx_DrawRect(target,x,y,w,titleH,0xFF0000,opengl);
		
	if(!opengl)
		SDL_UpdateRect(target,x,y,w,h);
}

void CTable::mouseDown(int x, int y, int button)
{
	if(onChoice)
		onChoice(this,userdata);
}

void CTable::mouseMove(int x, int y)
{
	int newHL = ((y-titleH)/rowH);
	if(y < titleH)
		newHL = -1;
	if(newHL >= rows.size())
		newHL = -1;
	if(newHL != current) {
		current = newHL;
		repaint();
	}
}

void CTable::mouseLeave()
{
	current = -1;
	repaint();
}

/*typedef std::vector<std::string> TableRow;
class CTable : public CControl {
	public:
		CTable(CControl* o, int x, int y, int w, int rowCount, int colCount, TableRow captions, int fontsize = FONT_CONTROL_TABLESIZE, SDL_Surface* t = 0);
		~CTable();
		TableRow* getCurrentRow();
		void addRow(const TableRow& row);
		void clearRows();
		void setOnChoice(CEventCallback cb = 0, void* data = 0);
	protected:
		CEventCallback onChoice;
		void* userdata;
		int current;
		CFont font;
		TableRow titles;
		std::vector<TableRow> rows;
		virtual void paint(SDL_Surface* target);
		virtual void mouseDown(int x, int y, int button);
};*/


CPopup::CPopup(CControl* o, int w, int h, std::string title, int fontsize, SDL_Surface* t) : CMenu(0,t?t:o->getTarget()),
	title_font(FONT_MENUFONT,fontsize,opengl)
{
	root = o->getRoot();
	caption = title;
	this->w = w;
	this->h = h;
	this->x = (getTarget()->w - this->w)/2;
	this->y = (getTarget()->h - this->h)/2;
	back = 0;
}

void CPopup::paint(SDL_Surface* target)
{
	aux::gfx_FillRect(target,x,y,w,h,0x000000,opengl);
	aux::gfx_DrawRect(target,x,y,w,h,0xFFFF00,opengl);
	int title_h = title_font.getHeight();
	int title_w = title_font.textWidth(caption);
	aux::gfx_DrawRect(target,x,y,w,title_h,0xFFFF00,opengl);
	aux::gfx_FillRect(target,x+1,y+1,w-2,title_h-2,0x00007F,opengl);
	title_font.render_to(target,x+(w-title_w)/2,y,255,42,0,caption);
	if(!opengl)
		SDL_UpdateRect(target,x,y,w,h);
}

void CPopup::menu_init()
{
	showTitle = false;
	if(back)
		SDL_FreeSurface(back);
	back = 0;
	if(opengl)
		return;
	back = SDL_CreateRGBSurface(SDL_SWSURFACE,this->w,this->h,32,0,0,0,0);
	SDL_Rect area = getArea();
	SDL_Rect size = getSize();
	SDL_BlitSurface(getTarget(),&area,back,&size);
}

void CPopup::menu_exit()
{
	if(opengl) {
		root->repaint();
		return;
	}
	if(!back)
		return;
	SDL_Rect area = getArea();
	SDL_Rect size = getSize();
	SDL_BlitSurface(back,&size,getTarget(),&area);
	SDL_UpdateRect(getTarget(),x,y,w,h);
	SDL_FreeSurface(back);
	back = 0;
}

/*void CPopup::addChild(CControl* child)
{
	child->setPos(child->getX()+x,child->getY()+y);
	childs.push_back(child);
}*/


#define POPUP_TITLESPACE 5
#define POPUP_BUTTONSPACE 5
#define POPUP_SIDESPACE 5
#define POPUP_BOTTOMSPACE 4


CPopupProgress::CPopupProgress(CControl* o, std::string title, std::string text, int w, int title_fontsize, int text_fontsize, SDL_Surface* t) : CPopup(o,w,42,title,title_fontsize,t)
{
	txt = new CTextArea(this,0,0,w-2*POPUP_SIDESPACE,text,text_fontsize);
	this->h = title_font.getHeight()+POPUP_TITLESPACE+txt->getH()+POPUP_BOTTOMSPACE;
	this->y = (getTarget()->h - this->h)/2;	
	txt->setPos(x+POPUP_SIDESPACE,y+title_font.getHeight()+POPUP_TITLESPACE);
}

void CPopupProgress::show()
{
	menu_init();
	repaint();
}

void CPopupProgress::hide()
{
	menu_exit();
}


void CPopupMessage_Okay(CControl* sender, void* pop)
{
	CPopupMessage* popup = (CPopupMessage*)pop;
	popup->requestClose();
}

CPopupMessage::CPopupMessage(CControl* o, std::string title, std::string text, int w, int title_fontsize, int text_fontsize, SDL_Surface* t) : CPopup(o,w,42,title,title_fontsize,t)
{
	txt = new CTextArea(this,0,0,w-2*POPUP_SIDESPACE,text,text_fontsize);
	ok_button = new CPushButton(this,0,0,CCapt::CAPT_POPUP_OKAY,FONT_POPUP_BUTTONSIZE);
	this->h = title_font.getHeight()+POPUP_TITLESPACE+txt->getH()+POPUP_BUTTONSPACE+ok_button->getH()+POPUP_BOTTOMSPACE;
	this->y = (getTarget()->h - this->h)/2;	
	txt->setPos(x+POPUP_SIDESPACE,y+title_font.getHeight()+POPUP_TITLESPACE);
	ok_button->setPos(x+(w-ok_button->getW())/2,y+title_font.getHeight()+POPUP_TITLESPACE+txt->getH()+POPUP_BUTTONSPACE);
	ok_button->setOnClick(CPopupMessage_Okay,this);
}



void CPopupDialog_Yes(CControl* sender, void* pop)
{
	CPopupDialog* popup = (CPopupDialog*)pop;
	popup->accept();
}

void CPopupDialog_No(CControl* sender, void* pop)
{
	CPopupDialog* popup = (CPopupDialog*)pop;
	popup->reject();
}

CPopupDialog::CPopupDialog(CControl* o, std::string title, std::string text, int w, int title_fontsize, int text_fontsize, SDL_Surface* t) : CPopup(o,w,42,title,title_fontsize,t)
{
	txt = new CTextArea(this,0,0,w-2*POPUP_SIDESPACE,text,text_fontsize);
	yes_button = new CPushButton(this,0,0,CCapt::CAPT_POPUP_APPLY,FONT_POPUP_BUTTONSIZE);
	no_button = new CPushButton(this,0,0,CCapt::CAPT_POPUP_CANCEL,FONT_POPUP_BUTTONSIZE);
	this->h = title_font.getHeight()+POPUP_TITLESPACE+txt->getH()+POPUP_BUTTONSPACE+yes_button->getH()+POPUP_BOTTOMSPACE;
	this->y = (getTarget()->h - this->h)/2;	
	txt->setPos(x+POPUP_SIDESPACE,y+title_font.getHeight()+POPUP_TITLESPACE);
	yes_button->setPos(x+1,y+title_font.getHeight()+POPUP_TITLESPACE+txt->getH()+POPUP_BUTTONSPACE);
	yes_button->setOnClick(CPopupDialog_Yes,this);
	no_button->setPos(x+(w-no_button->getW())-1,y+title_font.getHeight()+POPUP_TITLESPACE+txt->getH()+POPUP_BUTTONSPACE);
	no_button->setOnClick(CPopupDialog_No,this);
}

bool CPopupDialog::getValue()
{
	return value;
}

void CPopupDialog::accept()
{
	value = true;
	requestClose();
}

void CPopupDialog::reject()
{
	value = false;
	requestClose();
}


CPopupInput::CPopupInput(CControl* o, std::string title, std::string text, int w, int title_fontsize, int text_fontsize, SDL_Surface* t) : CPopupDialog(o,title,text,w,title_fontsize,text_fontsize,t)
{
	edit = new CTextEdit(this,0,0,w-2*POPUP_SIDESPACE,"");
	this->h = title_font.getHeight()+POPUP_TITLESPACE+txt->getH()+POPUP_BUTTONSPACE+edit->getH()+POPUP_BUTTONSPACE+yes_button->getH()+POPUP_BOTTOMSPACE;
	this->y = (getTarget()->h - this->h)/2;	
	txt->setPos(x+POPUP_SIDESPACE,y+title_font.getHeight()+POPUP_TITLESPACE);
	edit->setPos(x+POPUP_SIDESPACE,y+title_font.getHeight()+POPUP_TITLESPACE+txt->getH()+POPUP_BUTTONSPACE);
	yes_button->setPos(x+1,y+title_font.getHeight()+POPUP_TITLESPACE+txt->getH()+POPUP_BUTTONSPACE+edit->getH()+POPUP_BUTTONSPACE);
	no_button->setPos(x+(w-no_button->getW())-1,y+title_font.getHeight()+POPUP_TITLESPACE+txt->getH()+POPUP_BUTTONSPACE+edit->getH()+POPUP_BUTTONSPACE);

}

void CPopupInput::accept()
{
	if(getInput().length() < 1) {
		CPopupMessage msg(this,CCapt::CAPT_TITLE_EMPTY_STRING,CCapt::CAPT_TEXT_EMPTY_STRING);
		msg.execute();
		return;
	}
	value = true;
	requestClose();
}

std::string CPopupInput::getInput()
{
	return edit->getText();
}

void CPopupInput::pkeyDown(SDL_keysym* sym)
{
	keyDown(sym);
	edit->pkeyDown(sym);
}

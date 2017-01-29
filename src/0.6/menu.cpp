#include <stdio.h>
#include <string>
#include <vector>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "config.h"
#include "menu.h"
#include "text.h"

#define FULLSCREENFLAG SDL_FULLSCREEN

/** CControl Base Class **/

CControl::CControl(CControl* o,SDL_Surface* t)
{
	owner = o;
	target = t;
	closeReq = false;
	ownSDL = false;
	if(target==0){
		if(owner==0){
			ownSDL = true;
			SDL_Init(SDL_INIT_VIDEO);
			#ifdef WIN32
				target = SDL_SetVideoMode(SCREENWIDTH,SCREENHEIGHT,SCREENBPP,SDL_SWSURFACE|FULLSCREENFLAG);
			#else
				target = SDL_SetVideoMode(SCREENWIDTH,SCREENHEIGHT,SCREENBPP,SDL_HWSURFACE|FULLSCREENFLAG);
			#endif
			TTF_Init();
			SDL_EnableUNICODE(1);
		}else{
			target = owner->getTarget();
		}
	}
	if(owner!=0) 
		owner->addChild(this);
	mouseIn = false;
	onDestroy = 0;
}

CControl::~CControl()
{
	if(onDestroy)
		onDestroy(this);
	std::vector<CControl*>::iterator it;
	for(it = childs.begin(); it != childs.end(); it++){
		CControl* control = *it;
		delete control;
	}
	childs.clear();
	if(ownSDL){
		SDL_EnableUNICODE(0);
		TTF_Quit();
		SDL_Quit();
	}
}

void CControl::addChild(CControl* child)
{
	childs.push_back(child);
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
	if(owner!=0)
		owner->requestClose();
}

void CControl::invalidate()
{
// CLEAR ALL HIGHLIGHTS
	pmouseMove(-42,-42);
	repaint();
}

void CControl::repaint()
{
	paint(target);
	std::vector<CControl*>::iterator it;
	for(it = childs.begin(); it != childs.end(); it++){
		CControl* control = *it;
		if(control==0) continue;
		control->repaint();
	}
}

void CControl::pmouseEnter()
{
	mouseIn = true;
	mouseEnter();
}

void CControl::pmouseLeave()
{
	mouseIn = false;
	mouseLeave();
}

void CControl::pkeyDown(SDL_keysym *sym)
{
	std::vector<CControl*>::iterator it;
	for(it = childs.begin(); it != childs.end(); it++){
		CControl* control = *it;
		if(control==0) continue;
		if(control->isMouseIn())
			control->pkeyDown(sym);
	}
	keyDown(sym);
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
	std::vector<CControl*>::iterator it;
	for(it = childs.begin(); it != childs.end(); it++){
		CControl* control = *it;
		if(control==0) continue;
		if(control->isMouseIn()&&!(control->isInBounds(x,y)))
			control->pmouseLeave();
		if(!(control->isMouseIn())&&control->isInBounds(x,y))
			control->pmouseEnter();
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

/** CMenu Class **/
void CMenu::execute()
{
	repaint();
	while(!closing()){
		handleEvents();
	}

}

void CMenuDestroy(CMenu* menu)
{
	TTF_CloseFont(menu->titleFont);
}

CMenu::CMenu(CControl* o,SDL_Surface* t):CControl(o,t)
{
	titleFont = TTF_OpenFont(FONT_TITLEFONT,FONT_TITLESIZE);
	caption = "Untitled Menu";
	onDestroy = DESTROYHOOK(CMenuDestroy);
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
	SDL_Color c;
	c.r = 255;
	c.g = 127;
	c.b = 0;
	SDL_Surface* tex;
	SDL_Rect r;
	tex = RENDERMENUFONT(titleFont,caption.c_str(),c);
	if(tex == 0) {
		printf("Can't render font\n");
		return;
	}
	r.x = 0;
	r.y = 0;
	r.w = tex->w;
	r.h = tex->h;
	SDL_FillRect(target,0,0);
	SDL_BlitSurface(tex,&r,target,&r);
	SDL_FreeSurface(tex);
	SDL_Flip(target);
}
		
void CMenu::keyDown(SDL_keysym* sym)
{
	if(sym->sym==ABORT_KEY)
		requestClose();
}

/** CPushButton Class **/
void CPushButtonDestroy(CPushButton* b)
{
	TTF_CloseFont(b->font);
}

CPushButton::CPushButton(CControl* o, int x, int y, std::string Caption, void (*clicked)(CControl*,void*), void* data, int fontsize, SDL_Surface* t):CControl(o,t)
{
	font = TTF_OpenFont(FONT_MENUBUTTONFONT,fontsize);
	caption = Caption;
	onClick = clicked;
	userdata = data;
	highlight = false;
	this->x = x;
	this->y = y;
	TTF_SizeText(font,caption.c_str(),&(this->w),&(this->h));
	onDestroy = DESTROYHOOK(CPushButtonDestroy);
}

void CPushButton::paint(SDL_Surface* target)
{
	SDL_Color c;
	c.r = onClick?255:0;
	c.g = (highlight&&onClick)?127:0;
	c.b = onClick?0:255;
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
	SDL_FreeSurface(tex);
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
void CTextEditDestroy(CTextEdit* edit)
{
	TTF_CloseFont(edit->font);
}

CTextEdit::CTextEdit(CControl* o, int x, int y, int w, std::string Text, bool number, int fontsize, SDL_Surface* t):CControl(o,t)
{
	text = Text;
	font = TTF_OpenFont(FONT_MENUFONT,fontsize);
	highlight = false;
	numbers = number;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = TTF_FontHeight(font);
	onDestroy = DESTROYHOOK(CTextEditDestroy);
}

std::string CTextEdit::getText()
{
	return text;
}

void CTextEdit::paint(SDL_Surface* target)
{
	int col = SDL_MapRGB(target->format,0,0,highlight?255:127);
	SDL_Color c;
	c.r = 255;
	c.g = highlight?127:0;
	c.b = 0;
	SDL_Surface* tex;
	SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	SDL_FillRect(target,&r,col);
	if(text.length()>0){
		tex = RENDERMENUFONT(font,text.c_str(),c);
		if(tex == 0) {
			printf("Can't render font\n");
			return;
		}
		SDL_BlitSurface(tex,0,target,&r);
		SDL_FreeSurface(tex);
	}
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
	int nw,nh;
	std::string ntext;
	if(sym->unicode>0x1F&&sym->unicode<0x7F){
		if(numbers&&(sym->unicode<'0'||sym->unicode>'9'))
			return;
		ntext = text.c_str();
		ntext = ntext + (char)sym->unicode;
		TTF_SizeText(font,ntext.c_str(),&nw,&nh);
		if(nw>w)
			return;
		text = text + (char)sym->unicode;
		repaint();
	}
	else if(sym->sym==SDLK_BACKSPACE){
		text = text.substr(0,text.length()-1);
		repaint();
	}
}

/** CKeyEdit Class **/
void CKeyEditDestroy(CKeyEdit* ke)
{
	TTF_CloseFont(ke->font);
}

CKeyEdit::CKeyEdit(CControl* o, int x, int y, int w, SDLKey k, int fontsize, SDL_Surface* t):CControl(o,t)
{
	key = k;
	font = TTF_OpenFont(FONT_MENUFONT,fontsize);
	highlight = false;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = TTF_FontHeight(font);
	onDestroy = DESTROYHOOK(CKeyEditDestroy);
}

SDLKey CKeyEdit::getKey()
{
	return key;
}

void CKeyEdit::paint(SDL_Surface* target)
{
	int col = SDL_MapRGB(target->format,0,0,highlight?255:127);
	std::string text = "";
	if(key>=SDLK_WORLD_42&&key<SDLK_WORLD_52){
		char n = '0'+(key-SDLK_WORLD_42);
		text = "mouse ";
		text += n;
	}else if(key>=SDLK_KP0&&key<=SDLK_KP_EQUALS){
		text = "kp ";
		text += SDL_GetKeyName(key);
	}else{
		text = SDL_GetKeyName(key);
	}
	SDL_Color c;
	c.r = 255;
	c.g = highlight?127:0;
	c.b = 0;
	SDL_Surface* tex;
	SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	SDL_FillRect(target,&r,col);
	if(text.length()>0){
		tex = RENDERMENUFONT(font,text.c_str(),c);
		if(tex == 0) {
			printf("Can't render font\n");
			return;
		}
		SDL_BlitSurface(tex,0,target,&r);
		SDL_FreeSurface(tex);
	}
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

void CColorEditDestroy(CColorEdit* ed)
{
	TTF_CloseFont(ed->font);
}

CColorEdit::CColorEdit(CControl* o, int x, int y, int w, SDL_Color col, int fontsize, SDL_Surface* t):CControl(o,t)
{
	color = col;
	font = TTF_OpenFont(FONT_MENUFONT,fontsize);
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = TTF_FontHeight(font);
	onDestroy = DESTROYHOOK(CColorEditDestroy);
}

SDL_Color CColorEdit::getColor()
{
	return color;
}

void CColorEdit::paint(SDL_Surface* target)
{
	int col = SDL_MapRGB(target->format,0,0,127);
	int col2 = SDL_MapRGB(target->format,color.r,color.g,color.b);
	SDL_Color c;
	c.r = 0;
	c.g = 0;
	c.b = 0;
	
	SDL_Surface* tex;
	
	SDL_Rect r,cr;
	
	r.y = y; cr.y = r.y+2;
	r.w = (w-20)/3; cr.w = r.w-4;
	r.h = h; cr.h = r.h-4;
	
	r.x = x; cr.x = r.x+2;
	r.w = (w-20)/3; cr.w = ((r.w-4)*color.r)/255;
	SDL_FillRect(target,&r,col);
	SDL_FillRect(target,&cr,col2);
	tex = RENDERMENUFONT(font,CCapt::CAPT_REDCOMP.c_str(),c);
	if(tex == 0) {
		printf("Can't render font\n");
		return;
	}
	SDL_BlitSurface(tex,0,target,&r);
	SDL_FreeSurface(tex);
	
	r.x = x+((w-20)/3)+10; cr.x = r.x+2;
	r.w = (w-20)/3; cr.w = ((r.w-4)*color.g)/255;
	SDL_FillRect(target,&r,col);
	SDL_FillRect(target,&cr,col2);
	tex = RENDERMENUFONT(font,CCapt::CAPT_GREENCOMP.c_str(),c);
	if(tex == 0) {
		printf("Can't render font\n");
		return;
	}
	SDL_BlitSurface(tex,0,target,&r);
	SDL_FreeSurface(tex);
	
	r.x = x+2*((w-20)/3)+20; cr.x = r.x+2;
	r.w = (w-20)/3; cr.w = ((r.w-4)*color.b)/255;
	SDL_FillRect(target,&r,col);
	SDL_FillRect(target,&cr,col2);
	tex = RENDERMENUFONT(font,CCapt::CAPT_BLUECOMP.c_str(),c);
	if(tex == 0) {
		printf("Can't render font\n");
		return;
	}
	SDL_BlitSurface(tex,0,target,&r);
	SDL_FreeSurface(tex);
	
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
	repaint();
}

void CNumberEditDestroy(CNumberEdit* ed)
{
	TTF_CloseFont(ed->font);
}

CNumberEdit::CNumberEdit(CControl* o, int x, int y, int w, int num, std::string Caption, int fontsize, SDL_Surface* t):CControl(o,t)
{
	value = num;
	if(value>1000)
		value=1000;
	if(value<0)
		value=0;
	caption = Caption;
	highlight = false;
	font = TTF_OpenFont(FONT_MENUFONT,fontsize);
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = TTF_FontHeight(font);
	onDestroy = DESTROYHOOK(CNumberEditDestroy);
}

int CNumberEdit::getValue()
{
	return value;
}

void CNumberEdit::paint(SDL_Surface* target)
{
	int col = SDL_MapRGB(target->format,0,0,highlight?255:127);
	int col2 = SDL_MapRGB(target->format,0,highlight?255:127,0);
	SDL_Color c;
	c.r = 255;
	c.g = highlight?127:0;
	c.b = 0;
	
	SDL_Surface* tex;
	
	SDL_Rect r,cr;
	
	r.x = x; cr.x = r.x+2;
	r.y = y; cr.y = r.y+2;
	r.w = w; cr.w = ((r.w-4)*value)/1000;
	r.h = h; cr.h = r.h-4;
	
	
	SDL_FillRect(target,&r,col);
	SDL_FillRect(target,&cr,col2);
	tex = RENDERMENUFONT(font,caption.c_str(),c);
	if(tex == 0) {
		printf("Can't render font\n");
		return;
	}
	SDL_BlitSurface(tex,0,target,&r);
	SDL_FreeSurface(tex);
	
	
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

void CImageListDestroy(CImageList* il)
{
	SDL_FreeSurface(il->img);
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
	onDestroy = DESTROYHOOK(CImageListDestroy);
}

int CImageList::getImage()
{
	return image;
}

void CImageList::paint(SDL_Surface* target)
{
	SDL_Rect r1,r2;
	r1.x = x; r2.x = 0;
	r1.y = y; r2.y = image*h;
	r1.h = h; r2.h = h;
	r1.w = w; r2.w = w;
	SDL_BlitSurface(img,&r2,target,&r1);
	SDL_UpdateRect(target,x,y,w,h);//Flip(target);
}

void CImageList::mouseDown(int x,int y, int button)
{
	image = (image+1)%numImages;
	repaint();
}

//class CCheckBox:public CControl{
//	public:
void CCheckBoxDestroy(CCheckBox* cb)
{
	SDL_FreeSurface(cb->img);
	TTF_CloseFont(cb->font);
}

CCheckBox::CCheckBox(CControl* o, int x, int y, std::string capt, bool chk, void (*clicked)(CControl*,void*), void* userdata, int fontsize, SDL_Surface* t):CControl(o,t)
{
	int ww,hh;
	onClick = clicked;
	this->userdata = userdata;
	img = SDL_LoadBMP(CHECKBOX_IMAGE_FILE);
	font = TTF_OpenFont(FONT_MENUFONT,fontsize);
	checked = chk;
	caption = capt;
	TTF_SizeText(font,caption.c_str(),&ww,&hh);
	this->x = x;
	this->y = y;
	this->w = img->w+ww;
	this->h = (img->h/4)>hh?(img->h/4):hh;	
	highlight = false;
	onDestroy = DESTROYHOOK(CCheckBoxDestroy);
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
	SDL_Color c;
	c.r = 255;
	c.g = highlight?127:0;
	c.b = 0;
	SDL_Surface* tex;
	tex = RENDERMENUFONT(font,caption.c_str(),c);
	if(tex == 0) {
		printf("Can't render font\n");
		return;
	}
	SDL_Rect r,r2;
	r.x = x; r.y = y;	r.w = w;	r.h = h;
	SDL_FillRect(target,&r,0);
	int hh = 0;
	if(!checked) hh += img->h/4;
	if(!highlight) hh += img->h/2;
	r.x = x; r.y = y; r.w = w; r.h = h;
	r2.x = 0; r2.y = hh; r2.w = img->w; r2.h = img->h/4;	
	SDL_BlitSurface(img,&r2,target,&r);
	
	r.x = x+img->w; r.y = y; r.w = w; r.h = h;
	SDL_BlitSurface(tex,0,target,&r);
	
	SDL_FreeSurface(tex);
	SDL_UpdateRect(target,x,y,w,h);//Flip(target);
}

void CCheckBox::mouseDown(int x,int y, int button)
{
	checked = !checked;
	repaint();
	if(onClick)
		onClick(this,userdata);
}


//class CList :public CControl{
//	public:
# if 0
void CPushButtonDestroy(CPushButton* b)
{
	TTF_CloseFont(b->font);
}

CPushButton::CPushButton(CControl* o, int x, int y, std::string Caption, void (*clicked)(CControl*,void*), void* data, int fontsize, SDL_Surface* t):CControl(o,t)
{
	font = TTF_OpenFont(FONT_MENUBUTTONFONT,fontsize);
	caption = Caption;
	onClick = clicked;
	userdata = data;
	highlight = false;
	this->x = x;
	this->y = y;
	TTF_SizeText(font,caption.c_str(),&(this->w),&(this->h));
	onDestroy = DESTROYHOOK(CPushButtonDestroy);
}

void CPushButton::paint(SDL_Surface* target)
{
	SDL_Color c;
	c.r = onClick?255:0;
	c.g = (highlight&&onClick)?127:0;
	c.b = onClick?0:255;
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
	SDL_FreeSurface(tex);
	SDL_UpdateRect(target,x,y,w,h);//Flip(target);
}
# endif
void CListDestroy(CList* b)
{
	TTF_CloseFont(b->font);
	b->clearElements();
}

CList::CList(CControl* o, int x, int y, int w, void (*clicked)(CControl*,std::string,void*), void* data, int fontsize, SDL_Surface* t):CControl(o,t)
{
	font = TTF_OpenFont(FONT_MENUFONT,fontsize);
	elements.clear();
	onClick = clicked;
	userdata = data;
	highlight = -1;
	this->x = x;
	this->y = y;
	this->h = 0;
	this->w = w;
	objheight = (TTF_FontHeight(font)+2)*2;
	onDestroy = DESTROYHOOK(CListDestroy);
	oldh = 0;
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
	repaint();
}

void CList::addElement(std::string element)
{
	elements.push_back(element);
	h = objheight*elements.size();
	repaint();
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
	SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = oldh;
	SDL_FillRect(target,&r,0);
	oldh = h;
	SDL_UpdateRect(target,x,y,w,oldh);
	if(!elements.empty()){
		SDL_Color c;
		SDL_Surface* tex;
		int j = 0;
		for(int i=0;i<elements.size();i++){
			std::string::size_type pos = elements[i].find(" <br> ");
			std::string l1 = elements[i].substr(0,pos);
			std::string l2 = elements[i].substr(pos+6);
			c.r = 255;
			c.g = (highlight==i)?127:0;
			c.b = 0;
			tex = RENDERMENUFONT(font,l1.c_str(),c);
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
			if(l2 != ""){
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
			}
		}
	}
	SDL_Flip(target);//Flip(target);
}

void CList::mouseLeave()
{
	highlight = -1;
	repaint();
}

void CList::mouseMove(int x, int y)
{
	if(highlight!=y/objheight){
		highlight = y/objheight;
		repaint();
	}
}

void CList::mouseDown(int x, int y, int button)
{
	if(onClick){
		std::string el = elements[y/objheight];
		onClick(this,el,userdata);
	}
}

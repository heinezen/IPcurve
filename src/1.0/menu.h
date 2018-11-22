#ifndef MENU_H
#define MENU_H
#include <stdio.h>
#include <string>
#include <vector>
#include <SDL/SDL.h>
#include "config.h"
#include "text.h"
#include "font.h"
#include "timer.h"

//#define DESTROYHOOK(a) (void(*)(CControl*))(a)


class CTooltip;
class CControl;

typedef void (*CKeyCallback)(CControl*,SDL_keysym*,void*);
typedef void (*CEventCallback)(CControl*,void*);

class CControl{
	public:
		CControl(CControl* o,SDL_Surface* t = 0);
		virtual ~CControl();
		virtual void pmouseDown(int x, int y, int button);
		virtual void pmouseUp(int x, int y, int button);
		virtual void pmouseMove(int x, int y);
		virtual void pmouseEnter(int x, int y);
		virtual void pmouseLeave();
		virtual void pkeyDown(SDL_keysym* sym);
		virtual void addChild(CControl* child);
		virtual void deleteChild(CControl* child);
		virtual void repaint();
		void resetSDL();
		void cleanSDL();
		void setPos(int x, int y);
		int getX();
		int getY();
		int getW();
		int getH();
		int globalToLocalX(int xx);
		int globalToLocalY(int yy);
		bool isInBounds(int xx, int yy);
		bool isMouseIn();
		void requestClose();
		void invalidate(bool do_repaint = 1);
		SDL_Surface* getTarget();
		CControl* getOwner();
		SDL_Rect getArea();
		SDL_Rect getSize();
		void setTooltip(std::string title, std::string text);
		std::vector<CControl*> childs;
		void showTooltip();
		void hideTooltip();
		void hideAllTooltips();
		void setOnKeyDown(CKeyCallback OnKeyDown = 0, void* data = 0);
	protected:
		CTooltip* tooltip;
		CTimer* tooltip_timer;
		virtual void paint(SDL_Surface* target);
		virtual void mouseEnter();
		virtual void mouseLeave();
		virtual void mouseDown(int x, int y, int button);
		virtual void mouseUp(int x, int y, int button);
		virtual void mouseMove(int x, int y);
		virtual void keyDown(SDL_keysym* sym);
		//void (*onDestroy)(CControl*);
		bool closing();
		int x, y, w, h;
		int mouse_x, mouse_y;
		bool closeReq;
		bool mouseIn;
	private:
		bool ownSDL;
		SDL_Surface* target;
		CControl* owner;
		void* keyDown_data;
		CKeyCallback onKeyDown;
};

class CMenu : public CControl {
	public:
		CMenu(CControl* o,SDL_Surface* t = 0);
		virtual void execute();
		CFont titleFont;
	protected:
		std::string caption;
		virtual void menu_init();
		virtual void menu_exit();
		virtual void paint(SDL_Surface* target);
		virtual void handleEvents();
		virtual void keyDown(SDL_keysym* sym);
};



class CPushButton : public CControl {
	public:
		CPushButton(CControl* o, int x, int y, std::string Caption, int fontsize = FONT_MENUBUTTONSIZE, SDL_Surface* t = 0);
		virtual ~CPushButton();
		void setOnClick(CEventCallback OnClick = 0, void* data = 0);
		CFont font;
		std::string getCaption();
		void setColor(SDL_Color* c);
	protected:
		SDL_Color* color;
		std::string caption;
		bool highlight;
		CEventCallback onClick;
		void* userdata;
		virtual void paint(SDL_Surface* target);
		virtual void mouseEnter();
		virtual void mouseLeave();
		virtual void mouseDown(int x, int y, int button);
};

class CTextEdit : public CControl {
	public:
		CTextEdit(CControl* o, int x, int y, int w, std::string Text, bool number=false, int fontsize = FONT_CONTROLSIZE, SDL_Surface* t = 0);
		std::string getText();
		void setText(std::string txt);
		CFont font;
	protected:
		std::string text;
		bool highlight;
		bool numbers;
		virtual void paint(SDL_Surface* target);
		virtual void mouseEnter();
		virtual void mouseLeave();
		virtual void keyDown(SDL_keysym* sym);
};

class CKeyEdit : public CControl {
	public:
		CKeyEdit(CControl* o, int x, int y, int w, SDLKey k, int fontsize = FONT_CONTROLSIZE, SDL_Surface* t = 0);
		SDLKey getKey();
		CFont font;
	protected:
		SDLKey key;
		bool highlight;
		virtual void paint(SDL_Surface* target);
		virtual void mouseEnter();
		virtual void mouseLeave();
		virtual void keyDown(SDL_keysym* sym);
		virtual void mouseDown(int x,int y, int button);
};

class CColorEdit : public CControl {
	public:
		CColorEdit(CControl* o, int x, int y, int w, SDL_Color col, int fontsize = FONT_MENUSIZE, SDL_Surface* t = 0);
		//virtual ~CColorEdit();
		void setOnChange(CEventCallback OnChange = 0, void* data = 0);
		SDL_Color getColor();
		CFont font;
	protected:
		CEventCallback onChange;
		void* userdata;
		SDL_Color color;
		virtual void paint(SDL_Surface* target);
		virtual void mouseDown(int x,int y, int button);
};

class CNumberEdit : public CControl {
	public:
		CNumberEdit(CControl* o, int x, int y, int w, int num, std::string Caption, int fontsize = FONT_MENUSIZE, SDL_Surface* t = 0);
		//virtual ~CNumberEdit();
		int getValue();
		CFont font;
		void setValue(int value);
	protected:
		bool highlight;
		std::string caption;
		int value;
		virtual void paint(SDL_Surface* target);
		virtual void mouseEnter();
		virtual void mouseLeave();
		virtual void mouseDown(int x, int y, int button);
};

class CImageList : public CControl {
	public:
		CImageList(CControl* o, int x, int y, int h, int num, std::string bitmap, SDL_Surface* t = 0);
		virtual ~CImageList();
		void setOnChange(CEventCallback OnChange = 0, void* data = 0);
		int getImage();
		SDL_Surface* img;
	protected:
		CEventCallback onChange;
		void* userdata;
		int image;
		int numImages;
		virtual void paint(SDL_Surface* target);
		virtual void mouseDown(int x, int y, int button);
};

class CCheckBox : public CControl {
	public:
		CCheckBox(CControl* o, int x, int y, std::string capt, bool chk, int fontsize = FONT_MENUSIZE, SDL_Surface* t = 0);
		virtual ~CCheckBox();
		void setOnChange(CEventCallback OnChange = 0, void* data = 0);
		bool isChecked();
		void setChecked(bool checked);

	protected:
		CFont font;
		SDL_Surface* img;
		CEventCallback onChange;
		void* userdata;
		bool checked;
		bool highlight;
		std::string caption;
		virtual void mouseEnter();
		virtual void mouseLeave();
		virtual void paint(SDL_Surface* target);
		virtual void mouseDown(int x, int y, int button);
};

typedef void (*ListClickCallback)(CControl*,int,std::string,void*);

class CList : public CControl {
	public:
		CList(CControl* o, int x, int y, int w, int fontsize = FONT_MENUSIZE, SDL_Surface* t = 0);
		virtual ~CList();
		void setOnElementClick(ListClickCallback clicked = 0, void* data = 0);
		CFont font;
		std::vector<std::string> getElements();
		void clearElements();
		void addElement(std::string element);
		void removeElement(std::string element);
	protected:
		std::vector<std::string> elements;
		int highlight;
		int objheight;
		int oldh;
		ListClickCallback onClick;
		void* userdata;
		virtual void paint(SDL_Surface* target);
		virtual void mouseLeave();
		virtual void mouseMove(int x, int y);
		virtual void mouseDown(int x, int y, int button);
};

class CTooltip : public CControl {
	public:
		CTooltip(CControl* o, int w, std::string Title, std::string Text, int title_fontsize = FONT_TOOLTIP_TITLESIZE, int text_fontsize = FONT_TOOLTIP_TEXTSIZE, SDL_Surface* t = 0);
		virtual ~CTooltip();
		void show(int x, int y);
		void hide();
		bool isVisible();
	protected:
		std::string text;
		std::string title;
		bool visible;
		SDL_Surface* buffer;
		SDL_Surface* back;
		CFont title_font;
		CFont text_font;
		virtual void paint(SDL_Surface* target);
		void real_paint(SDL_Surface* target);
		int calc_height();
};

class CTabControl : public CControl {
	public:
		CTabControl(CControl* o, int x, int y, int w, int h, int fontsize = FONT_TABSIZE, SDL_Surface* t = 0);
		virtual void pmouseDown(int x, int y, int button);
		virtual void pmouseUp(int x, int y, int button);
		virtual void pmouseMove(int x, int y);
		virtual void pkeyDown(SDL_keysym* sym);
		virtual void repaint();
		int getTabX();
		int getTabY();
		int getTabW();
		int getTabH();
		int getActiveTab();
		void setOnTabChanged(CEventCallback cb = 0, void* data = 0);
	protected:
		CEventCallback onTabChanged;
		void* userdata;
		CFont font;
		virtual void paint(SDL_Surface* target);
		virtual void mouseDown(int x, int y, int button);
		int activeTab;		
};

class CTabWidget : public CControl {
	public:
		CTabWidget(CTabControl* o, std::string Name, SDL_Surface* t = 0);
		std::string getName();
	protected:
		std::string name;
		virtual void paint(SDL_Surface* target);

	
};

class CTextArea : public CControl {
	public:
		CTextArea(CControl* o, int x, int y, int w, std::string text, int fontsize = FONT_CONTROL_TEXTSIZE, SDL_Surface* t = 0);
		void setText(std::string text);
	protected:
		std::string text;
		CFont font;
		virtual void paint(SDL_Surface* target);
};



typedef std::vector<std::string> TableRow;
typedef std::vector<int> ColWidths;

class CTable : public CControl {
	public:
		CTable(CControl* o, int x, int y, int w, int h, TableRow captions, int fontsize = FONT_CONTROL_TABLESIZE, SDL_Surface* t = 0);
		~CTable();
		TableRow* getCurrentRow();
		void addRow(const TableRow& row);
		void clearRows();
		void setOnChoice(CEventCallback cb = 0, void* data = 0);
		void setColWidths(ColWidths* w);
	protected:
		CEventCallback onChoice;
		void* userdata;
		int current;
		int colCount;
		int rowH;
		int titleH;
		CFont font;
		TableRow titles;
		std::vector<TableRow> rows;
		ColWidths* widths;
		virtual void paint(SDL_Surface* target);
		virtual void mouseDown(int x, int y, int button);
		virtual void mouseMove(int x, int y);
		virtual void mouseLeave();
};



class CPopup : public CMenu {
	public:
		CPopup(CControl* o, int w, int h, std::string title, int fontsize = FONT_POPUP_TITLESIZE, SDL_Surface* t = 0);
		//virtual void addChild(CControl* child); 
	protected:
		SDL_Surface* back;
		CFont title_font;
		virtual void paint(SDL_Surface* target);
		virtual void menu_init();
		virtual void menu_exit();
};


class CPopupProgress : public CPopup {
	public:
		CPopupProgress(CControl* o, std::string title, std::string text, int w = POPUPMESSAGE_STDWIDTH, int title_fontsize = FONT_POPUP_TITLESIZE, int text_fontsize = FONT_CONTROL_TEXTSIZE, SDL_Surface* t = 0);
		void show();
		void hide();
	protected:
		CTextArea* txt;
		//virtual void paint(SDL_Surface* target);
};

class CPopupMessage : public CPopup {
	public:
		CPopupMessage(CControl* o, std::string title, std::string text, int w = POPUPMESSAGE_STDWIDTH, int title_fontsize = FONT_POPUP_TITLESIZE, int text_fontsize = FONT_CONTROL_TEXTSIZE, SDL_Surface* t = 0);
	protected:
		CTextArea* txt;
		CPushButton* ok_button;
		//virtual void paint(SDL_Surface* target);
};

class CPopupDialog : public CPopup {
	public:
		CPopupDialog(CControl* o, std::string title, std::string text, int w = POPUPMESSAGE_STDWIDTH, int title_fontsize = FONT_POPUP_TITLESIZE, int text_fontsize = FONT_CONTROL_TEXTSIZE, SDL_Surface* t = 0);
		bool getValue();
		virtual void accept();
		virtual void reject();
	protected:
		bool value;
		CTextArea* txt;
		CPushButton* yes_button;
		CPushButton* no_button;
		//virtual void paint(SDL_Surface* target);
};

class CPopupInput : public CPopupDialog {
	public:
		CPopupInput(CControl* o, std::string title, std::string text, int w = POPUPMESSAGE_STDWIDTH, int title_fontsize = FONT_POPUP_TITLESIZE, int text_fontsize = FONT_CONTROL_TEXTSIZE, SDL_Surface* t = 0);
		std::string getInput();
		virtual void accept();
	protected:
		virtual void pkeyDown(SDL_keysym* sym);
		CTextEdit* edit;
};

#endif

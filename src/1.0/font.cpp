#include "font.h"

CFont::CFont(std::string fontName, int size)
{
	font = TTF_OpenFont((aux::data_path+fontName).c_str(),size);
}

CFont::~CFont()
{
	TTF_CloseFont(font);
}

void CFont::render_to(SDL_Surface* target, int x, int y, SDL_Color c, const std::string& text)
{
	SDL_Surface* surf = render(c,text);
	if(surf){
		SDL_Rect src, dest;
		src.x = src.y = 0;
		dest.x = x; dest.y = y;
		src.w = dest.w = surf->w;
		src.h = dest.h = surf->h;
		SDL_BlitSurface(surf,&src,target,&dest);
		SDL_FreeSurface(surf);
	}
}

void CFont::render_to(SDL_Surface* target, int x, int y, byte r, byte g, byte b, const std::string& text)
{
	SDL_Color col;
	col.r = r; col.g = g; col.b = b;
	render_to(target,x,y,col,text);
}

SDL_Surface* CFont::render(byte r, byte g, byte b, const std::string& text)
{
	SDL_Color col;
	col.r = r; col.g = g; col.b = b;
	return render(col,text);
}

SDL_Surface* CFont::render(SDL_Color c, const std::string& text)
{
	SDL_Surface* surf = TTF_RenderText_Blended(font,text.c_str(),c);
	//if(!surf)
	//	printf("error rendering font: %s\n",text.c_str());
	return surf;
}

int CFont::getHeight()
{
	return TTF_FontHeight(font);
}

int CFont::textWidth(std::string text)
{
	int w,h;
	TTF_SizeText(font,text.c_str(),&w,&h);
	return w;
}

string_list CFont::wordWrap(const std::string& text, int w)
{
	string_list lines;
	std::string txt = text.c_str();
	int line_w;
	std::string line = "";
	while(txt.length()>0) {
		std::string word = "";
		line_w = 0;
		while(txt.length()>0) {
			size_t pos = txt.find(" ");
			if(pos!=std::string::npos) {
				word = txt.substr(0,pos+1);
				txt = txt.substr(pos+1);
			} else {
				word = txt;
				txt = "";
			}
			//printf("word: %s\n",word.c_str());
			if(word == "\n ") {
				break;
			} else {
				int line_w_new = line_w + textWidth(word);
				if(line_w_new > w) {
					txt = word + txt;
					break;
				} else {
					line += word;
					line_w = line_w_new;
				}
			}
		}
		//printf("line: %s\n",line.c_str());
		//text_font.render_to(target,LeftSpace,cur_y,0,0,0,line);
		lines.push_back(std::string(line.c_str()));
		line = "";
	}
	return lines;
}
 


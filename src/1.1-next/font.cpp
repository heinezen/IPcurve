#include "font.h"



CFont::CFont(std::string fontName, int size, bool use_opengl)
{
	ttfFont = TTF_OpenFont((aux::data_path+fontName).c_str(),size);
	opengl = use_opengl;
	if(opengl) {
		initFont();
		printf("IPCurve's OpenGL font engine intialized!\n");
	}
}

CFont::~CFont()
{
	if(opengl)
		deinitFont();
	TTF_CloseFont(ttfFont);
}

void CFont::render_to(SDL_Surface* target, int x, int y, SDL_Color c, const std::string& text)
{
	SDL_Surface* surf = render(c,text);
	if(surf) {
		if(opengl) {
			//printf("rendering opengl font: %s\n");
			glDrawText((unsigned char*)text.c_str(),x,y,c.r/255.0,c.g/255.0,c.b/255.0);
		} else {
			SDL_Rect src, dest;
			src.x = src.y = 0;
			dest.x = x; dest.y = y;
			src.w = dest.w = surf->w;
			src.h = dest.h = surf->h;
			SDL_BlitSurface(surf,&src,target,&dest);
			SDL_FreeSurface(surf);
		}
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
	SDL_Surface* surf = TTF_RenderText_Blended(ttfFont,text.c_str(),c);
	//if(!surf)
	//	printf("error rendering font: %s\n",text.c_str());
	return surf;
}

int CFont::getHeight()
{
	return TTF_FontHeight(ttfFont);
}

int CFont::textWidth(std::string text)
{
	int w,h;
	TTF_SizeText(ttfFont,text.c_str(),&w,&h);
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
 
 

void CFont::loadChar(unsigned char c) 
{
	GLfloat texcoord[4];
	char letter[2] = {0, 0};
	if((c >= MIN_GLYPH) && (c <= MAX_GLYPH) && (glyphs[((int)c)].pic == 0)) {

		letter[0] = c;
		TTF_GlyphMetrics(ttfFont, (Uint16)c, &glyphs[((int)c)].minx, &glyphs[((int)c)].maxx, &glyphs[((int)c)].miny, &glyphs[((int)c)].maxy, &glyphs[((int)c)].advance);
		SDL_Color foreground;
		foreground.r = foreground.g = foreground.b = 255;
		SDL_Surface* g0 = TTF_RenderText_Blended(ttfFont, letter, foreground);

		SDL_Surface *g1 = 0;
		if(g0) {
			g1 = SDL_DisplayFormatAlpha(g0);
			SDL_FreeSurface(g0);
		}

		if(g1) {
			glyphs[((int)c)].pic = g1;
			glyphs[((int)c)].tex = aux::SDL_GL_LoadTexture(g1, texcoord);
			glyphs[((int)c)].texMinX = texcoord[0];
			glyphs[((int)c)].texMinY = texcoord[1];
			glyphs[((int)c)].texMaxX = texcoord[2];
			glyphs[((int)c)].texMaxY = texcoord[3];
		}
	}
}


void CFont::initFont()
{

	height = TTF_FontHeight(ttfFont);
	ascent = TTF_FontAscent(ttfFont);
	descent = TTF_FontDescent(ttfFont);
	lineSkip = TTF_FontLineSkip(ttfFont);

	for (int i = MIN_GLYPH; i <= MAX_GLYPH; i++) {
		glyphs[i].pic = 0;
		glyphs[i].tex = 0;
	}
}

void CFont::deinitFont()
{
	for (int i = MIN_GLYPH; i <= MAX_GLYPH; i++) {
		if(glyphs[i].pic)
			SDL_FreeSurface(glyphs[i].pic);
		if(glyphs[i].tex)
			glDeleteTextures(1,&glyphs[i].tex);
	}
}

/*  void Font::textSize(char *text, 
                SDL_Rect *r)
  {
    int maxx = 0;
    int advance = 0;
    int minx = 0;
		int w_largest = 0;
		char lastchar = 0;

    r->x = 0;
    r->y = 0;
    r->w = 0;
    r->h = height;

    while (0 != *text)
    {
      if ((MIN_GLYPH <= *text) && (*text <= MAX_GLYPH))
      {
				lastchar = *text;
				if (*text == '\n') {
					r->h += lineSkip;
    			r->w = r->w - advance + maxx;
					if (r->w > w_largest) w_largest = r->w;
					r->w = 0;
				} else {
        	loadChar(*text);

        	maxx = glyphs[((int)*text)].maxx;
        	advance = glyphs[((int)*text)].advance;
        	r->w += advance;
				}
      }

      text++;
    }
		if (lastchar != '\n') {
    	r->w = r->w - advance + maxx;
			if (r->w > w_largest) w_largest = r->w;
		} else {
			r->h -= lineSkip;
		}

		if (w_largest > r->w) r->w = w_largest;

  }*/

void CFont::glDrawText(unsigned char* text, int x, int y, float r, float g, float b)
{
	GLfloat left, right;
	GLfloat top, bottom;
	GLfloat texMinX, texMinY;
	GLfloat texMaxX, texMaxY;
	GLfloat minx;
	GLfloat baseleft = x;

	//glPushAttrib(GL_ALL_ATTRIB_BITS);


	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	while(*text) {
		if (*text == '\n') {
			x = baseleft;
			y += lineSkip;
		} else if (((*text) >= MIN_GLYPH) && ((*text) <= MAX_GLYPH)) {
			loadChar(*text);

			texMinX = glyphs[((int)*text)].texMinX;
			texMinY = glyphs[((int)*text)].texMinY;
			texMaxX = glyphs[((int)*text)].texMaxX;
			texMaxY = glyphs[((int)*text)].texMaxY;

			minx = glyphs[((int)*text)].minx;

			left   = x + minx;
			right  = x + glyphs[((int)*text)].pic->w + minx;
			top    = y;
			bottom = y + glyphs[((int)*text)].pic->h;

			glBindTexture(GL_TEXTURE_2D, glyphs[((int)*text)].tex);
			glColor3f(r,g,b);
			glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(texMinX, texMinY); glVertex2f( left,    top);
				glTexCoord2f(texMaxX, texMinY); glVertex2f(right,    top);
				glTexCoord2f(texMinX, texMaxY); glVertex2f( left, bottom);
				glTexCoord2f(texMaxX, texMaxY); glVertex2f(right, bottom);
			glEnd();

			x += glyphs[((int)*text)].advance;
		}

		text++;
	}
	//glPopAttrib();
}

unit SDL_ttf;

interface
uses
  SDL;

type
  PTTF_Font = ^TTTF_font;
  TTTF_Font = record
  end;

const
{*
   Set and retrieve the font style
   This font style is implemented by modifying the font glyphs, and
   doesn't reflect any inherent properties of the truetype font file.
*}
  TTF_STYLE_NORMAL	= $00;
  TTF_STYLE_BOLD       	= $01;
  TTF_STYLE_ITALIC	= $02;
  TTF_STYLE_UNDERLINE	= $04;

//returns 0 on succes, -1 if error occurs
function TTF_Init : integer; cdecl;
{$EXTERNALSYM TTF_Init}

// Open a font file and create a font of the specified point size
function TTF_OpenFont( const filename : Pchar; ptsize : integer ) : PTTF_Font; cdecl;
{$EXTERNALSYM TTF_OpenFont}
function TTF_OpenFontIndex( const filename : Pchar; ptsize : integer; index : Longint ): PTTF_Font; cdecl;
{$EXTERNALSYM TTF_OpenFontIndex}

function TTF_GetFontStyle( font : PTTF_Font) : integer; cdecl;
{$EXTERNALSYM TTF_GetFontStyle}
procedure TTF_SetFontStyle( font : PTTF_Font; style : integer ); cdecl;
{$EXTERNALSYM TTF_SetFontStyle}

{ Get the total height of the font - usually equal to point size }
function  TTF_FontHeight( font : PTTF_Font ) : Integer; cdecl;
{$EXTERNALSYM TTF_FontHeight}
{ Get the offset from the baseline to the top of the font
   This is a positive value, relative to the baseline.
}
function  TTF_FontAscent( font : PTTF_Font ) : Integer; cdecl;
{$EXTERNALSYM TTF_FontAscent}
{ Get the offset from the baseline to the bottom of the font
   This is a negative value, relative to the baseline.
}
function  TTF_FontDescent( font : PTTF_Font ) : Integer; cdecl;
{$EXTERNALSYM TTF_FontDescent}
{ Get the recommended spacing between lines of text for this font }
function  TTF_FontLineSkip( font : PTTF_Font ): Integer; cdecl;
{$EXTERNALSYM TTF_FontLineSkip}

{ Get the number of faces of the font }
function TTF_FontFaces( font : PTTF_Font ) : Longint; cdecl;
{$EXTERNALSYM TTF_FontFaces}

{ Get the font face attributes, if any }
function  TTF_FontFaceIsFixedWidth( font : PTTF_Font ): Integer; cdecl;
{$EXTERNALSYM TTF_FontFaceIsFixedWidth}
function  TTF_FontFaceFamilyName( font : PTTF_Font ): PChar; cdecl;
{$EXTERNALSYM TTF_FontFaceFamilyName}
function  TTF_FontFaceStyleName( font : PTTF_Font ): PChar; cdecl;
{$EXTERNALSYM TTF_FontFaceStyleName}

{ Get the metrics (dimensions) of a glyph }
function  TTF_GlyphMetrics( font : PTTF_Font; ch : Uint16;
				     var minx : integer; var maxx : integer;
                                     var miny : integer; var maxy : integer;
                                     var advance : integer ): Integer; cdecl;
{$EXTERNALSYM TTF_GlyphMetrics}

{ Get the dimensions of a rendered string of text }
function  TTF_SizeText( font : PTTF_Font; const text : PChar; var w : integer; var y : integer ): Integer; cdecl;
{$EXTERNALSYM TTF_SizeText}
function  TTF_SizeUTF8( font : PTTF_Font; const text : PChar; var w : integer; var y : integer): Integer; cdecl;
{$EXTERNALSYM TTF_SizeUTF8}
function  TTF_SizeUNICODE( font : PTTF_Font; const text : PUint16; var w : integer; var y : integer): Integer; cdecl;
{$EXTERNALSYM TTF_SizeUNICODE}

{ Create an 8-bit palettized surface and render the given text at
   fast quality with the given font and color.  The 0 pixel is the
   colorkey, giving a transparent background, and the 1 pixel is set
   to the text color.
   This function returns the new surface, or NULL if there was an error.
}
function  TTF_RenderText_Solid( font : PTTF_Font;
				const text : PChar; fg : TSDL_Color ): PSDL_Surface; cdecl;
{$EXTERNALSYM TTF_RenderText_Solid}
function  TTF_RenderUTF8_Solid( font : PTTF_Font;
				const text : PChar; fg : TSDL_Color ): PSDL_Surface; cdecl;
{$EXTERNALSYM TTF_RenderUTF8_Solid}
function  TTF_RenderUNICODE_Solid( font : PTTF_Font;
				const text :Uint16; fg : TSDL_Color ): PSDL_Surface; cdecl;
{$EXTERNALSYM TTF_RenderUNICODE_Solid}
{ 
Create an 8-bit palettized surface and render the given glyph at
   fast quality with the given font and color.  The 0 pixel is the
   colorkey, giving a transparent background, and the 1 pixel is set
   to the text color.  The glyph is rendered without any padding or
   centering in the X direction, and aligned normally in the Y direction.
   This function returns the new surface, or NULL if there was an error.
}
function  TTF_RenderGlyph_Solid( font : PTTF_Font;
					ch : Uint16; fg : TSDL_Color ): PSDL_Surface; cdecl;
{$EXTERNALSYM TTF_RenderGlyph_Solid}

{ Create an 8-bit palettized surface and render the given text at
   high quality with the given font and colors.  The 0 pixel is background,
   while other pixels have varying degrees of the foreground color.
   This function returns the new surface, or NULL if there was an error.
}
function  TTF_RenderText_Shaded( font : PTTF_Font;
				const text : PChar; fg : TSDL_Color; bg : TSDL_Color ): PSDL_Surface; cdecl;
{$EXTERNALSYM TTF_RenderText_Shaded}
function  TTF_RenderUTF8_Shaded( font : PTTF_Font;
				const text : PChar; fg : TSDL_Color; bg : TSDL_Color ): PSDL_Surface; cdecl;
{$EXTERNALSYM TTF_RenderUTF8_Shaded}
function  TTF_RenderUNICODE_Shaded( font : PTTF_Font;
				const text : PUint16; fg : TSDL_Color; bg : TSDL_Color ): PSDL_Surface; cdecl;
{$EXTERNALSYM TTF_RenderUNICODE_Shaded}

{ Create an 8-bit palettized surface and render the given glyph at
   high quality with the given font and colors.  The 0 pixel is background,
   while other pixels have varying degrees of the foreground color.
   The glyph is rendered without any padding or centering in the X
   direction, and aligned normally in the Y direction.
   This function returns the new surface, or NULL if there was an error.
}
function  TTF_RenderGlyph_Shaded( font : PTTF_Font; ch : Uint16; fg : TSDL_Color;
                                  bg : TSDL_Color ): PSDL_Surface; cdecl;
{$EXTERNALSYM TTF_RenderGlyph_Shaded}

{ Create a 32-bit ARGB surface and render the given text at high quality,
   using alpha blending to dither the font with the given color.
   This function returns the new surface, or NULL if there was an error.
}
function  TTF_RenderText_Blended( font : PTTF_Font;
				const text : PChar; fg : TSDL_Color ): PSDL_Surface; cdecl;
{$EXTERNALSYM TTF_RenderText_Blended}
function  TTF_RenderUTF8_Blended( font : PTTF_Font;
				const text : PChar; fg : TSDL_Color ): PSDL_Surface; cdecl;
{$EXTERNALSYM TTF_RenderUTF8_Blended}
function  TTF_RenderUNICODE_Blended( font : PTTF_Font;
				const text: PUint16; fg : TSDL_Color ): PSDL_Surface; cdecl;
{$EXTERNALSYM TTF_RenderUNICODE_Blended}

{ Create a 32-bit ARGB surface and render the given glyph at high quality,
   using alpha blending to dither the font with the given color.
   The glyph is rendered without any padding or centering in the X
   direction, and aligned normally in the Y direction.
   This function returns the new surface, or NULL if there was an error.
}
function  TTF_RenderGlyph_Blended( font : PTTF_Font; ch : Uint16; fg : TSDL_Color ): PSDL_Surface; cdecl;
{$EXTERNALSYM TTF_RenderGlyph_Blended}

{ For compatibility with previous versions, here are the old functions }
{#define TTF_RenderText(font, text, fg, bg)
	TTF_RenderText_Shaded(font, text, fg, bg)
#define TTF_RenderUTF8(font, text, fg, bg)	
	TTF_RenderUTF8_Shaded(font, text, fg, bg)
#define TTF_RenderUNICODE(font, text, fg, bg)	
	TTF_RenderUNICODE_Shaded(font, text, fg, bg)}

{ Close an opened font file }
procedure TTF_CloseFont( font : PTTF_Font ); cdecl;
{$EXTERNALSYM TTF_CloseFont}

//De-initialize TTF engine
procedure TTF_Quit; cdecl; 
{$EXTERNALSYM TTF_Quit}

implementation

const
  {$IFDEF WIN32}
  Modulename = 'SDL_ttf.dll';
  {$ENDIF}
  {$IFDEF LINUX}
  Modulename = 'libSDL_ttf.so';
  {$ENDIF}
  {$IFDEF MACOS}
  Modulename =  'libSDL_ttf.dylib';
  {$ENDIF}

function TTF_Init; external ModuleName;

function TTF_OpenFont; external ModuleName;

function TTF_OpenFontIndex; external ModuleName;

function TTF_GetFontStyle; external ModuleName;

procedure TTF_SetFontStyle; external ModuleName;

function  TTF_FontHeight; external ModuleName;
{ Get the offset from the baseline to the top of the font
   This is a positive value, relative to the baseline.
}
function  TTF_FontAscent; external ModuleName;
{ Get the offset from the baseline to the bottom of the font
   This is a negative value, relative to the baseline.
}
function  TTF_FontDescent; external ModuleName;
{ Get the recommended spacing between lines of text for this font }
function  TTF_FontLineSkip; external ModuleName;

{ Get the number of faces of the font }
function TTF_FontFaces; external ModuleName;

{ Get the font face attributes, if any }
function  TTF_FontFaceIsFixedWidth; external ModuleName;
function  TTF_FontFaceFamilyName; external ModuleName;
function  TTF_FontFaceStyleName; external ModuleName;

{ Get the metrics (dimensions) of a glyph }
function  TTF_GlyphMetrics; external ModuleName;

{ Get the dimensions of a rendered string of text }
function  TTF_SizeText; external ModuleName;
function  TTF_SizeUTF8; external ModuleName;
function  TTF_SizeUNICODE; external ModuleName;

{ Create an 8-bit palettized surface and render the given text at
   fast quality with the given font and color.  The 0 pixel is the
   colorkey, giving a transparent background, and the 1 pixel is set
   to the text color.
   This function returns the new surface, or NULL if there was an error.
}
function  TTF_RenderText_Solid; external ModuleName;
function  TTF_RenderUTF8_Solid; external ModuleName;
function  TTF_RenderUNICODE_Solid; external ModuleName;
{ 
Create an 8-bit palettized surface and render the given glyph at
   fast quality with the given font and color.  The 0 pixel is the
   colorkey, giving a transparent background, and the 1 pixel is set
   to the text color.  The glyph is rendered without any padding or
   centering in the X direction, and aligned normally in the Y direction.
   This function returns the new surface, or NULL if there was an error.
}
function  TTF_RenderGlyph_Solid; external ModuleName;

{ Create an 8-bit palettized surface and render the given text at
   high quality with the given font and colors.  The 0 pixel is background,
   while other pixels have varying degrees of the foreground color.
   This function returns the new surface, or NULL if there was an error.
}
function  TTF_RenderText_Shaded; external ModuleName;
function  TTF_RenderUTF8_Shaded; external ModuleName;
function  TTF_RenderUNICODE_Shaded; external ModuleName;

{ Create an 8-bit palettized surface and render the given glyph at
   high quality with the given font and colors.  The 0 pixel is background,
   while other pixels have varying degrees of the foreground color.
   The glyph is rendered without any padding or centering in the X
   direction, and aligned normally in the Y direction.
   This function returns the new surface, or NULL if there was an error.
}
function  TTF_RenderGlyph_Shaded; external ModuleName;

{ Create a 32-bit ARGB surface and render the given text at high quality,
   using alpha blending to dither the font with the given color.
   This function returns the new surface, or NULL if there was an error.
}
function  TTF_RenderText_Blended; external ModuleName;
function  TTF_RenderUTF8_Blended; external ModuleName;
function  TTF_RenderUNICODE_Blended; external ModuleName;

{ Create a 32-bit ARGB surface and render the given glyph at high quality,
   using alpha blending to dither the font with the given color.
   The glyph is rendered without any padding or centering in the X
   direction, and aligned normally in the Y direction.
   This function returns the new surface, or NULL if there was an error.
}
function  TTF_RenderGlyph_Blended; external ModuleName;

{ For compatibility with previous versions, here are the old functions }
{#define TTF_RenderText(font, text, fg, bg)
	TTF_RenderText_Shaded(font, text, fg, bg)
#define TTF_RenderUTF8(font, text, fg, bg)	
	TTF_RenderUTF8_Shaded(font, text, fg, bg)
#define TTF_RenderUNICODE(font, text, fg, bg)	
	TTF_RenderUNICODE_Shaded(font, text, fg, bg)}

{ Close an opened font file }
procedure TTF_CloseFont; external ModuleName;

procedure TTF_Quit; external ModuleName;

end.

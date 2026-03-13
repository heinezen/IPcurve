unit grafik;
interface
uses SDL,
     SDL_ttf,
     log,
     sysutils;
type NumberGfxArray = array['0'..'9'] of PSDL_Surface;
var
 TheScreen: PSDL_Surface;
procedure SetQuad(x,y,w,h,Color:Word;Surface:PSDL_Surface); //Quadrat Malen
function InitGraph:integer;
procedure ExitGraph;
function TextOut(txt:string;Target:PSDL_Surface;x,y,size:integer;typ:string;r,g,b:byte):boolean;
function NewSurfaceTextOut(txt:string;x,y,size:integer;typ:string;r,g,b:byte):PSDL_Surface;

function GetR(color:cardinal):byte;
function GetB(color:cardinal):byte;
function GetG(color:cardinal):byte;

function SDLColor(r,g,b:byte):TSDL_Color;
function SDLRect(x,y,w,h:integer):TSDL_Rect;
procedure CacheFonts;
procedure NumberOut(number:integer;target:PSDL_Surface;x,y:integer;Grafiken:NumberGfxArray);


implementation

procedure CacheFonts;
begin

end;

function InitGraph:integer;
begin
 Result:=TTF_Init;
end;

procedure ExitGraph;
begin
 TTF_Quit;
end;

procedure NumberOut(number:integer;target:PSDL_Surface;x,y:integer;Grafiken:NumberGfxArray);
var s:string;
    i:integer;
    nextx:integer;
    SrcRect,DestRect:TSDL_Rect;
begin
 s:=Inttostr(number);
 nextx:=x;
 for i:=1 to Length(s) do begin
  SrcRect:=SDLRect(0,0,Grafiken[s[i]].w,Grafiken[s[i]].h);
  DestRect:=SDLRect(nextx,y,Grafiken[s[i]].w,Grafiken[s[i]].h);
  SDL_BlitSurface(Grafiken[s[i]],@SrcRect,target,@DestRect);
  inc(nextx,Grafiken[s[i]].w);
 end;
end;


function TextOut(txt:string;Target:PSDL_Surface;x,y,size:integer;typ:string;r,g,b:byte):boolean;
var Text:PSDL_Surface;
    color:TSDL_Color;
    Font:PTTF_Font;
    Dest:TSDL_Rect;
begin
 Result:=true;
 color.r:=r;
 color.b:=b;
 color.g:=g;
 Font:=TTF_OpenFont(PChar(Typ),size);
 if font=nil then begin     // konnte font nicht öffnet
  Result:=false;
  LogOut('kurveSdl warning: '+SDL_GetError+', could not open font');
  Exit;
 end;
 Text:=TTF_RenderText_Solid(Font,PChar(txt),color);
 if text=nil then begin     // konnte font nicht rendern
  Result:=false;
  LogOut('kurveSdl warning: '+SDL_GetError+', could not render font');
  TTF_CloseFont(Font);
  Exit;
 end;
  Dest.x:=x;
  Dest.y:=y;
  Dest.w:=text.w;
  Dest.h:=text.h;
  if (SDL_BlitSurface(text,nil,Target,@Dest)<0) then begin //konte surface nicht kopieren
   Result:=false;
   LogOut('kurveSdl warning: '+SDL_GetError+', could not blit surface');
   TTF_CloseFont(Font);
   SDL_FreeSurface(text);
   Exit;
  end;
  TTF_CloseFont(font);
  SDL_FreeSurface(text);
end;

function NewSurfaceTextOut(txt:string;x,y,size:integer;typ:string;r,g,b:byte):PSDL_Surface;
var color:TSDL_Color;
    Font:PTTF_Font;
begin
 color.r:=r;
 color.b:=b;
 color.g:=g;
 Font:=TTF_OpenFont(PChar(Typ),size);
 if font=nil then begin     // konnte font nicht öffnet
  Result:=nil;
  LogOut('kurveSdl warning: '+SDL_GetError+', could not open font');
  Exit;
 end;
 Result:=TTF_RenderText_Solid(Font,PChar(txt),color);
 if Result=nil then begin     // konnte font nicht rendern
  Result:=nil;
  LogOut('kurveSdl warning: '+SDL_GetError+', could not render font');
  TTF_CloseFont(Font);
  Exit;
 end;
  TTF_CloseFont(font);
end;


procedure SetQuad(x,y,w,h,Color:Word;Surface:PSDL_Surface); //Quadrat Malen
var Rect:TSDL_Rect;
begin
 Rect.x:=x;
 Rect.y:=y;
 Rect.w:=w;
 Rect.h:=h;
 SDL_FillRect(Surface,@Rect,Color)
end;

function GetR(color:cardinal):byte;
begin
 Result:=((color and $F800)shr 11)*8;
end;

function GetG(color:cardinal):byte;
begin
 Result:=((color and $07E0)shr 6)*8;
end;

function GetB(color:cardinal):byte;
begin
 Result:=(color and $001F)*8;
end;

function SDLColor(r,g,b:byte):TSDL_Color;
begin
 Result.r:=r;
 Result.b:=b;
 Result.g:=g;
end;

function SDLRect(x,y,w,h:integer):TSDL_Rect;
begin
 Result.x:=x;
 Result.y:=y;
 Result.w:=w;
 Result.h:=h;
end;

end.

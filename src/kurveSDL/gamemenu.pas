unit gamemenu;
interface
 uses SDL,
      SDLUtils,
      menu,
      grafik,
      engine,
      constants,
      SysUtils;
type
 TMainMenuOwner = class(TSDLComponentOwner)
  public
   procedure OnCreate; override;
   procedure Paint(force:boolean=false); override;
 end;
 TSettingsMenuOwner = class(TSDLComponentOwner)
  public
   procedure OnCreate; override;
   procedure Paint(force:boolean=false); override;
 end;

{ TLanHostMenuOwner = class(TSDLComponentOwner)
  public
   procedure OnCreate; override;
   procedure Paint(force:boolean=false); override;
 end;

 TLanClientConnectMenuOwner = class(TSDLComponentOwner)
  public
   procedure OnCreate; override;
   procedure Paint(force:boolean=false); override;
 end;
 }
{ TLanClientWaitMenuOwner = class(TSDLComponentOwner)
  public
   procedure OnCreate; override;
   procedure Paint(force:boolean=false); override;
 end;
 }
 TCreditsMenuOwner = class(TSDLComponentOwner)
  public
   toPaint : PSDL_Surface;
   mx,my:integer;
   procedure OnCreate; override;
   procedure Paint(force:boolean=false); override;
 end;


var MainMenu:TMainMenuOwner;
    SettingsMenu:TSettingsMenuOwner;
//    LanHostMenu:TLanHostMenuOwner;
//    LanClientConnectMenu:TLanClientConnectMenuOwner;
//    LanClientWaitMenu:TLanClientWaitMenuOwner;
    CreditsMenu:TCreditsMenuOwner;
implementation
(* ------ Main Menu ------- *)

procedure MainMenuSettingsClick;
begin
 RunLevel:=42;
end;

procedure MainMenuCreditsClick;
begin
 RunLevel:=89;
end;

procedure MainMenuQuitClick;
begin
 Quit;
end;

{procedure MainMenuLanHostClick;
begin
 RunLevel:=-20;
end;

procedure MainMenuLanClientClick;
begin
 RunLevel:=-9;
end;
 }
procedure MainMenuLocalGameClick;
begin
 RunLevel:=10;
end;

procedure TMainMenuOwner.OnCreate;
begin
(* -------- Button "Lokales Spiel" ----------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,70,90,225,30,'Local'));
 with TSDLTextButton(FindComponent('Local')) do begin
  Text:=GetText('Lokales Spiel');
  FontColor:=SDLColor(200,0,0);
  OnClick:=MainMenuLocalGameClick;
 end;

(* -------- Button "Einstellungen" ----------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,70,120,238,30,'Settings'));
 with TSDLTextButton(FindComponent('Settings')) do begin
  Text:=GetText('Einstellungen');
  FontColor:=SDLColor(200,0,0);
  OnClick:=MainMenuSettingsClick;
 end;

(* ----------- Button "Credits" -------------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,70,210,238,30,'Credits'));
 with TSDLTextButton(FindComponent('Credits')) do begin
  Text:=GetText('Credits');
  FontColor:=SDLColor(200,0,0);
  OnClick:=MainMenuCreditsClick;
 end;


{ (* -------- Button "Lan Host" ----------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,70,150,315,30,'LanH'));
 with TSDLTextButton(FindComponent('LanH')) do begin
  Text:=GetText('LAN Spiel Hosten');
  FontColor:=SDLColor(200,0,0);
  OnClick:=MainMenuLanHostClick;
 end;

 (* -------- Button "Lan Client" ----------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,70,180,515,30,'LanC'));
 with TSDLTextButton(FindComponent('LanC')) do begin
  Text:=GetText('LAN Spiel Betreten');
  FontColor:=SDLColor(200,0,0);
  OnClick:=MainMenuLanClientClick;
 end;
 }

 (* -------- Button "Beenden" ----------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,70,600,238,40,'End'));
 with TSDLTextButton(FindComponent('End')) do begin
  Text:=GetText('Beenden');
  FontColor:=SDLColor(0,200,0);
  OnClick:=MainMenuQuitClick;
 end;
end;

procedure TMainMenuOwner.Paint;
var Event:TSDL_Event;
begin
 inherited Paint(force);
 while SDL_PollEvent(@Event)<>0 do begin
  case Event.type_ of
   SDL_QUITEV: begin // Programm soll beendet werden
                Quit;
               end;
   SDL_KEYDOWN: begin // Taste Gedrückt
                 if Event.key.keysym.sym=SDLK_ESCAPE then
                  Quit;
                end;  //keydown
  end; //case
 end; //while
end;

(* ------ Einstellungen ------- *)
procedure SettingsMenuAnimClick;
begin
 DoAnimText:=not DoAnimText;
 TSDLCheckBox(SettingsMenu.FindComponent('AnimTitel')).Checked:=DoAnimText;
end;

procedure SettingsMenuLochTClick;
begin
 Loecher:=not Loecher;
 TSDLCheckBox(SettingsMenu.FindComponent('LochT')).Checked:=Loecher;
end;

procedure SettingsMenuBackClick;
begin
 RunLevel:=0;
end;

procedure SettingsLangClick;
begin
 if lang=en then lang:=de
 else lang:=en;
 RunLevel:=0;
end;

procedure ColorRedClick1;
const x = 1;
begin
 Farben[x]:=SDL_MapRgb(TheScreen.format,Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Progress*255/100));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Paint;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).FontColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).AfterConstruction;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).Paint;
end;

procedure ColorRedClick2;
const x = 2;
begin
 Farben[x]:=SDL_MapRgb(TheScreen.format,Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Progress*255/100));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Paint;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).FontColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).AfterConstruction;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).Paint;
end;

procedure ColorRedClick3;
const x = 3;
begin
 Farben[x]:=SDL_MapRgb(TheScreen.format,Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Progress*255/100));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Paint;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).FontColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).AfterConstruction;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).Paint;
end;

procedure ColorRedClick4;
const x = 4;
begin
 Farben[x]:=SDL_MapRgb(TheScreen.format,Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Progress*255/100));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Paint;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).FontColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).AfterConstruction;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).Paint;
end;

procedure ColorRedClick5;
const x = 5;
begin
 Farben[x]:=SDL_MapRgb(TheScreen.format,Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Progress*255/100));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Paint;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).FontColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).AfterConstruction;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).Paint;
end;

procedure ColorRedClick6;
const x = 6;
begin
 Farben[x]:=SDL_MapRgb(TheScreen.format,Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Progress*255/100));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Paint;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).FontColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).AfterConstruction;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).Paint;
end;

procedure ColorRedClick7;
const x = 7;
begin
 Farben[x]:=SDL_MapRgb(TheScreen.format,Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Progress*255/100),Round(TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Progress*255/100));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Rc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Gc')).Paint;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).AfterConstruction;
 TSDLValueSetting(SettingsMenu.FindComponent(Inttostr(x)+'Bc')).Paint;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).FontColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).AfterConstruction;
 TSDLTextButton(SettingsMenu.FindComponent(Inttostr(x)+'Label')).Paint;
end;


function ColorRedClick(x:integer):TProc;
begin
 case x of
  1:Result:=ColorRedClick1;
  2:Result:=ColorRedClick2;
  3:Result:=ColorRedClick3;
  4:Result:=ColorRedClick4;
  5:Result:=ColorRedClick5;
  6:Result:=ColorRedClick6;
  7:Result:=ColorRedClick7;
  else Result:=nil;
 end;
end;

procedure SettingsMenuGameSpdClick;
begin
 GameSpeed:=Round((100 - TSDLValueSetting(SettingsMenu.FindComponent('GameSpd')).Progress)/3);
end;

procedure SettingsMenuHoleSizeClick;
begin
 LochSize:=Round((TSDLValueSetting(SettingsMenu.FindComponent('LochS')).Progress)/10)+1;
end;

procedure SettingsMenuHoleAbstMaxClick;
begin
 LochAbstandMin:=Round((TSDLValueSetting(SettingsMenu.FindComponent('LochAbstMax')).Progress)*10)+100;
 LochAbstandMax:=Round((TSDLValueSetting(SettingsMenu.FindComponent('LochAbstMin')).Progress)*2)+LochAbstandMin;
end;

procedure SettingsMenuHoleAbstMinClick;
begin
 LochAbstandMax:=Round((TSDLValueSetting(SettingsMenu.FindComponent('LochAbstMin')).Progress)*2)+LochAbstandMin;
end;

procedure SettingsMenuBlinkSpdClick;
begin
 BlinkZeit:=TSDLValueSetting(SettingsMenu.FindComponent('FlashSpd')).Progress;
end;


procedure TSettingsMenuOwner.OnCreate;
var x:integer;
begin
 (* -------- Label "Links" ----------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,600,160,110,25,'LLabel',FALSE));
 with TSDLTextButton(FindComponent('LLabel')) do begin
  Text:=GetText('Links');
  FontColor:=SDLColor(0,0,255);
  OnClick:=nil;
 end;

 (* -------- Label "Rechts" ----------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,740,160,110,25,'RLabel',FALSE));
 with TSDLTextButton(FindComponent('RLabel')) do begin
  Text:=GetText('Rechts');
  FontColor:=SDLColor(0,0,255);
  OnClick:=nil;
 end;

 (* -------- Label "Special" ----------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,880,160,110,25,'SLabel',FALSE));
 with TSDLTextButton(FindComponent('SLabel')) do begin
  Text:=GetText('Special');
  FontColor:=SDLColor(0,0,255);
  OnClick:=nil;
 end;


 for x:=1 to MaxSpieler do begin
 (* -------- Label "Spielernummer" ----------- *)
  AddComponent(TSDLTextButton.Create(TheScreen,490,120+x*80,50,60,Inttostr(x)+'Label',FALSE));
  with TSDLTextButton(FindComponent(Inttostr(x)+'Label')) do begin
   Text:=Inttostr(x);
   FontColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
   OnClick:=nil;
  end;

 (* -------- Spezialeigenschaften ----------- *)
  AddComponent(TSDLImageButton.Create(TheScreen,540,120+x*80,50,50,Inttostr(x)+'Spez',FALSE));
  with TSDLImageButton(FindComponent(Inttostr(x)+'Spez')) do begin
   Image:=SDL_LoadBMP('./special.dat');
   DispImage:=Ord(Specials[x]);
   OnClick:=nil;
  end;
  if MausSteuerung=x then begin
  (* -------- MausLeft ----------- *)
   AddComponent(TSDLTextButton.Create(TheScreen,600,120+x*80,130,25,Inttostr(x)+'L',FALSE));
   with TSDLTextButton(FindComponent(Inttostr(x)+'L')) do begin
    Text:=GetText('LMaus');
    FontColor:=SDLColor($FF,$FF,$FF);
    OnClick:=nil;
   end;
  (* -------- MausRight ----------- *)
   AddComponent(TSDLTextButton.Create(TheScreen,740,120+x*80,130,25,Inttostr(x)+'R',FALSE));
   with TSDLTextButton(FindComponent(Inttostr(x)+'R')) do begin
    Text:=GetText('RMaus');
    FontColor:=SDLColor($FF,$FF,$FF);
    OnClick:=nil;
   end;
  (* -------- MausSpecial ----------- *)
   AddComponent(TSDLTextButton.Create(TheScreen,880,120+x*80,130,25,Inttostr(x)+'S',FALSE));
   with TSDLTextButton(FindComponent(Inttostr(x)+'S')) do begin
    Text:=GetText('MidMaus');
    FontColor:=SDLColor($FF,$FF,$FF);
    OnClick:=nil;
   end;
  end
  else begin
   (* -------- KeyField xL ----------- *)
   AddComponent(TSDLKeyEdit.Create(TheScreen,600,120+x*80,130,25,IntToStr(x)+'L'));
   with TSDLKeyEdit(FindComponent(IntToStr(x)+'L')) do begin
    FontColor:=SDLColor(200,0,0);
    Text:=SDL_GetKeyName(Tasten[x,1]);
    KeyVal:=Tasten[x,1];
   end;
   (* -------- KeyField xR ----------- *)
   AddComponent(TSDLKeyEdit.Create(TheScreen,740,120+x*80,130,25,IntToStr(x)+'R'));
   with TSDLKeyEdit(FindComponent(IntToStr(x)+'R')) do begin
    FontColor:=SDLColor(200,0,0);
    Text:=SDL_GetKeyName(Tasten[x,2]);
    KeyVal:=Tasten[x,2];
   end;
   (* -------- KeyField xS ----------- *)
   AddComponent(TSDLKeyEdit.Create(TheScreen,880,120+x*80,130,25,IntToStr(x)+'S'));
   with TSDLKeyEdit(FindComponent(IntToStr(x)+'S')) do begin
    FontColor:=SDLColor(200,0,0);
    Text:=SDL_GetKeyName(Tasten[x,3]);
    KeyVal:=Tasten[x,3];
   end;
  end;
  (* -------- RedColor-Regler ----------- *)
   AddComponent(TSDLValueSetting.Create(TheScreen,600,150+x*80,130,25,IntToStr(x)+'Rc',FALSE));
   with TSDLValueSetting(FindComponent(IntToStr(x)+'Rc')) do begin
    Progress :=round((GetR(Farben[x])/255)*100);
    FontColor:=SDLColor(0,0,0);
    FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
    Text:='Red';
    OnClick:=ColorRedClick(x);
   end;
  (* -------- GreenColor-Regler ----------- *)
   AddComponent(TSDLValueSetting.Create(TheScreen,740,150+x*80,130,25,IntToStr(x)+'Gc',FALSE));
   with TSDLValueSetting(FindComponent(IntToStr(x)+'Gc')) do begin
    Progress :=round((GetG(Farben[x])/255)*100);
    FontColor:=SDLColor(0,0,0);
    FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
    Text:='Green';
    OnClick:=ColorRedClick(x);
   end;
  (* -------- BlueColor-Regler ----------- *)
   AddComponent(TSDLValueSetting.Create(TheScreen,880,150+x*80,130,25,IntToStr(x)+'Bc',FALSE));
   with TSDLValueSetting(FindComponent(IntToStr(x)+'Bc')) do begin
    Progress :=round((GetB(Farben[x])/255)*100);
    FontColor:=SDLColor(0,0,0);
    FillColor:=SDLColor(GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
    Text:='Blue';
    OnClick:=ColorRedClick(x);
   end;
 end;
 (* -------- CheckBox "Animierte Titel" ----------- *)
 AddComponent(TSDLCheckBox.Create(TheScreen,70,120,300,30,'AnimTitel'));
 with TSDLCheckBox(FindComponent('AnimTitel')) do begin
  FontColor:=SDLColor(200,0,0);
  Text:=GetText('Animierte Titel');
  Checked:=DoAnimText;
  OnClick:=SettingsMenuAnimClick;
 end;

 (* -------- CheckBox "Lücken" ----------- *)
 AddComponent(TSDLCheckBox.Create(TheScreen,70,170,150,30,'LochT'));
 with TSDLCheckBox(FindComponent('LochT')) do begin
  FontColor:=SDLColor(200,0,0);
  Text:='Holes';
  Checked:=Loecher;
  OnClick:=SettingsMenuLochTClick;
 end;

(* -------- LochSize-Regler ----------- *)
 AddComponent(TSDLValueSetting.Create(TheScreen,70,210,400,30,'LochS'));
 with TSDLValueSetting(FindComponent('LochS')) do begin
  Progress := ((LochSize-1)*10);
  FontColor:=SDLColor(200,0,0);
  FillColor:=SDLColor(0,200,0);
  Text:='HoleSize';
  OnClick:=SettingsMenuHoleSizeClick;
 end;

(* -------- LochAbstandMin-Regler ----------- *)
 AddComponent(TSDLValueSetting.Create(TheScreen,70,250,400,30,'LochAbstMax'));
 with TSDLValueSetting(FindComponent('LochAbstMax')) do begin
  Progress := Round((LochAbstandMax-100)/10);
  FontColor:=SDLColor(200,0,0);
  FillColor:=SDLColor(0,200,0);
  Text:=GetText('HoleAbstand Min');
  OnClick:=SettingsMenuHoleAbstMaxClick;
 end;

 (* -------- LochAbstandToleranz-Regler ----------- *)
 AddComponent(TSDLValueSetting.Create(TheScreen,70,290,400,30,'LochAbstMin'));
 with TSDLValueSetting(FindComponent('LochAbstMin')) do begin
  Progress := Round((LochAbstandMax-LochAbstandMin)/2);
  FontColor:=SDLColor(200,0,0);
  FillColor:=SDLColor(0,200,0);
  Text:=GetText('HoleAbstand Toleranz');
  OnClick:=SettingsMenuHoleAbstMinClick;
 end;

 (* -------- GameSpeed-Regler ----------- *)
 AddComponent(TSDLValueSetting.Create(TheScreen,70,340,400,30,'GameSpd'));
 with TSDLValueSetting(FindComponent('GameSpd')) do begin
  Progress := 100 - (GameSpeed*3);
  FontColor:=SDLColor(200,0,0);
  FillColor:=SDLColor(0,200,0);
  Text:=GetText('SpielSpeed');
  OnClick:=SettingsMenuGameSpdClick;
 end;

 (* -------- BlinkZeit-Regler ----------- *)
 AddComponent(TSDLValueSetting.Create(TheScreen,70,380,400,30,'FlashSpd'));
 with TSDLValueSetting(FindComponent('FlashSpd')) do begin
  Progress :=BlinkZeit;
  FontColor:=SDLColor(200,0,0);
  FillColor:=SDLColor(0,200,0);
  Text:=GetText('BlinkZeit');
  OnClick:=SettingsMenuBlinkSpdClick;
 end;

 (* -------- Button "Sprache" ----------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,70,560,400,40,'lang'));
 with TSDLTextButton(FindComponent('lang')) do begin
  Text:=GetText('English Please');
  FontColor:=SDLColor(200,200,0);
  OnClick:=SettingsLangClick
 end;

(* -------- Button "Zurück" ----------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,70,600,150,40,'GoBack'));
 with TSDLTextButton(FindComponent('GoBack')) do begin
  Text:=GetText('Fertig');
  FontColor:=SDLColor(0,0,200);
  OnClick:=SettingsMenuBackClick
 end;
end;

procedure TSettingsMenuOwner.Paint;
var Event:TSDL_Event;
begin
 inherited Paint(force);
 while SDL_PollEvent(@Event)<>0 do begin
  case Event.type_ of
   SDL_QUITEV: begin // Programm soll beendet werden
                Quit;
               end;
   SDL_KEYDOWN: begin // Taste Gedrückt
                 if Event.key.keysym.sym=SDLK_ESCAPE then
                  RunLevel:=0;
                end;  //keydown
  end; //case
 end; //while
end;


{(* ------------------- LAN Host --------------------*)

procedure HostOvertake1;
begin
end;

procedure HostOvertake2;
begin
end;

procedure HostOvertake3;
begin
end;

procedure HostOvertake4;
begin
end;

procedure HostOvertake5;
begin
end;

procedure HostOvertake6;
begin
end;

procedure HostOvertake7;
begin
end;

procedure HostStartGameClick;
begin
// IAmTheMaster:=true;
// Runlevel:=-1;
end;

procedure TLanHostMenuOwner.OnCreate;
begin
(* -------- Buttons "Overtake" ----------- *)
(* AddComponent(TSDLTextButton.Create(TheScreen,860,300,250,20,'OT1'));
 with TSDLTextButton(FindComponent('OT1')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[1]),GetG(Farben[1]),GetB(Farben[1]));
  OnClick:=HostOvertake1;
 end;
 AddComponent(TSDLTextButton.Create(TheScreen,860,330,250,20,'OT2'));
 with TSDLTextButton(FindComponent('OT2')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[2]),GetG(Farben[2]),GetB(Farben[2]));
  OnClick:=HostOvertake2;
 end;
 AddComponent(TSDLTextButton.Create(TheScreen,860,360,250,20,'OT3'));
 with TSDLTextButton(FindComponent('OT3')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[3]),GetG(Farben[3]),GetB(Farben[3]));
  OnClick:=HostOvertake3;
 end;
 AddComponent(TSDLTextButton.Create(TheScreen,860,390,250,20,'OT4'));
 with TSDLTextButton(FindComponent('OT4')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[4]),GetG(Farben[4]),GetB(Farben[4]));
  OnClick:=HostOvertake4;
 end;
 AddComponent(TSDLTextButton.Create(TheScreen,860,420,250,20,'OT5'));
 with TSDLTextButton(FindComponent('OT5')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[5]),GetG(Farben[5]),GetB(Farben[5]));
  OnClick:=HostOvertake5;
 end;
 AddComponent(TSDLTextButton.Create(TheScreen,860,450,250,20,'OT6'));
 with TSDLTextButton(FindComponent('OT6')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[6]),GetG(Farben[6]),GetB(Farben[6]));
  OnClick:=HostOvertake6;
 end;
 AddComponent(TSDLTextButton.Create(TheScreen,860,480,250,20,'OT7'));
 with TSDLTextButton(FindComponent('OT7')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[7]),GetG(Farben[7]),GetB(Farben[7]));
  OnClick:=HostOvertake7;
 end;
  *)

(* -------- Button "Spiel Starten" ----------- *)
(* AddComponent(TSDLTextButton.Create(TheScreen,500,600,250,40,'StartIt'));
 with TSDLTextButton(FindComponent('StartIt')) do begin
  Text:=GetText('Spiel Starten');
  FontColor:=SDLColor(0,0,200);
  OnClick:=HostStartGameClick
 end;
  *)

(* -------- Button "Zurück" ----------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,70,600,250,40,'GoBack'));
 with TSDLTextButton(FindComponent('GoBack')) do begin
  Text:=GetText('Abbrechen');
  FontColor:=SDLColor(0,0,200);
  OnClick:=SettingsMenuBackClick
 end;

end;

procedure TLanHostMenuOwner.Paint(force:boolean=false);
var Event:TSDL_Event;
begin
 inherited Paint(force);
 while SDL_PollEvent(@Event)<>0 do begin
  case Event.type_ of
   SDL_QUITEV: begin // Programm soll beendet werden
                Quit;
               end;
   SDL_KEYDOWN: begin // Taste Gedrückt
                 if Event.key.keysym.sym=SDLK_ESCAPE then
                  RunLevel:=0;
                end;  //keydown
  end; //case
 end; //while
end;


(* ------ LAN Client Connect ------- *)

procedure LanClientConnectClick;
var TheIP:string;
begin
// MyName:=TSDLEdit(LanClientConnectMenu.FindComponent('NetName')).Text;
// TheIP:=TSDLEdit(LanClientConnectMenu.FindComponent('NetIP')).Text;
// if MyName='' then Exit;
// if TheIP='' then Exit;
// Connect(TheIP);
// if not Connected then Exit;
// RunLevel:=-10;
end;

procedure TLanClientConnectMenuOwner.OnCreate;
begin
(* -------- Editfeld "IP" ----------- *)
// AddComponent(TSDLEdit.Create(TheScreen,200,100,450,40,'NetIP'));
// with TSDLEdit(FindComponent('NetIP')) do begin
//  Text:='0.0.0.0';
//  FontColor:=SDLColor(200,200,0);
// end;

 (* -------- Editfeld "Name" ----------- *)
// AddComponent(TSDLEdit.Create(TheScreen,200,200,450,40,'NetName'));
// with TSDLEdit(FindComponent('NetName')) do begin
//  Text:=MyName;
//  FontColor:=SDLColor(200,200,0);
// end;

(* -------- Button "Connect" ----------- *)
// AddComponent(TSDLTextButton.Create(TheScreen,500,600,250,40,'Con'));
// with TSDLTextButton(FindComponent('Con')) do begin
//  Text:='Connect';
//  FontColor:=SDLColor(0,0,200);
//  OnClick:=LanClientConnectClick;
// end;

(* -------- Button "Zurück" ----------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,70,600,250,40,'GoBack'));
 with TSDLTextButton(FindComponent('GoBack')) do begin
  Text:=GetText('Abbrechen');
  FontColor:=SDLColor(0,0,200);
  OnClick:=SettingsMenuBackClick
 end;
end;

procedure TLanClientConnectMenuOwner.Paint;
var Event:TSDL_Event;
begin
 inherited Paint(force);
 while SDL_PollEvent(@Event)<>0 do begin
  case Event.type_ of
   SDL_QUITEV: begin // Programm soll beendet werden
                Quit;
               end;
   SDL_KEYDOWN: begin // Taste Gedrückt
                 if Event.key.keysym.sym=SDLK_ESCAPE then
                  RunLevel:=0;
                end;  //keydown
  end; //case
 end; //while
end;

(* ------------------- LAN ClientWait --------------------*)

procedure ClientOvertake1;
begin
end;

procedure ClientOvertake2;
begin
end;

procedure ClientOvertake3;
begin
end;

procedure ClientOvertake4;
begin
end;

procedure ClientOvertake5;
begin
end;

procedure ClientOvertake6;
begin
end;

procedure ClientOvertake7;
begin
end;

procedure TLanClientWaitMenuOwner.OnCreate;
begin
(* -------- Buttons "Overtake" ----------- *)
(* AddComponent(TSDLTextButton.Create(TheScreen,860,300,250,20,'OT1'));
 with TSDLTextButton(FindComponent('OT1')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[1]),GetG(Farben[1]),GetB(Farben[1]));
  OnClick:=ClientOvertake1;
 end;
 AddComponent(TSDLTextButton.Create(TheScreen,860,330,250,20,'OT2'));
 with TSDLTextButton(FindComponent('OT2')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[2]),GetG(Farben[2]),GetB(Farben[2]));
  OnClick:=ClientOvertake2;
 end;
 AddComponent(TSDLTextButton.Create(TheScreen,860,360,250,20,'OT3'));
 with TSDLTextButton(FindComponent('OT3')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[3]),GetG(Farben[3]),GetB(Farben[3]));
  OnClick:=ClientOvertake3;
 end;
 AddComponent(TSDLTextButton.Create(TheScreen,860,390,250,20,'OT4'));
 with TSDLTextButton(FindComponent('OT4')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[4]),GetG(Farben[4]),GetB(Farben[4]));
  OnClick:=ClientOvertake4;
 end;
 AddComponent(TSDLTextButton.Create(TheScreen,860,420,250,20,'OT5'));
 with TSDLTextButton(FindComponent('OT5')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[5]),GetG(Farben[5]),GetB(Farben[5]));
  OnClick:=ClientOvertake5;
 end;
 AddComponent(TSDLTextButton.Create(TheScreen,860,450,250,20,'OT6'));
 with TSDLTextButton(FindComponent('OT6')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[6]),GetG(Farben[6]),GetB(Farben[6]));
  OnClick:=ClientOvertake6;
 end;
 AddComponent(TSDLTextButton.Create(TheScreen,860,480,250,20,'OT7'));
 with TSDLTextButton(FindComponent('OT7')) do begin
  Text:=GetText('Kontrollieren');
  FontColor:=SDLColor(GetR(Farben[7]),GetG(Farben[7]),GetB(Farben[7]));
  OnClick:=ClientOvertake7;
 end;


 *)

(* -------- Button "Zurück" ----------- *)
 AddComponent(TSDLTextButton.Create(TheScreen,70,600,250,40,'GoBack'));
 with TSDLTextButton(FindComponent('GoBack')) do begin
  Text:=GetText('Abbrechen');
  FontColor:=SDLColor(0,0,200);
//  OnClick:=net.DisConnect;
 end;

end;

procedure TLanClientWaitMenuOwner.Paint(force:boolean=false);
var Event:TSDL_Event;
begin
 inherited Paint(force);
 while SDL_PollEvent(@Event)<>0 do begin
  case Event.type_ of
   SDL_QUITEV: begin // Programm soll beendet werden
                Quit;
               end;
   SDL_KEYDOWN: begin // Taste Gedrückt
                 if Event.key.keysym.sym=SDLK_ESCAPE then
                  RunLevel:=0;
                end;  //keydown
  end; //case
 end; //while
end;

             }

procedure TCreditsMenuOwner.OnCreate;
begin
toPaint := SDL_CreateRGBSurface(SDL_SWSURFACE,MapWidth,MapHeight,16,$00,$00,$00,$00);
//SDL_GradientFillRect(toPaint,nil,SDLColor(0,0,50),SDLColor(0,0,200),gsHorizontal);
TextOut('          kurveSDL',toPaint,10,50,80,GameFont,0,0,255);
TextOut('                           version 0.5',toPaint,10,140,40,GameFont,0,0,255);
TextOut('                 Programming',toPaint,10,210,50,GameFont,255,0,0);
TextOut('                               Michi Hostettler',toPaint,10,270,30,GameFont,255,127,0);
TextOut('                     Testing',toPaint,10,330,50,GameFont,255,0,0);
TextOut('                                   Class M07a',toPaint,10,390,30,GameFont,255,127,0);
TextOut('                                   Hostettlers',toPaint,10,430,30,GameFont,255,127,0);
TextOut('                   Thanks to',toPaint,10,480,50,GameFont,255,0,0);
TextOut('                                Urs Hostettler',toPaint,10,540,30,GameFont,255,127,0);
TextOut('                                       for the AI players',toPaint,10,575,25,GameFont,255,127,0);
TextOut('                                The SDL Team',toPaint,10,610,30,GameFont,255,127,0);
TextOut('                                for the great graphics library',toPaint,10,645,25,GameFont,255,127,0);
TextOut('                             kurveSDL is free software, and might be copied and modified',toPaint,10,740,20,GameFont,0,0,255);
mx := 0;
my := MapHeight-50;
end;

procedure TCreditsMenuOwner.Paint;
var Event:TSDL_Event;
    r21,r22,r1:TSDL_Rect;
begin
 SDL_FillRect(TheScreen,nil,0);
 dec(my,2);
 if my<=(-MapHeight) then my:=mapHeight-50;
 r21 := SDLRect(mx,my,mapWidth,mapHeight);
// r22 := SDLRect(mx,my-mapHeight,mapWidth,mapHeight);
 r1 := SDLRect(0,0,mapWidth,mapHeight);
 SDL_BlitSurface(toPaint,@r1,theScreen,@r21);
// SDL_BlitSurface(toPaint,@r1,theScreen,@r22);


 while SDL_PollEvent(@Event)<>0 do begin
  case Event.type_ of
   SDL_QUITEV: begin // Programm soll beendet werden
                Quit;
               end;
   SDL_KEYDOWN: begin // Taste Gedrückt
                  RunLevel:=0;
                end;  //keydown
   SDL_MOUSEBUTTONDOWN: begin // Taste Gedrückt
                  RunLevel:=0;
                end;  //keydown
  end; //case
 end; //while
end;


end.

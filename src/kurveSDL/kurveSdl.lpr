program kurveSdl;
uses
  SDL,
  SysUtils,
  schlangen in 'schlangen.pas',
  grafik in 'grafik.pas',
  constants in 'constants.pas',
  maps in 'maps.pas',
  menu in 'menu.pas',
  log in 'log.pas',
  gamemenu in 'gamemenu.pas',
  engine in 'engine.pas',
  ki in 'ki.pas';
  
var Ticks:integer;
    changeColor:cardinal;
    i,x:integer;
    Step:integer;


procedure AnimText(Text:string);
begin
 if DoAnimText then begin
        TextOut(GetText(Text),TheScreen,10,10,50,GameFont,round((sin((changeColor/360)*2*Pi)+1)*127),round((cos((changeColor/360)*2*Pi)+1)*127),round((cos(((changeColor+45)/360)*2*Pi)+1)*127));
        inc(changeColor,4);
        if changeColor>=360 then changeColor:=changeColor mod 360;
 end
 else begin
  TextOut(GetText(Text),TheScreen,10,10,50,GameFont,255,255,200);
 end;
end;

begin
 if SDL_INIT(SDL_INIT_TIMER or SDL_INIT_VIDEO)<0 then Quit(true); //SDL Starten
 if InitGraph<0 then Quit(true); //SDL_ttf u.a. Starten
 if FullScreen then TheScreen:= SDL_SetVideoMode(MapWidth,MapHeight,16,SDL_HWSURFACE or SDL_FULLSCREEN or SDL_HWACCEL or SDL_HWPALETTE) // Surface Erstellen
 else TheScreen:= SDL_SetVideoMode(MapWidth,MapHeight,16,SDL_HWSURFACE or SDL_HWACCEL or SDL_HWPALETTE);
 if TheScreen=nil then Quit(true); // Surface Ok?
 RunLevel:=0;
 Step:=0;
 Randomize;
 LoadSettings;
 InitGame;
 SpielerCount:=0; // keine spieler an anfang
 changeColor:=0; // Color-Animation im Titelbild
 if CatchMouse then SDL_ShowCursor(0); // maus versrecken
 repeat // Main-Loop
  case RunLevel of
   (* ------------------- TITELBILD ---------------*)
   0: begin // Titelbild
       if MainMenu=nil then begin
        SDL_FillRect(TheScreen,nil,0);
        MainMenu:=TMainMenuOwner.Create(CursorBMP,TheScreen,MouseX,MouseY);
       end;
       AnimText(TitelText);
       MainMenu.Paint;
       if RunLevel<>0 then begin
        MainMenu.Free;
        MainMenu:=nil;
        SDL_FillRect(TheScreen,nil,0);
       end;
       SDL_Flip(TheScreen);
      end;
   (* ------------------- CREDITS ---------------*)
   89: begin
       if CreditsMenu=nil then begin
        SDL_FillRect(TheScreen,nil,0);
        CreditsMenu:=TCreditsMenuOwner.Create(CursorBMP,TheScreen,MouseX,MouseY);
       end;
       Ticks:=SDL_GetTicks;
       CreditsMenu.Paint;
       if RunLevel<>89 then begin
        CreditsMenu.Free;
        CreditsMenu:=nil;
        SDL_FillRect(TheScreen,nil,0);
       end;
       SDL_Flip(TheScreen);
       dec(Ticks,SDL_GetTicks);
       if (40+ticks)>0 then SDL_Delay(40+Ticks);
      end;
   (* ------------------- EINSTELLUNGEN ---------------*)
   42: begin
       if SettingsMenu=nil then begin
        SDL_FillRect(TheScreen,nil,0);
        SettingsMenu:=TSettingsMenuOwner.Create(CursorBMP,TheScreen,MouseX,MouseY);
       end;
       AnimText(SettingsText);
       SettingsMenu.Paint;
       if RunLevel<>42 then begin
        for x:=1 to MaxSpieler do begin
         if MausSteuerung<>x then begin
          Tasten[x,1]:=TSDLKeyEdit(SettingsMenu.FindComponent(IntToStr(x)+'L')).KeyVal;
          Tasten[x,2]:=TSDLKeyEdit(SettingsMenu.FindComponent(IntToStr(x)+'R')).KeyVal;
          Tasten[x,3]:=TSDLKeyEdit(SettingsMenu.FindComponent(IntToStr(x)+'S')).KeyVal;
          Specials[x]:=TSpecial(TSDLImageButton(SettingsMenu.FindComponent(IntToStr(x)+'Spez')).DispImage);
         end
         else begin
          Tasten[x,1]:=$00;
          Tasten[x,2]:=$00;
          Specials[x]:=TSpecial(TSDLImageButton(SettingsMenu.FindComponent(IntToStr(x)+'Spez')).DispImage);
         end;
        end;
        SettingsMenu.Free;
        SettingsMenu:=nil;
        SDL_FillRect(TheScreen,nil,0);
       end;
       SDL_Flip(TheScreen);
      end;
   (* ------------------- LOKALES SPIELMENU ---------------*)
   10: begin // Lokales Spielmenü
       AnimText(LocalTitelText);
       ProcessTitelEvents;
       SDL_Flip(TheScreen);
      end;
   (* ------------------- LOKALER SIEGERSCREEN ---------------*)
   11: begin // Lokaler Sigerscreen
       AnimText(LocalSiegerText);
       for i:=1 to MaxSpieler do begin
        if SchlangenRang[i]>0 then begin
         if Punkte[SchlangenRang[i]]=1 then TextOut(Inttostr(i)+'. (1 Punkt)',TheScreen,100,100+i*85,50,GameFont,GetR(Farben[SchlangenRang[i]]),GetG(Farben[SchlangenRang[i]]),GetB(Farben[SchlangenRang[i]]))
          else TextOut(Inttostr(i)+'. ('+Inttostr(Punkte[SchlangenRang[i]])+' Punkte)',TheScreen,100,100+i*85,50,GameFont,GetR(Farben[SchlangenRang[i]]),GetG(Farben[SchlangenRang[i]]),GetB(Farben[SchlangenRang[i]]));
        end;
       end;
       ProcessSiegerEvents;
       SDL_Flip(TheScreen);
      end;
   (* ------------------- LOKALER END-OF-ROUND SCREEN ---------------*)
   1: begin // Ende der Runde
       ProcessRundenEvents;
       SDL_Flip(TheScreen);
      end;
   (* ------------------- LOKALES SPIEL LÄUFT -----------------------*)
   2: begin // Spiel Läuft
       Ticks:=SDL_GetTicks;
       ProcessSpielEvents;
       SchlangenBewegen;
       if SpielerCount<=CompiCount then begin
        Step:=(Step+1) mod CalcStepSize;
        if Step=0 then
         SDL_Flip(TheScreen);
       end
       else SDL_Flip(TheScreen);
       dec(Ticks,SDL_GetTicks);
       if SpielerCount>CompiCount then
        if GameSpeed+Ticks>0 then SDL_Delay(GameSpeed+Ticks);
      end;
   (* ------------------- LAN-Server (warte auf Spieler) --------------- *)
   -20: begin
       if LanHostMenu=nil then begin
        SDL_FillRect(TheScreen,nil,0);
        LanHostMenu:=TLanHostMenuOwner.Create(CursorBMP,TheScreen,MouseX,MouseY);
//        OpenServer;
       end;
       AnimText(LanHostText);
//       ShowClients(TheScreen);
//       CheckServer;
//       DoServerTest;
       LanHostMenu.Paint;
       if RunLevel<>-20 then begin
        LanHostMenu.Free;
        LanHostMenu:=nil;
        SDL_FillRect(TheScreen,nil,0);
//        if RunLevel=0 then begin
//         CloseServer;
        end;
//        else HostStartGame;
//       end;
       SDL_Flip(TheScreen);
      end;
   (* ------------------- LAN-Client (Connect) ---------------*)
   -9: begin
       if LanClientConnectMenu=nil then begin
        SDL_FillRect(TheScreen,nil,0);
        LanClientConnectMenu:=TLanClientConnectMenuOwner.Create(CursorBMP,TheScreen,MouseX,MouseY);
       end;
       AnimText(LanClientConnectText);
       LanClientConnectMenu.Paint;
       if RunLevel<>-9 then begin
        LanClientConnectMenu.Free;
        LanClientConnectMenu:=nil;
        SDL_FillRect(TheScreen,nil,0);
       end;
       SDL_Flip(TheScreen);
      end;
   (* ------------------- LAN-Client (warte auf Spieler) --------------- *)
   -10: begin
       if LanClientWaitMenu=nil then begin
        SDL_FillRect(TheScreen,nil,0);
        LanClientWaitMenu:=TLanClientWaitMenuOwner.Create(CursorBMP,TheScreen,MouseX,MouseY);
       end;
       AnimText(LanClientText);
//       ShowClients(TheScreen);
//       DoClientTest;
       LanClientWaitMenu.Paint;
       if RunLevel<>-10 then begin
        LanClientWaitMenu.Free;
        LanClientWaitMenu:=nil;
        SDL_FillRect(TheScreen,nil,0);
       end;
       SDL_Flip(TheScreen);
      end;
   (* ------------------- LAN END-OF-ROUND SCREEN ---------------*)
(*   -1: begin // Ende der Runde
       if IAmTheMaster then
        ProcessRundenEvents
       else ClientCheckRound;
       SDL_Flip(TheScreen);
      end;*)
   (* ------------------- LAN SPIEL LÄUFT -----------------------*)
(*   -2: begin // Spiel Läuft
       if IAmTheMaster then begin
        Ticks:=SDL_GetTicks;
        ProcessSpielEvents;
        SyncWithClients;
        SendFrameData;
        SchlangenBewegen;
        if SpielerCount<=CompiCount then begin
         Step:=(Step+1) mod CalcStepSize;
         if Step=0 then
          SDL_Flip(TheScreen);
        end
        else SDL_Flip(TheScreen);
        dec(Ticks,SDL_GetTicks);
        if SpielerCount>CompiCount then
         if GameSpeed+Ticks>0 then SDL_Delay(GameSpeed+Ticks);
       end
       else begin
        ProcessSpielEvents;
        ClientSendSync;
        GetServerSync;
        SchlangenBewegen;
        SDL_Flip(TheScreen);
       end;
      end; *)
  end;
 until RunLevel=555;
 Quit;
end.

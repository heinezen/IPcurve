unit engine;

interface
uses SDL,
     schlangen,
     maps,
     constants,
     grafik,
     log,
     IniFiles,
     SysUtils;
procedure Quit(error:boolean=false);
procedure ShowScores;
procedure InitGame;
procedure LoadSettings;
procedure SaveSettings;
procedure StartRound;
procedure EndGame(ende:boolean=false);
procedure StartGame;
procedure KillSnake(i:integer);
procedure SchlangenBewegen;  // Schlangen bewegen und zeichnen
procedure ProcessTitelEvents;
procedure ProcessRundenEvents;
procedure ProcessSpielEvents;
procedure ProcessSiegerEvents;

var Spieler:array[1..MaxSpieler] of TSchlange;
    Farben:array[1..MaxSpieler] of Word;
    Tasten:array[1..MaxSpieler,1..3] of Word;
    Drin:array[1..MaxSpieler] of Boolean;
    Compi:array[1..MaxSpieler] of Boolean;
    Specials:array[1..MaxSpieler] of TSpecial;
    CompiAlgs:array[1..MaxSpieler] of TAIAlg;
    TheMap:TMap;
    SpielerCount:integer;
    CompiCount:integer;
    Ziel:integer;
    RunLevel:integer;
    WaitRest:integer;
    LochAbstandMax:integer;
    LochAbstandMin:integer;
    LochSize:integer;
    Loecher:boolean;
    DoAnimText:boolean;
    GameSpeed:integer;
    BlinkZeit:integer;
    KIWaitZeit:integer;
    BlinkSchlange:integer;
    MouseX,MouseY:integer;
    CalcStepSize:integer;
    SchlangenRang:array[1..MaxSpieler] of integer;
    Punkte:array[1..MaxSpieler] of integer;
(*
RunLevels
---------
0 - Titelbild

1 - Ende der Runde, lokales Spiel
2 - Runde im Gange, lokales Spiel

10 - Lokales Spielmenü
11 - Lokaler "Ausgekurvt" Bildschirm


-1 - Ende der Runde, LAN
-2 - Runde im Gange, LAN

-20 - Host-Bildschirm (LAN)
-9 - Client-Connect-Bildschirm (LAN)
-9 - Client-Bildschirm (LAN)

42 - Einstellungsmenu
89 - Credits-Bildschirm

*)
implementation

function GetRang(i:integer):integer;
var x:integer;
begin
 Result:=1;
 for x:=1 to MaxSpieler do
  if SchlangenRang[x]=i then begin
   Result:=x;
   Break;
  end;
end;

procedure Quit(error:boolean=false);
begin
 ExitGraph;
 SDL_ShowCursor(1);
 SDL_Quit;
 if error then LogOut('kurveSdl Error:'+SDL_GetError)
 else SaveSettings;
 Halt;
end;


procedure ShowScores;
var i,x:integer;
begin
 for i:=1 to MaxSpieler do begin
  x:=SchlangenRang[i];
  if (x>0)and(Spieler[x]<>nil) then begin
   SetQuad(SpielFeldWidth+20,(i-1)*(ScoreHeight+SpecialHeight+30)+30,300,ScoreHeight+5+SpecialHeight,$1111,TheScreen);
   NumberOut(Spieler[x].Score,TheScreen,SpielFeldWidth+20,(i-1)*(ScoreHeight+SpecialHeight+30)+30,Spieler[x].ScoreGfx);
   if Spieler[x].Special = spInOut then
    NumberOut(Spieler[x].SpecialCount+1,TheScreen,SpielFeldWidth+50,(i-1)*(ScoreHeight+SpecialHeight+30)+ScoreHeight+30,Spieler[x].SpecialGfx)
   else if Spieler[x].Special <> spNone then
    NumberOut(Spieler[x].SpecialCount,TheScreen,SpielFeldWidth+50,(i-1)*(ScoreHeight+SpecialHeight+30)+ScoreHeight+30,Spieler[x].SpecialGfx);
  end;
 end;
end;

procedure InitGame;
var x:integer;
begin
 for x:=1 to MaxSpieler do begin
  Drin[x]:=false;
  Compi[x]:=false;
  Spieler[x]:=nil;
 end;
end;

procedure LoadSettings;
var Einstellungen:TIniFile;
    x:integer;
begin
 Einstellungen:=TIniFile.Create('.'+PathDelim+'kurve.ini');
 for x:=1 to MaxSpieler do Farben[x]:=Einstellungen.ReadInteger('color','color'+inttostr(x),FarbDefaults[x]);
 for x:=1 to MaxSpieler do Tasten[x,1]:=Einstellungen.ReadInteger('keys','keyLeft'+inttostr(x),TastenDefaults[x,1]);
 for x:=1 to MaxSpieler do Tasten[x,2]:=Einstellungen.ReadInteger('keys','keyRight'+inttostr(x),TastenDefaults[x,2]);
 for x:=1 to MaxSpieler do Tasten[x,3]:=Einstellungen.ReadInteger('keys','keySpec'+inttostr(x),TastenDefaults[x,3]);
 for x:=1 to MaxSpieler do Specials[x]:=TSpecial(Einstellungen.ReadInteger('spec','special'+inttostr(x),0));
 for x:=1 to MaxSpieler do CompiAlgs[x]:=TAIAlg(Einstellungen.ReadInteger('ki','alg'+inttostr(x),0));
 DoAnimText:=Einstellungen.ReadBool('graphics','animtext',DoAnimTextStd);
 LochAbstandMax:=Einstellungen.ReadInteger('holes','max',LochAbstandMaxStd);
 LochAbstandMin:=Einstellungen.ReadInteger('holes','min',LochAbstandMinStd);
 LochSize:=Einstellungen.ReadInteger('holes','size',LochSizeStd);
 Loecher:=Einstellungen.ReadBool('holes','hasholes',LoecherStd);
 GameSpeed:=Einstellungen.ReadInteger('speed','gamespeed',GameSpeedStd);
 BlinkZeit:=Einstellungen.ReadInteger('speed','flashtime',BlinkZeitStd);
 KIWaitZeit:=Einstellungen.ReadInteger('ki','kistep',KIWaitZeitStd);
 CalcStepSize:=Einstellungen.ReadInteger('ki','kicalcstep',CalcStepSizeStd);
 lang:=TLang(Einstellungen.ReadInteger('locale','lang',Ord(de)));
 Einstellungen.Free;
end;

procedure SaveSettings;
var Einstellungen:TIniFile;
    x:integer;
begin
 Einstellungen:=TIniFile.Create('.'+PathDelim+'kurve.ini');
 for x:=1 to MaxSpieler do Einstellungen.WriteInteger('color','color'+inttostr(x),Farben[x]);
 for x:=1 to MaxSpieler do Einstellungen.WriteInteger('keys','keyLeft'+inttostr(x),Tasten[x,1]);
 for x:=1 to MaxSpieler do Einstellungen.WriteInteger('keys','keyRight'+inttostr(x),Tasten[x,2]);
 for x:=1 to MaxSpieler do Einstellungen.WriteInteger('keys','keySpec'+inttostr(x),Tasten[x,3]);
 for x:=1 to MaxSpieler do Einstellungen.WriteInteger('spec','special'+inttostr(x),Ord(Specials[x]));
 for x:=1 to MaxSpieler do Einstellungen.WriteInteger('ki','alg'+inttostr(x),Ord(CompiAlgs[x]));
 Einstellungen.WriteBool('graphics','animtext',DoAnimText);
 Einstellungen.WriteInteger('holes','max',LochAbstandMax);
 Einstellungen.WriteInteger('holes','min',LochAbstandMin);
 Einstellungen.WriteInteger('holes','size',LochSize);
 Einstellungen.WriteBool('holes','hasholes',Loecher);
 Einstellungen.WriteInteger('speed','gamespeed',GameSpeed);
 Einstellungen.WriteInteger('speed','flashtime',BlinkZeit);
 Einstellungen.WriteInteger('ki','kistep',KIWaitZeit);
 Einstellungen.WriteInteger('ki','kicalcstep',CalcStepSize);
 Einstellungen.WriteInteger('locale','lang',Ord(lang));
 Einstellungen.Free;
end;

function getScount(i:integer):integer;
begin
Result:=GetRang(i);
if Spieler[i].Special=spTradePlaces then Dec(result);
if Spieler[i].Special=spInOut then Inc(result);
end;

procedure StartRound; // Runde Starten
var x:integer;
begin
 RunLevel:=2;
 WaitRest:=BlinkZeit;
 BlinkSchlange:=1;
 SetQuad(0,0,MapWidth,MapHeight,$1111,TheScreen);
 SetQuad(10,10,SpielfeldWidth,SpielfeldHeight,$0000,TheScreen);
 FillMap(@TheMap,mapWall);
 SetItemQuad(@TheMap,mapFree,10,10,SpielfeldWidth,SpielfeldHeight);
 SpielerCount:=0;
 CompiCount:=0;
 for x:=1 to MaxSpieler do
  If Spieler[x]<>nil then begin
   Spieler[x].x:=200+Random(SpielfeldWidth-400);
   Spieler[x].y:=200+Random(SpielfeldHeight-400);
   Spieler[x].Richtung:=Random(359);
   Spieler[x].Alive:=true;
   Spieler[x].Taste:=keine;
   Spieler[x].SpecialCount:=GetSCount(x);
   Spieler[x].SpecialPower:=0;
//   if Ziel>0 then Spieler[x].SpecialCount:=round((1-(Spieler[x].Score/Ziel)))*10
//   else Spieler[x].SpecialCount:=9999;
   Spieler[x].Clean;
   if Spieler[x].IstKI then
    Inc(CompiCount);
   Inc(SpielerCount);
  end;
 ShowScores
end;

procedure EndGame(ende:boolean=false); // Spiel Beenden
var x:integer;
begin
 if ende then RunLevel:=11
 else RunLevel:=10;
 SpielerCount:=0;
 CompiCount:=0;
 for x:=1 to MaxSpieler do begin
  if Spieler[x]<>nil then
  begin
   Punkte[x]:=Spieler[x].Score;
   Spieler[x].Free;
  end
  else
   Punkte[x]:=-1;
  Spieler[x]:=nil;
  Drin[x]:=false;
  Compi[x]:=false;
 end;
 SetQuad(0,0,MapWidth,MapHeight,$0000,TheScreen);
end;

procedure EndRound; // Runde Beenden
var x:integer;
begin
 RunLevel:=1;
 for x:=1 to MaxSpieler do
  if Spieler[x]<>nil then
   if Spieler[x].Score>=Ziel then
    EndGame(true);
end;


procedure StartGame;  // Spiel Starten
var x:integer;
    i:integer;
begin
 RunLevel:=1;
 i:=0;
 for x:=1 to MaxSpieler do
   SchlangenRang[x]:=-1;
 for x:=1 to MaxSpieler do
  if Drin[x] then begin
   Inc(i);
   SchlangenRang[i]:=x;
   Spieler[x]:=TSchlange.Create(Farben[x],x);
   Spieler[x].IstKI:=Compi[x];
  end;
 if SpielerCount=1 then Ziel:=1
  else Ziel:=(SpielerCount-1)*10;
 StartRound;
end;


procedure KillSnake(i:integer);
var x,t,r:integer;
begin
 if Spieler[i]<>nil then Spieler[i].Alive:=false;
 dec(SpielerCount);
 if Spieler[i].IstKI then
  dec(compiCount);
 for x:=1 to MaxSpieler do
  if (Spieler[x]<>nil) and (Spieler[x].Alive) then begin
   Spieler[x].Score:=Spieler[x].Score+1;
   r:=GetRang(x);
   while (r>1)and(Spieler[x].Score>Spieler[SchlangenRang[r-1]].Score) do begin
//   if (r>1) then begin
//    if Spieler[x].Score>Spieler[SchlangenRang[r-1]].Score then begin
     t:=SchlangenRang[r-1];
     SchlangenRang[r-1]:=SchlangenRang[r];
     SchlangenRang[r]:=t;
     dec(r);
//    end;
//   end;
   end;
  end;
 ShowScores;
 if (SpielerCount<=1)and(Runlevel=2) then EndRound;
// else if IAmTheMaster and (Runlevel=-2)and(SpielerCount<=1) then HostEndRound;
end;

procedure SchlangenBewegen;  // Schlangen bewegen und zeichnen
var x:integer;
begin
// if WaitRest>0 then dec(WaitRest);
 for x:=1 to MaxSpieler do
  if (Spieler[x]<>nil)and(Spieler[x].Alive) then begin
   if BlinkSchlange<=SpielerCount then begin
    if (x=SchlangenRang[1+SpielerCount-BlinkSchlange]) then begin
     if (WaitRest>0) then begin
      dec(WaitRest);
      if ((WaitRest mod 9)>4)or (not(WaitRest>0)) then
       SetQuad(Spieler[x].x,Spieler[x].y,Spieler[x].width,Spieler[x].width,Spieler[x].color,TheScreen)
      else SetQuad(Spieler[x].x,Spieler[x].y,Spieler[x].width,Spieler[x].width,$0000,TheScreen);
     end
     else begin
      WaitRest:=BlinkZeit;
      inc(BlinkSchlange);
     end;
    end;
   end else begin
     if (Runlevel>0) then
      Spieler[x].MoveBegin;



     if Spieler[x].CollisionCheck(@TheMap)<>mapFree then
      KillSnake(x);

      if abs(RunLevel)=2 then begin // Runde läuft noch..
       if not(Spieler[x].MoveEnd) then begin
        SetQuad(Spieler[x].OldX,Spieler[x].OldY,Spieler[x].width,Spieler[x].width,$0000,TheScreen);
        SetItemQuad(@TheMap,mapFree,Spieler[x].OldX,Spieler[x].OldY,Spieler[x].width,Spieler[x].width);
       end
       else if Spieler[x].Alive then begin
        SetQuad(Spieler[x].OldX,Spieler[x].OldY,Spieler[x].width,Spieler[x].width,Spieler[x].color,TheScreen);
        SetItemQuad(@TheMap,mapSnake,Spieler[x].OldX,Spieler[x].OldY,Spieler[x].width,Spieler[x].width);
       end;
      end;
    if (abs(RunLevel)=1) or (abs(RunLevel)=2) then begin // Spiel läuft noch..
      SetQuad(Spieler[x].x,Spieler[x].y,Spieler[x].width,Spieler[x].width,Spieler[x].color,TheScreen);
      SetItemQuad(@TheMap,mapSnake,Spieler[x].x,Spieler[x].y,Spieler[x].width,Spieler[x].width);
    end;
   end;
  end;
end;

procedure SetAIAlg(x,y,h:integer;Alg:TAIAlg;col:Cardinal);
var s:string;
begin
 case Alg of
  aiHumanoid: s:='Normal';
  aiUrs: s:='Enhanced';
  aiFilE: s:='Special';
 end;

 SetQuad(x,y,200,h,$0000,TheScreen);
 TextOut(' '+s,TheScreen,x,y,h,GameFont,GetR(col),GetG(col),GetB(col));


end;



procedure ProcessTitelEvents;
var Event:TSDL_Event;
    x:integer;
begin
 while SDL_PollEvent(@Event)<>0 do begin
  case Event.type_ of
   SDL_QUITEV: begin // Programm soll beendet werden
                Quit;
               end;
   SDL_KEYDOWN: begin // Taste Gedrückt
                 // Space
                 if event.key.keysym.sym=SDLK_SPACE then
                  if SpielerCount>0 then begin
                   StartGame;
                   Break;
                  end;
                 // Escape
                 if Event.key.keysym.sym=SDLK_ESCAPE then
                 begin
                  RunLevel:=0;
                  for x:=1 to MaxSpieler do begin
                   Drin[x]:=false;
                   Compi[x]:=false;
                  end;
                  SpielerCount:=0;
                  CompiCount:=0;
                 end;
                 //Spieler-Taste
                 for x:=1 to MaxSpieler do begin
                  if (Event.key.keysym.sym=Tasten[x,1]) then begin
                   if (not Drin[x]) then begin
                    Drin[x]:=true;
                    TextOut(GetText(ReadyText),TheScreen,10,100+x*40,30,GameFont,GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
                    inc(SpielerCount);
                   end
                   else begin
                    Compi[x]:=not Compi[x];
                    if Compi[x] then begin
                     TextOut(GetText(KIText),TheScreen,150,100+x*40,30,GameFont,GetR(Farben[x]),GetG(Farben[x]),GetB(Farben[x]));
                     SetAIAlg(200,105+x*40,20,CompiAlgs[x],Farben[x]);
                     end
                     else SetQuad(150,100+x*40,600,30,$0000,TheScreen);
                   end;
                  end;
                  if (Event.key.keysym.sym=Tasten[x,2])and(Drin[x]) then begin
                   Drin[x]:=false;
                   Compi[x]:=false;
                   SetQuad(10,100+x*40,600,30,$0000,TheScreen);
                   dec(SpielerCount);
                  end;
                  if (Event.key.keysym.sym=Tasten[x,3])and(Compi[x]) then begin
                   CompiAlgs[x]:=TAiAlg((Ord(CompiAlgs[x])+1) mod (Ord(High(TAIAlg))+1));
                   SetAIAlg(200,105+x*40,20,CompiAlgs[x],Farben[x]);
                  end
                 end; //for
                end;  //keydown
   SDL_MOUSEBUTTONDOWN: begin // Maus gedrückt
     if MausSteuerung<>0 then begin
      if Event.button.button=SDL_BUTTON_LEFT then //linke taste
        if (not Drin[MausSteuerung]) then begin
         Drin[MausSteuerung]:=true;
         TextOut(GetText(ReadyText),TheScreen,10,100+MausSteuerung*40,30,GameFont,GetR(Farben[MausSteuerung]),GetG(Farben[MausSteuerung]),GetB(Farben[MausSteuerung]));
         inc(SpielerCount);
        end
        else begin
         Compi[MausSteuerung]:=not Compi[MausSteuerung];
        if Compi[MausSteuerung] then begin
         TextOut(GetText(KIText),TheScreen,150,100+MausSteuerung*40,30,GameFont,GetR(Farben[MausSteuerung]),GetG(Farben[MausSteuerung]),GetB(Farben[MausSteuerung]));
         SetAIAlg(200,105+MausSteuerung*40,20,CompiAlgs[MausSteuerung],Farben[MausSteuerung]);
        end
        else SetQuad(150,100+MausSteuerung*40,600,30,$0000,TheScreen);
       end;

      if Event.button.button=SDL_BUTTON_RIGHT then //recchte taste
       if (Drin[MausSteuerung]) then begin
        Drin[MausSteuerung]:=false;
        SetQuad(10,100+MausSteuerung*40,600,30,$0000,TheScreen);
        dec(SpielerCount);
       end;
      if Event.button.button=SDL_BUTTON_MIDDLE then //recchte taste
       if (Compi[MausSteuerung]) then begin
        CompiAlgs[MausSteuerung]:=TAiAlg((Ord(CompiAlgs[MausSteuerung])+1) mod (Ord(High(TAIAlg))+1));
        SetAIAlg(200,105+MausSteuerung*40,20,CompiAlgs[MausSteuerung],Farben[MausSteuerung]);
       end;
     end; //maussteuerung zugeordnet
   end;

  end; //case
 end; //while
end;

procedure ProcessSiegerEvents;
var Event:TSDL_Event;
    x:integer;
begin
 while SDL_PollEvent(@Event)<>0 do begin
  case Event.type_ of
   SDL_QUITEV: begin // Programm soll beendet werden
                Quit;
               end;
   SDL_KEYDOWN: begin // Taste Gedrückt
                 if event.key.keysym.sym=SDLK_SPACE then begin
                  RunLevel:=10;
                  SDL_FillRect(TheScreen,nil,$0000);
                 end;
                 if Event.key.keysym.sym=SDLK_ESCAPE then
                 begin
                  RunLevel:=0;
                  SDL_FillRect(TheScreen,nil,$0000);
                  for x:=1 to MaxSpieler do begin
                   Drin[x]:=false;
                   Compi[x]:=false;
                  end;
                  SpielerCount:=0;
                  CompiCount:=0;
                 end;
                end;
  end;
 end; //while
end;



procedure ProcessRundenEvents;
var Event:TSDL_Event;
begin
 while SDL_PollEvent(@Event)<>0 do begin
  case Event.type_ of
   SDL_QUITEV: begin // Programm soll beendet werden
                Quit;
               end;
   SDL_KEYDOWN: begin // Taste Gedrückt
                 if Event.key.keysym.sym=SDLK_PRINT then
                  SDL_SaveBMP(TheScreen,'kurveScreen.bmp');
                 if RunLevel=-1 then begin
//                  if Event.key.keysym.sym=SDLK_ESCAPE then HostEndGame;
//                  if event.key.keysym.sym=SDLK_SPACE then
//                   HostStartRound;
                 end
                 else begin
                  if Event.key.keysym.sym=SDLK_ESCAPE then EndGame;
                  if event.key.keysym.sym=SDLK_SPACE then
                   StartRound;
                  if event.key.keysym.sym=SDLK_F1 then
                   if spieler[1]<>nil then begin
                    Compi[1]:=not Compi[1];
                    Spieler[1].IstKI:=Compi[1];
                    SetQuad(115,50+1*50,800,40,$0000,TheScreen);
                    if Compi[1] then
                     TextOut(GetText('Spieler')+' 1 '+GetText('ist jetzt ein KI'),TheScreen,120,50+1*50,40,GameFont,GetR(Farben[1]),GetG(Farben[1]),GetB(Farben[1]))
                    else
                     TextOut(GetText('Spieler')+' 1 '+GetText('ist jetzt kein KI mehr'),TheScreen,120,50+1*50,40,GameFont,GetR(Farben[1]),GetG(Farben[1]),GetB(Farben[1]))
                   end;
                  if event.key.keysym.sym=SDLK_F2 then
                   if spieler[2]<>nil then begin
                    Compi[2]:=not Compi[2];
                    Spieler[2].IstKI:=Compi[2];
                    SetQuad(115,50+2*50,800,40,$0000,TheScreen);
                    if Compi[2] then
                     TextOut(GetText('Spieler')+' 2 '+GetText('ist jetzt ein KI'),TheScreen,120,50+2*50,40,GameFont,GetR(Farben[2]),GetG(Farben[2]),GetB(Farben[2]))
                    else
                     TextOut(GetText('Spieler')+' 2 '+GetText('ist jetzt kein KI mehr'),TheScreen,120,50+2*50,40,GameFont,GetR(Farben[2]),GetG(Farben[2]),GetB(Farben[2]))
                   end;
                  if event.key.keysym.sym=SDLK_F3 then
                   if spieler[3]<>nil then begin
                    Compi[3]:=not Compi[3];
                    Spieler[3].IstKI:=Compi[3];
                    SetQuad(115,50+3*50,800,40,$0000,TheScreen);
                    if Compi[3] then
                     TextOut(GetText('Spieler')+' 3 '+GetText('ist jetzt ein KI'),TheScreen,120,50+3*50,40,GameFont,GetR(Farben[3]),GetG(Farben[3]),GetB(Farben[3]))
                    else
                     TextOut(GetText('Spieler')+' 3 '+GetText('ist jetzt kein KI mehr'),TheScreen,120,50+3*50,40,GameFont,GetR(Farben[3]),GetG(Farben[3]),GetB(Farben[3]))
                   end;
                  if event.key.keysym.sym=SDLK_F4 then
                   if spieler[4]<>nil then begin
                    Compi[4]:=not Compi[4];
                    Spieler[4].IstKI:=Compi[4];
                    SetQuad(115,50+4*50,800,40,$0000,TheScreen);
                    if Compi[4] then
                     TextOut(GetText('Spieler')+' 4 '+GetText('ist jetzt ein KI'),TheScreen,120,50+4*50,40,GameFont,GetR(Farben[4]),GetG(Farben[4]),GetB(Farben[4]))
                    else
                     TextOut(GetText('Spieler')+' 4 '+GetText('ist jetzt kein KI mehr'),TheScreen,120,50+4*50,40,GameFont,GetR(Farben[4]),GetG(Farben[4]),GetB(Farben[4]))
                   end;
                  if event.key.keysym.sym=SDLK_F5 then
                   if spieler[5]<>nil then begin
                    Compi[5]:=not Compi[5];
                    Spieler[5].IstKI:=Compi[5];
                    SetQuad(115,50+5*50,800,40,$0000,TheScreen);
                    if Compi[5] then
                     TextOut(GetText('Spieler')+' 5 '+GetText('ist jetzt ein KI'),TheScreen,120,50+5*50,40,GameFont,GetR(Farben[5]),GetG(Farben[5]),GetB(Farben[5]))
                    else
                     TextOut(GetText('Spieler')+' 5 '+GetText('ist jetzt kein KI mehr'),TheScreen,120,50+5*50,40,GameFont,GetR(Farben[5]),GetG(Farben[5]),GetB(Farben[5]))
                   end;
                  if event.key.keysym.sym=SDLK_F6 then
                   if spieler[6]<>nil then begin
                    Compi[6]:=not Compi[6];
                    Spieler[6].IstKI:=Compi[6];
                    SetQuad(115,50+6*50,800,40,$0000,TheScreen);
                    if Compi[6] then
                     TextOut(GetText('Spieler')+' 6 '+GetText('ist jetzt ein KI'),TheScreen,120,50+6*50,40,GameFont,GetR(Farben[6]),GetG(Farben[6]),GetB(Farben[6]))
                    else
                     TextOut(GetText('Spieler')+' 6 '+GetText('ist jetzt kein KI mehr'),TheScreen,120,50+6*50,40,GameFont,GetR(Farben[6]),GetG(Farben[6]),GetB(Farben[6]))
                   end;
                  if event.key.keysym.sym=SDLK_F7 then
                   if spieler[7]<>nil then begin
                    Compi[7]:=not Compi[7];
                    Spieler[7].IstKI:=Compi[7];
                    SetQuad(115,50+7*50,800,40,$0000,TheScreen);
                    if Compi[7] then
                     TextOut(GetText('Spieler')+' 7 '+GetText('ist jetzt ein KI'),TheScreen,120,50+7*50,40,GameFont,GetR(Farben[7]),GetG(Farben[7]),GetB(Farben[7]))
                    else
                     TextOut(GetText('Spieler')+' 7 '+GetText('ist jetzt kein KI mehr'),TheScreen,120,50+7*50,40,GameFont,GetR(Farben[7]),GetG(Farben[7]),GetB(Farben[7]))
                   end;
                 end;
                end;  //keydown
  end; //case
 end; //while
end;



procedure ProcessSpielEvents;
var Event:TSDL_Event;
    x:integer;
begin
 while SDL_PollEvent(@Event)<>0 do begin
  case Event.type_ of
   SDL_QUITEV: begin // Programm soll beendet werden
                Quit;
               end;
   SDL_KEYDOWN: begin // Taste Gedrückt
                 if Event.key.keysym.sym=SDLK_ESCAPE then begin
//                  if RunLevel=-2 then begin
//                   if IAmTheMaster then HostEndRound
//                                   else ClientEndGame;
//                  end
                  EndRound;
                 end;
                 if Event.key.keysym.sym=SDLK_PRINT then
                  SDL_SaveBMP(TheScreen,'kurveScreen.bmp');
                 for x:=1 to MaxSpieler do
                  if Spieler[x]<>nil then begin
                   if Event.key.keysym.sym=Tasten[x,1] then
                    Spieler[x].Taste:=links;
                   if Event.key.keysym.sym=Tasten[x,2] then
                    Spieler[x].Taste:=rechts;
                   if (Event.key.keysym.sym=Tasten[x,3]) then begin
                    Spieler[x].StartSpecial;
                   end;
                   end;
                end; //keydown
   SDL_KEYUP: begin // Taste Losgelassen
                 for x:=1 to MaxSpieler do
                  if Spieler[x]<>nil then begin
                   if Event.key.keysym.sym=Tasten[x,1] then
                    if Spieler[x].Taste=links then Spieler[x].Taste:=keine;
                   if Event.key.keysym.sym=Tasten[x,2] then
                    if Spieler[x].Taste=rechts then Spieler[x].Taste:=keine;
                   end;
                end; //keyup
   SDL_MOUSEBUTTONDOWN: begin //maustaste gedrückt
     if MausSteuerung<>0 then begin
      if Event.button.button=SDL_BUTTON_LEFT then //linke taste
       if Spieler[MausSteuerung]<>nil then
        Spieler[MausSteuerung].Taste:=links;

      if Event.button.button=SDL_BUTTON_RIGHT then //recchte taste
       if Spieler[MausSteuerung]<>nil then
        Spieler[MausSteuerung].Taste:=rechts;

      if Event.button.button=SDL_BUTTON_MIDDLE then //recchte taste
       if (Spieler[MausSteuerung]<>nil) then begin
        Spieler[MausSteuerung].StartSpecial;
       end;
     end; //maussteuerung zugeordnet
   end; //mousedown

   SDL_MOUSEBUTTONUP: begin
     if MausSteuerung<>0 then begin
      if Event.button.button=SDL_BUTTON_LEFT then //linke taste
       if (Spieler[MausSteuerung]<>nil) then
        if Spieler[MausSteuerung].Taste=links then Spieler[MausSteuerung].Taste:=keine;

      if Event.button.button=SDL_BUTTON_RIGHT then //rechte taste
       if (Spieler[MausSteuerung]<>nil) then
        if Spieler[MausSteuerung].Taste=rechts then Spieler[MausSteuerung].Taste:=keine;
     end;
   end;
  end; //case
 end; //while
end;


end.

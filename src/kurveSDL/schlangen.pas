unit schlangen;

interface
uses SDL,SysUtils,Math,constants,maps,grafik,SDLUtils;

type
TTaste = (links,rechts,keine);
TAIAlg = (aiHumanoid,aiUrs,aiFilE);
TSpecial = (spNone,spInOut,spAcc,spCurve,spOpenHole,spJump,spBarriere,spTradePlaces);
TXY = record
 x,y:integer;
end;
TDir = (dirx,diry);
TLastPlaces = array[1..5] of TXY;


TSchlange = class
 private
  FX,FY:real;
  FTaste:TTaste;
  FRichtung:Integer;
  KRadius:Integer;
  KSpeed:Integer;
  FWidth:Integer;
  FScore:Integer;
  FColor:Word;
  FActive:boolean;
  Loch:integer;
  HatLoecher:boolean;
  FIstKI:boolean;
  function GetFX:integer;
  function GetFY:integer;
  function GetOldX:integer;
  function GetOldY:integer;
  procedure SetFX(const Value:integer);
  procedure SetFY(const Value:integer);
  procedure SetRichtung(const Value:integer);
 public
  ScoreGfx:NumberGfxArray;
  SpecialGfx:NumberGfxArray;
 // Versch. KI Attribute
  AiRest:integer;
  AiAlt:integer;
  AIData:Pointer;
 // Schlange Bereinigen
  LastPlaces:TLastPlaces;
  LastPlacesPtr:integer;
  Special:TSpecial;
  SpecialPower:integer;
  SpecialCount:integer;
  FLoch:boolean;
  AIAlg:TAIAlg;
  AIInvertAngles:boolean;
  function CollisionCheck(Map:PMap):TMapItem;
  function CollisionCheckXY(Map:PMap;wx,wy:real;streng:boolean=false):TMapItem;
  procedure StartSpecial;
  constructor Create(Farbe:word;num:integer);
  procedure MoveBegin;
  function MoveEnd:boolean;
  property x:integer read GetFX write SetFX;
  property y:integer read GetFY write SetFY;
  property RawX:real read FX write FX;
  property RawY:real read FY write FY;
  property OldX:integer read GetOldX;
  property OldY:integer read GetOldY;
  property Taste:TTaste read FTaste write FTaste;
  property Richtung:integer read FRichtung write SetRichtung;
  property Color:Word read FColor write FColor;
  property Width:Integer read FWidth write FWidth;
  property Alive:Boolean read FActive write FActive;
  property IstKI:Boolean read FIstKI write FIstKI;
  property Score:integer read FScore write FScore;
  procedure Clean;
end;
PSchlange = ^TSchlange;

implementation
 uses engine,ki;
 procedure TSchlange.Clean;
 var i:integer;
 begin
  for i:=1 to 5 do begin
   LastPlaces[i].x:=-10;
   LastPlaces[i].y:=-10;
   LastPlacesPtr:=1;
  end;
 end;

function GetPow(Special:TSpecial):integer;
begin
 Result:=0;
 case Special of
  spNone: Result:=0;
  spInOut: Result:=50;
  spAcc: Result:=50;
  spCurve: Result:=50;
  spOpenHole: Result:=2;
  spJump: Result:=2;
  spBarriere: Result:=2;
  spTradePlaces: Result:=Random(100)+1;
 end;
end;
 

 procedure TSchlange.StartSpecial;
 begin
  if (SpecialCount>0)and(SpecialPower=0)and(OldX>0) then begin
   SpecialPower:=GetPow(Special);
   // Bots use TradePlaces INSANTLY
    if (Special=spTradePlaces) and FIstKI then SpecialPower:=2;
   dec(SpecialCount);
   ShowScores;
  end;
 end;

 constructor TSchlange.Create;
 var x:integer;
     c:char;
 begin
  inherited Create;
  //if num=3 then KRadius:=2
  //else
  KRadius:=3;
  AIData:=nil;
  FLoch:=false;
  Special:=Specials[num];
  SpecialCount:=0;
  SpecialPower:=0;

  AIAlg:=CompiAlgs[num];
  AIInvertAngles:=boolean(Random(1));

  KSpeed:=2;
  FColor:=Farbe;
  FWidth:=4;
  FTaste:=keine;
  Loch:=-1;
  HatLoecher:=Loecher;
  FScore:=0;
  for x:=1 to 5 do begin
   LastPlaces[x].x:=0;
   LastPlaces[x].y:=0;
  end;
  for c:='0' to '9' do begin
   ScoreGfx[c]:=NewSurfaceTextOut(c,0,0,ScoreHeight,GameFont,GetR(Farbe),GetG(Farbe),GetB(Farbe));
   if ScoreGfx[c]=nil then Quit(true);
  end;
  for c:='0' to '9' do begin
   SpecialGfx[c]:=NewSurfaceTextOut(c,0,0,SpecialHeight,GameFont,GetR(Farbe),GetG(Farbe),GetB(Farbe));
   if SpecialGfx[c]=nil then Quit(true);
  end;
  Clean;
 end;

 function TSchlange.GetFX:integer;
 begin
  Result:=round(FX);
 end;
 function TSchlange.GetFY:integer;
 begin
  Result:=round(FY);
 end;
 procedure TSchlange.SetFX(const Value:integer);
 begin
  FX:=Value;
  LastPlaces[LastPlacesPtr].x:=round(FX);
  LastPlaces[LastPlacesPtr].y:=round(FY);
  LastPlacesPtr:=(LastPlacesPtr mod 5)+1;
 end;

 procedure TSchlange.SetFY(const Value:integer);
 begin
  FY:=Value;
  LastPlaces[LastPlacesPtr].x:=round(FX);
  LastPlaces[LastPlacesPtr].y:=round(FY);
  LastPlacesPtr:=(LastPlacesPtr mod 5)+1;
 end;
 procedure TSchlange.SetRichtung(const Value:integer);
 begin
  FRichtung:=(Value+360) mod 360;
 end;

 function InMap(x:integer;dir:TDir):integer;
 begin
  if x<10 then x:=10;
  if (dir=dirx) and (x>=SpielfeldWidth+11) then x:=SpielfeldWidth+10;
  if (dir=diry) and (x>=SpielfeldHeight+11) then x:=SpielfeldHeight+10;
  Result:=x;
 end;

 procedure TSchlange.MoveBegin;
 var Sinus,Cosinus:extended;
     x,y,i:integer;
     Temp:TLastPlaces;
 begin
  if FIstKI then SteureKIAlg(@Self);
  case FTaste of
   links: FRichtung:=(FRichtung+KRadius+360)mod 360;
   rechts: FRichtung:=(FRichtung-KRadius+360)mod 360;
  end;
  Math.SinCos((FRichtung/180)*Pi,Sinus,Cosinus);
  FX:=FX+Cosinus*KSpeed;
  FY:=FY-Sinus*KSpeed;
  if SpecialPower>1 then begin
   case Special of
    spInOut: begin
     inc(SpecialPower);
     if FX<11 then begin FX:=(SpielFeldWidth)+10-round(fx); SpecialPower:=1; end;
     if FX>SpielFeldWidth+5 then begin FX:=round(fx) mod (SpielFeldWidth)+10; SpecialPower:=1; end;
     if FY<11 then begin FY:=(SpielFeldHeight)+10-round(fy); SpecialPower:=1; end;
     if FY>SpielFeldHeight+5 then begin FY:=round(fy) mod (SpielFeldHeight)+10; SpecialPower:=1; end;

//     if FX<15 then begin FX:=SpielFeldWidth+5; SpecialPower:=1; end;
//     if FX>SpielFeldWidth+5 then begin FX:=15; SpecialPower:=1; end;
//     if FY<15 then begin FY:=MapHeight-15; SpecialPower:=1; end;
//     if FY>MapHeight-15 then begin FY:=15; SpecialPower:=1; end;
    end;
    spAcc: begin
     KSpeed:=3;
     KRadius:=4;
    end;
    spCurve: begin
     KSpeed:=1;
     KRadius:=5;
    end;
    spOpenHole: begin
     SDL_LockSurface(TheScreen);
     for x:=0 to 10 do
      for y:=0 to 10 do begin
       SDL_DrawLine(TheScreen,round(FX)+x,round(FY)+y,InMap(round(FX+Cos(Richtung/180*Pi)*100)+x,dirx),InMap(round(FY-Sin(Richtung/180*Pi)*100)+y,diry),$0000);
       SetItemLine(@TheMap,mapFree,round(FX)+x,round(FY)+y,InMap(round(FX+Cos(Richtung/180*Pi)*100)+x,dirx),InMap(round(FY-Sin(Richtung/180*Pi)*100)+y,diry));
      end;
     SDL_UnLockSurface(TheScreen);
    end;
    spJump: begin
     FX:=FX+Cos(Richtung/180*Pi)*30;
     FY:=FY-Sin(Richtung/180*Pi)*30;
     if (FX<5) then FX:=5;
     if (FX>=MapWidth-5) then FX:=MapWidth-5;
     if (FY<5) then FY:=5;
     if (FY>=MapHeight-5) then FY:=MapHeight-5;
     LastPlaces[LastPlacesPtr].x:=round(FX);
     LastPlaces[LastPlacesPtr].y:=round(FY);
     LastPlacesPtr:=(LastPlacesPtr mod 5)+1;
    end;
    spBarriere:begin
     SDL_LockSurface(TheScreen);
     for x:=0 to 2 do
      for y:=0 to 2 do begin
       SDL_DrawLine(TheScreen,InMap(round(OldX+Cos((Richtung-90)/180*Pi)*50)+x,dirx),InMap(round(OldY-Sin((Richtung-90)/180*Pi)*50)+y,diry),InMap(round(OldX+Cos((Richtung+90)/180*Pi)*50)+x,dirx),InMap(round(OldY-Sin((Richtung+90)/180*Pi)*50)+y,diry),Color);
       SetItemLine(@TheMap,mapSnake,InMap(round(OldX+Cos((Richtung-90)/180*Pi)*50)+x,dirx),InMap(round(OldY-Sin((Richtung-90)/180*Pi)*50)+y,diry),InMap(round(OldX+Cos((Richtung+90)/180*Pi)*50)+x,dirx),InMap(round(OldY-Sin((Richtung+90)/180*Pi)*50)+y,diry));
      end;
     SDL_UnLockSurface(TheScreen);
    end;
   end;
   dec(SpecialPower);
  end;
  if SpecialPower=1 then begin
   case Special of
    spAcc: begin
     KSpeed:=2;
     KRadius:=3;
    end;
    spCurve: begin
     KSpeed:=2;
     KRadius:=3;
    end;
    spTradePlaces: begin
     for i:=1 to MaxSpieler do
      if schlangenRang[i]>0 then begin
      x:=SchlangenRang[i];
       if (Spieler[x]<>nil) and Spieler[x].Alive and (Spieler[x]<>Self) and(Spieler[x].IstKI or (CompiCount<1)) then begin
        // Swap X/Y
         Cosinus:=Spieler[x].RawX;
         Spieler[x].RawX:=FX;
         FX:=Cosinus;
         Cosinus:=Spieler[x].RawY;
         Spieler[x].RawY:=FY;
         FY:=Cosinus;
        //Swap LastPlaces
         Temp:=Spieler[x].LastPlaces;
         Spieler[x].LastPlaces:=LastPlaces;
         LastPlaces:=Temp;
         y:=Spieler[x].LastPlacesPtr;
         Spieler[x].LastPlacesPtr:=LastPlacesPtr;
         LastPlacesPtr:=y;
        // Swap Direction
         y:=Spieler[x].Richtung;
         Spieler[x].Richtung:=Richtung;
         Richtung:=y;
        // BREAK for
         BREAK;
       end;
      end; 
     end;
   end;
   dec(SpecialPower);
  end;
  if SpecialPower=0 then begin
   case Special of
    spInOut: begin
     StartSpecial;
    end;
   end;
   KSpeed:=2;
   KRadius:=3;
  end;
 end;

 function TSchlange.CollisionCheck(Map:PMap):TMapItem;
 begin
  Result:=CollisionCheckXY(Map,FX,FY,false);
 end;

 function TSchlange.CollisionCheckXY(Map:PMap;wx,wy:real;streng:boolean=false):TMapItem;
 var fx,fy,x,y,i:integer;
 begin
  Result:=mapFree;
  if streng then begin
  for fx:=round(wx) to round(wx)+FWidth do begin
   for fy:=round(wy) to round(wy)+FWidth do begin
    x:=fx;
    y:=fy;
    if (Special=spInOut) and (SpecialCount>0) then begin
     if FX<11 then X:=(SpielFeldWidth)+10-fx;
     if FX>SpielFeldWidth+5 then X:=fx mod (SpielFeldWidth)+10;
     if FY<11 then Y:=(SpielFeldHeight)+10-fy;
     if FY>SpielFeldHeight+5 then Y:=fy mod (SpielFeldHeight)+10;
    end;
    if Map^[x,y]<>mapFree then begin
     Result:=Map^[x,y];
     if Result=mapSnake then begin
      for i:=1 to 5 do
       if
         (LastPlaces[i].x<=x) and
         (LastPlaces[i].y<=y) and
         (LastPlaces[i].x+FWidth>=x) and
         (LastPlaces[i].y+FWidth>=y)
         then
       begin
        Result:=mapFree;
        Break;
       end; // if
     end; //wohl die eigene schlange;
     if Result<>mapFree then Break;
    end; //if not mapFree
   end; // for Y
   if Result<>mapFree then Break;
  end;  //for X  end
  end
  else begin
  for x:=ceil(wx) to floor(wx)+FWidth do begin
   for y:=ceil(wy) to floor(wy)+FWidth do begin
    if Map^[x,y]<>mapFree then begin
     Result:=Map^[x,y];
     if Result=mapSnake then begin
      for i:=1 to 5 do
       if
         (LastPlaces[i].x<=x) and
         (LastPlaces[i].y<=y) and
         (LastPlaces[i].x+FWidth>=x) and
         (LastPlaces[i].y+FWidth>=y)
         then
       begin
        Result:=mapFree;
        Break;
       end; // if
     end; //wohl die eigene schlange;
     if Result<>mapFree then Break;
    end; //if not mapFree
   end; // for Y
   if Result<>mapFree then Break;
  end;  //for X
  end;
 end;


 procedure MakeLoch(Schlange:TSchlange);
 begin
  Schlange.Loch:=LochAbstandMin-Random(LochAbstandMax-LochAbstandMin);
 end;


 function TSchlange.MoveEnd:boolean;
 begin
  LastPlaces[LastPlacesPtr].x:=round(FX);
  LastPlaces[LastPlacesPtr].y:=round(FY);
  LastPlacesPtr:=(LastPlacesPtr mod 5)+1;
  Result:=FLoch;
  FLoch:=true;
  if HatLoecher then begin
   if Loch>1 then dec(Loch)
   else if Loch<-1 then begin inc(Loch); FLoch:=false; end
   else if Loch=1 then Loch:=-LochSize
   else if Loch=-1 then MakeLoch(Self);
  end;
 end;

 function TSchlange.GetOldX:integer;
 begin
  Result:=LastPlaces[LastPlacesPtr].x;
 end;

 function TSchlange.GetOldY:integer;
 begin
  Result:=LastPlaces[LastPlacesPtr].y;
 end;

end.

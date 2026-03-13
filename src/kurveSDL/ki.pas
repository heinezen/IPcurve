unit ki;

interface
uses Schlangen,
     engine,
     maps,
     math;

procedure SteureKI(Schlange:PSchlange);
procedure SteureKIUrs(Schlange:PSchlange);
procedure SteureKIFilE(Schlange:PSchlange);
procedure SteureKIAlg(Schlange:PSchlange);

implementation

procedure SteureKIAlg(Schlange:PSchlange);
begin
 case Schlange^.AIAlg of
  aiHumanoid:SteureKI(schlange);
  aiUrs:SteureKIUrs(schlange);
  aiFilE:SteureKIFilE(schlange);
 end;
end;

procedure Fill(var x:array of integer;const val:integer);
var i:integer;
begin
 for i:=Low(x) to High(x) do
  x[i]:=val;
end;

function Max(a:array of integer):integer;
var i:integer;
begin
 Result:=Low(a);
 for i:=Low(a) to High(a) do
  if a[i]>a[Result] then Result:=i;
end;



procedure SteureKIFilE(Schlange:PSchlange);

const AnzWinkel = 9;
      WinkelGrad:array[0..AnzWinkel-1] of integer = (0,110,-110,60,-60,33,-33,16,-16);
      Limit = 1000;
      Mul = 4;
var Winkel:array[0..AnzWinkel-1] of integer;
    WinkelBlocker:array[0..AnzWinkel-1] of TMapItem;
    i:integer;
    r,cx,cy:real;
    Cosinus,Sinus:extended;
begin
 if Schlange^.AiRest<=0 then begin
  Fill(Winkel,0);
  // winkel messen
  for i:=0 to AnzWinkel-1 do begin
   cx:=Schlange^.RawX;
   cy:=Schlange^.RawY;
   if Schlange^.AIInvertAngles then r:=(((Schlange^.Richtung-WinkelGrad[i])mod 360)/360)*2*Pi
                               else r:=(((Schlange^.Richtung+WinkelGrad[i])mod 360)/360)*2*Pi;
   while (Schlange^.CollisionCheckXY(@TheMap,cx,cy,true)=mapFree)and(Winkel[i]<Limit) do begin
     Math.SinCos(r,Sinus,Cosinus);
     cx:=cx+Cosinus*Mul;
     cy:=cy-Sinus*Mul;
     inc(Winkel[i],Mul);
   end;
   WinkelBlocker[i]:=Schlange^.CollisionCheckXY(@TheMap,cx,cy,true);
  end;
  //spezialregeln
  //winkeltabelle
  // [0]=0,[1]=110,[2]=-110,[3]=60,[4]=-60,[5]=33,[6]=-33,[7]=16,[8]=-16
// if Schlange^.Special=sp


 if Winkel[6]<3 then Winkel[7]:=1;
 if Winkel[5]<3 then Winkel[8]:=1;
 if Winkel[8]<10 then Winkel[4]:=1;
 if Winkel[7]<10 then Winkel[3]:=1;
 if Winkel[5]<25 then Winkel[4]:=1;
 if Winkel[6]<25 then Winkel[3]:=1;
 if Winkel[8]<10 then Winkel[2]:=1;
 if Winkel[7]<10 then Winkel[1]:=1;
 if Winkel[5]<30 then Winkel[2]:=1;
 if Winkel[6]<30 then Winkel[1]:=1;
 if Winkel[3]<44 then Winkel[2]:=1;
 if Winkel[4]<44 then Winkel[1]:=1;
 if Winkel[8]<8 then Winkel[6]:=1;
 if Winkel[7]<8 then Winkel[5]:=1;
 Winkel[1]:=Winkel[1]-40;
 Winkel[2]:=Winkel[2]-40;
 if Schlange^.AiAlt in [2,4,6,8] then begin
  Winkel[1]:=Winkel[1]-350;
  Winkel[3]:=Winkel[3]-180;
  Winkel[5]:=Winkel[5]-30;
  Winkel[7]:=Winkel[7]-15;
 end;

 if Schlange^.AiAlt in [1,3,5,7] then begin
  Winkel[2]:=Winkel[2]-350;
  Winkel[4]:=Winkel[4]-180;
  Winkel[6]:=Winkel[6]-30;
  Winkel[8]:=Winkel[8]-15;
 end;

 if (Winkel[0]>120)and((Winkel[7]>100) or(Winkel[8]>100)) then Winkel[0]:=Winkel[0]+50;
  //auswerten
  i:=Max(Winkel);
  Schlange^.AiAlt:=i;
 if (i=1) or (i=2) then
  Schlange^.AiRest:=3
 else if (i=3) or (i=4) then
  Schlange^.AiRest:=2
 else if (i=5) or (i=6) then
  Schlange^.AiRest:=1
 else if (i=7) or (i=8) then
  Schlange^.AiRest:=0;
 if (Winkel[i]<50) and (Schlange^.Special=spTradePlaces) then
  Schlange^.StartSpecial;
 end
 else begin
  i:=Schlange^.AiAlt;
  dec(Schlange^.AiRest);
 end;

 if i=0 then
  Schlange^.Taste:=keine
 else if (i mod 2)=0 then
  Schlange^.Taste:=rechts
 else
  Schlange^.Taste:=links;
end;


















procedure SteureKI(Schlange:PSchlange);
const AnzWinkel = 9;
      WinkelGrad:array[0..AnzWinkel-1] of integer = (0,110,-110,96,-96,42,-42,18,-18);
      Limit = 1000;
      Mul = 4;
var Winkel:array[0..AnzWinkel-1] of integer;
    i:integer;
    r,cx,cy:real;
    Cosinus,Sinus:extended;
begin
 if Schlange^.AiRest<=0 then begin
  Fill(Winkel,0);
  // winkel messen
  for i:=0 to AnzWinkel-1 do begin
   cx:=Schlange^.RawX;
   cy:=Schlange^.RawY;
   if Schlange^.AIInvertAngles then r:=(((Schlange^.Richtung-WinkelGrad[i])mod 360)/360)*2*Pi
                               else r:=(((Schlange^.Richtung+WinkelGrad[i])mod 360)/360)*2*Pi;
   while (Schlange^.CollisionCheckXY(@TheMap,cx,cy,true)=mapFree)and(Winkel[i]<Limit) do begin
     Math.SinCos(r,Sinus,Cosinus);
     cx:=cx+Cosinus*Mul;
     cy:=cy-Sinus*Mul;
     inc(Winkel[i],Mul);
   end;
  end;
  //spezialregeln

 if Winkel[8]<15 then Winkel[4]:=1;
 if Winkel[7]<15 then Winkel[3]:=1;
 if Winkel[0]<10 then Winkel[4]:=1;;
 if Winkel[0]<10 then Winkel[3]:=1;
 if Winkel[5]<30 then Winkel[4]:=1;
 if Winkel[6]<30 then Winkel[3]:=1;

 if Winkel[8]<50 then Winkel[2]:=1;
 if Winkel[7]<50 then Winkel[1]:=1;
 if Winkel[0]<20 then Winkel[2]:=1;
 if Winkel[0]<20 then Winkel[1]:=1;
 if Winkel[0]>200 then Winkel[2]:=1;
 if Winkel[0]>200 then Winkel[1]:=1;
 if Winkel[5]<60 then Winkel[2]:=1;
 if Winkel[6]<60 then Winkel[1]:=1;
 if Winkel[3]<86 then Winkel[2]:=1;
 if Winkel[4]<86 then Winkel[1]:=1;

 if Winkel[7]<10 then Winkel[6]:=1;
 if Winkel[8]<10 then Winkel[5]:=1;

 if (Winkel[0]>250)and(Winkel[7]>100)and(Winkel[8]>100) then Winkel[0]:=9999;

  //auswerten
  i:=Max(Winkel);
  Schlange^.AiAlt:=i;
  Schlange^.AiRest:=KiWaitZeit;
 end
 else begin
  i:=Schlange^.AiAlt;
  dec(Schlange^.AiRest);
 end;
 if i=0 then
  Schlange^.Taste:=keine
 else if (i mod 2)=0 then
  Schlange^.Taste:=rechts
 else
  Schlange^.Taste:=links;
end;


























procedure SteureKIUrs(Schlange:PSchlange);
const AnzWinkel = 9;
      WinkelGrad:array[0..AnzWinkel-1] of integer = (0,110,-110,60,-60,33,-33,16,-16);
      Limit = 1000;
      Mul = 4;
var Winkel:array[0..AnzWinkel-1] of integer;
    i:integer;
    r,cx,cy:real;
    Cosinus,Sinus:extended;
begin
 if Schlange^.AiRest<=0 then begin
  Fill(Winkel,0);
  // winkel messen
  for i:=0 to AnzWinkel-1 do begin
   cx:=Schlange^.RawX;
   cy:=Schlange^.RawY;
   if Schlange^.AIInvertAngles then r:=(((Schlange^.Richtung-WinkelGrad[i])mod 360)/360)*2*Pi
                               else r:=(((Schlange^.Richtung+WinkelGrad[i])mod 360)/360)*2*Pi;
   while (Schlange^.CollisionCheckXY(@TheMap,cx,cy,true)=mapFree)and(Winkel[i]<Limit) do begin
     Math.SinCos(r,Sinus,Cosinus);
     cx:=cx+Cosinus*Mul;
     cy:=cy-Sinus*Mul;
     inc(Winkel[i],Mul);
   end;
  end;
  //spezialregeln
  //winkeltabelle
  // [0]=0,[1]=110,[2]=-110,[3]=60,[4]=-60,[5]=33,[6]=-33,[7]=16,[8]=-16

 if Winkel[6]<3 then Winkel[7]:=1;
 if Winkel[5]<3 then Winkel[8]:=1;
 if Winkel[8]<10 then Winkel[4]:=1;
 if Winkel[7]<10 then Winkel[3]:=1;
 if Winkel[5]<25 then Winkel[4]:=1;
 if Winkel[6]<25 then Winkel[3]:=1;
 if Winkel[8]<10 then Winkel[2]:=1;
 if Winkel[7]<10 then Winkel[1]:=1;
 if Winkel[5]<30 then Winkel[2]:=1;
 if Winkel[6]<30 then Winkel[1]:=1;
 if Winkel[3]<44 then Winkel[2]:=1;
 if Winkel[4]<44 then Winkel[1]:=1;
 if Winkel[8]<8 then Winkel[6]:=1;
 if Winkel[7]<8 then Winkel[5]:=1;
 Winkel[1]:=Winkel[1]-40;
 Winkel[2]:=Winkel[2]-40;
 if Schlange^.AiAlt in [2,4,6,8] then begin
  Winkel[1]:=Winkel[1]-350;
  Winkel[3]:=Winkel[3]-180;
  Winkel[5]:=Winkel[5]-30;
  Winkel[7]:=Winkel[7]-15;
 end;

 if Schlange^.AiAlt in [1,3,5,7] then begin
  Winkel[2]:=Winkel[2]-350;
  Winkel[4]:=Winkel[4]-180;
  Winkel[6]:=Winkel[6]-30;
  Winkel[8]:=Winkel[8]-15;
 end;

// if Winkel[4]>200 then begin
//  Schlange^.AiAlt:=4;
//  Schlange^.AiRest:=6;
// end;

 if (Winkel[0]>120)and((Winkel[7]>100) or(Winkel[8]>100)) then Winkel[0]:=Winkel[0]+50;

  //auswerten
  i:=Max(Winkel);
  Schlange^.AiAlt:=i;
 if (i=1) or (i=2) then
  Schlange^.AiRest:=3
 else if (i=3) or (i=4) then
  Schlange^.AiRest:=2
 else if (i=5) or (i=6) then
  Schlange^.AiRest:=1
 else if (i=7) or (i=8) then
  Schlange^.AiRest:=0;

 end
 else begin
  i:=Schlange^.AiAlt;
  dec(Schlange^.AiRest);
 end;

 if i=0 then
  Schlange^.Taste:=keine
 else if (i mod 2)=0 then
  Schlange^.Taste:=rechts
 else
  Schlange^.Taste:=links;
end;

end.

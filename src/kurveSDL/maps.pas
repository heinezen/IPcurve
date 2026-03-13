unit maps;

interface
uses constants;

type
 TMapItem = (mapFree,mapSnake,mapWall);
 TMap = array[0..MapWidth,0..MapHeight] of TMapItem;
 PMap = ^TMap;

procedure FillMap(Map:PMap;Item:TMapItem);
procedure SetItemQuad(Map:PMap;Item:TMapItem;x,y,w,h:integer);
procedure SetItem(Map:PMap;Item:TMapItem;x,y:integer);
procedure SetItemLine(map:PMap;item:TMapItem;x1,y1,x2,y2:integer);

implementation

procedure SetItem(Map:PMap;Item:TMapItem;x,y:integer);
begin
 if (x<=MapWidth)and(x>=0)and(y<=MapHeight)and(y>=0) then
  Map^[x,y]:=Item;
end;

procedure SetItemQuad(Map:PMap;Item:TMapItem;x,y,w,h:integer);
var ix,iy:integer;
begin
 for ix:=x to x+w do
  for iy:=y to y+h do
   SetItem(Map,Item,ix,iy);
end;

procedure FillMap(Map:PMap;Item:TMapItem);
var x,y:integer;
begin
 for x:=0 to MapWidth do
  for y:=0 to MapHeight do
   Map^[x,y]:=Item;
end;

procedure SetItemLine(map:PMap;item:TMapItem;x1,y1,x2,y2:integer);
var
  dx, dy, sdx, sdy, x, y, px, py : integer;
begin
  dx := x2 - x1;
  dy := y2 - y1;
  if dx < 0 then
    sdx := -1
  else
    sdx := 1;
  if dy < 0 then
    sdy := -1
  else
    sdy := 1;
  dx := sdx * dx + 1;
  dy := sdy * dy + 1;
  x := 0;
  y := 0;
  px := x1;
  py := y1;
  if dx >= dy then
  begin
    for x := 0 to dx - 1 do
    begin
      SetItem( Map, Item ,px, py);
      y := y + dy;
      if y >= dx then
      begin
        y := y - dx;
        py := py + sdy;
      end;
      px := px + sdx;
    end;
  end
  else
  begin
    for y := 0 to dy - 1 do
    begin
      SetItem( Map, Item ,px, py);
      x := x + dx;
      if x >= dy then
      begin
        x := x - dy;
        px := px + sdx;
      end;
      py := py + sdy;
    end;
  end;
end;


end.

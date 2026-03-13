unit menu;
interface
 uses constants,
      SDL,
      grafik,
      SDLUtils;
 type
 TProc = procedure();
 TSDLComponent=class
  private
   target:PSDL_Surface;
   EnableHighLight:boolean;
   FBounds:TSDL_Rect;
   FVisible:boolean;
   procedure FSetBounds(const value:TSDL_Rect);

   procedure AfterCreate; virtual;
   procedure BeforeDestroy; virtual;
   procedure SetVisible(const Value:boolean);

  public
   OnClick:TProc;
   FHighLighted:boolean;
   Name:String;
   constructor Create(t:PSDL_Surface;x,y,w,h:cardinal;n:string;DoHighlight:boolean=true);
   destructor Destroy; override;
   property Bounds:TSDL_Rect read FBounds write FSetBounds;
   property Visible:boolean read FVisible write SetVisible;
   procedure Paint; virtual; abstract;
   procedure Clear;

   procedure ProcessKeyDown(Event:TSDL_Event); virtual;
   procedure ProcessMouseDown(relx,rely:integer); virtual;

   procedure HighLight; virtual;
   procedure LowLight; virtual;
   function IsInBounds(x,y:integer):boolean;
 end;

 TSDLTextComponent=class(TSDLComponent)
  private
   FFontColor:TSDL_Color;
   FText:string;
   procedure SetFontColor(const Value:TSDL_Color);
   procedure SetText(const Value:string);
  public
   property FontColor:TSDL_Color read FFontColor write SetFontColor;
   property Text:string read FText write SetText;
 end;

 TSDLTextButton=class(TSDLTextComponent)
  private
   Content:PSDL_Surface;
   ContentHL:PSDL_Surface;
   procedure AfterCreate;override;
   procedure BeforeDestroy;override;
  public
   procedure Paint; override;
   procedure HighLight; override;
   procedure LowLight; override;
 end;

 TSDLImageButton=class(TSDLComponent)
  private
   Contents: array of PSDL_Surface;
   DispImg:integer;
   procedure AfterCreate;override;
   procedure BeforeDestroy;override;
   procedure SetImage(const Value:PSDL_Surface);
   procedure SetDisp(const Value:Integer);
  public
   property DispImage:integer read DispImg write SetDisp;
   property Image:PSDL_Surface write SetImage;
   procedure Paint; override;
   procedure HighLight; override;
   procedure LowLight; override;
   procedure ProcessMouseDown(relx,rely:integer); override;
 end;


 TSDLEdit=class(TSDLTextButton)
  private
   procedure AfterCreate;override;
  public
   procedure ProcessKeyDown(Event:TSDL_Event); override;
 end;

 TSDLKeyEdit=class(TSDLEdit)
  public
   KeyVal:integer;
   procedure ProcessKeyDown(Event:TSDL_Event); override;
 end;

 TSDLCheckBox=class(TSDLTextButton)
  private
   FChecked:boolean;
   Yes:PSDL_Surface;
   YesHL:PSDL_Surface;
   No:PSDL_Surface;
   NoHL:PSDL_Surface;
   procedure AfterCreate;override;
   procedure BeforeDestroy;override;
   procedure SetChecked(const Value:boolean);
  public
   procedure Paint; override;
   property Checked:boolean read FChecked write SetChecked default false;
 end;

 TSDLValueSetting = class(TSDLTextButton)
  private
   FProgress:Integer;
   FFillColor:TSDL_Color;
   procedure AfterCreate;override;
   procedure SetFillColor(const Value:TSDL_Color);
  public
   procedure ProcessMouseDown(relx,rely:integer); override;
   property FillColor:TSDL_Color read FFillColor write SetFillColor;
   property Progress:integer read FProgress write FProgress;
 end;

 TSDLCursor = class
  private
   FImage:PSDL_Surface;
   FBack:PSDL_Surface;
   FTarget:PSDL_Surface;
   FX,FY:integer;
   Changed:Boolean;
   procedure SetX(const Value:integer);
   procedure SetY(const Value:integer);
  public
   constructor Create(Img:string;trg:PSDL_Surface;ix,iy:integer);
   destructor Destroy; override;
   property X:Integer read FX write SetX default 0;
   property Y:Integer read FY write SetY default 0;
   procedure Clear;
   procedure GrabBG;
   procedure Paint(force:boolean=false);
 end;

 TSDLComponentOwner=class
  private
   ComponentList:array of TSDLComponent;
   TheCursor:TSDLCursor;
  public
   constructor Create(Cur:string;targ:PSDL_Surface;mx,my:integer);
   destructor Destroy; override;
   function FindComponent(Name:string):TSDLComponent;
   procedure OnCreate; virtual;
   function AddComponent(Component:TSDLComponent):integer;
   procedure Paint(force:boolean=false); virtual;
 end;

implementation
uses engine;
 (* --------------------------------- *)
 (*              TSDLCursor           *)
 (* --------------------------------- *)
  procedure TSDLCursor.SetX(const Value:integer);
//  var Rect1,Rect2:TSDL_Rect;
  begin
   if Value<>FX then begin
//    Rect1:=SDLRect(0,0,FImage.w,FImage.h);
//    Rect2:=SDLRect(FX,FY,FImage.w,FImage.h);
{    SDL_BlitSurface(FBack,
                    @Rect1,
                    FTarget,
                    @Rect2);}
    Clear;
    FX:=Value;
    Changed:=true;
    Paint;
   end;
  end;

  procedure TSDLCursor.SetY(const Value:integer);
//  var Rect1,Rect2:TSDL_Rect;
  begin
   if Value<>FY then begin
//    Rect1:=SDLRect(0,0,FImage.w,FImage.h);
//    Rect2:=SDLRect(FX,FY,FImage.w,FImage.h);
{    SDL_BlitSurface(FBack,
                    @Rect1,
                    FTarget,
                    @Rect2);}
    Clear;
    FY:=Value;
    Changed:=true;
    Paint;
   end;
  end;

  destructor TSDLCursor.Destroy;
  begin
   SDL_FreeSurface(FImage);
   SDL_FreeSurface(FBack);
  end;

  constructor TSDLCursor.Create;
  var TempSurface:PSDL_Surface;
  begin
   TempSurface:=SDL_LoadBMP(PChar(Img));
   FImage:=SDL_CreateRGBSurface(SDL_HWSURFACE or SDL_HWACCEL,TempSurface.w,TempSurface.h,16,$00,$00,$00,$00);
   FBack:=SDL_CreateRGBSurface(SDL_HWSURFACE or SDL_HWACCEL,TempSurface.w,TempSurface.h,16,$00,$00,$00,$00);
   SDL_BlitSurface(TempSurface,nil,FImage,nil);
   SDL_FreeSurface(TempSurface);
   FTarget:=trg;
   FX:=ix;
   FY:=iy;
   SDL_SetColorKey(FImage,SDL_SRCCOLORKEY or SDL_RLEACCEL,$0000);
   Changed:=true;
   GrabBG;
   Paint;
  end;

  procedure TSDLCursor.Clear;
  var Rect1,Rect2:TSDL_Rect;
  begin
     Rect1:=SDLRect(0,0,FImage.w,FImage.h);
     Rect2:=SDLRect(FX,FY,FImage.w,FImage.h);
     SDL_BlitSurface(FBack,
                     @Rect1,
                     FTarget,
                     @Rect2);
  end;

  procedure TSDLCursor.GrabBG;
  var Rect1,Rect2:TSDL_Rect;
  begin
    Rect1:=SDLRect(0,0,FImage.w,FImage.h);
    Rect2:=SDLRect(FX,FY,FImage.w,FImage.h);
    SDL_BlitSurface(FTarget,
                    @Rect2,
                    FBack,
                    @Rect1);
  end;


  procedure TSDLCursor.Paint;
  var Rect1,Rect2:TSDL_Rect;
  begin
   if Changed or Force then begin
    Rect1:=SDLRect(0,0,FImage.w,FImage.h);
    Rect2:=SDLRect(FX,FY,FImage.w,FImage.h);
    if (FTarget<>nil) and (FImage<>nil) and (FBack<>nil) then begin
     if force then
      SDL_BlitSurface(FBack,
                      @Rect1,
                      FTarget,
                      @Rect2);

     SDL_BlitSurface(FTarget,
                     @Rect2,
                     FBack,
                     @Rect1);
     SDL_BlitSurface(FImage,
                     @Rect1,
                     FTarget,
                     @Rect2);
    end;
    changed:=false;
   end;
  end;

 (* --------------------------------- *)
 (*            TSDLComponent          *)
 (* --------------------------------- *)
 procedure TSDLComponent.AfterCreate;
 begin

 end;

 procedure TSDLComponent.BeforeDestroy;
 begin

 end;

 procedure TSDLComponent.HighLight;
 begin
 end;

 procedure TSDLComponent.ProcessMouseDown(relx,rely:integer);
 begin
 end;

 procedure TSDLComponent.ProcessKeyDown(Event:TSDL_Event);
 begin
 end;

 procedure TSDLComponent.LowLight;
 begin
 end;

 procedure TSDLComponent.Clear;
 begin
  SDL_FillRect(target,@FBounds,$0000);
 end;

 procedure TSDLComponent.SetVisible;
 begin
  FVisible:=Value;
  if FVisible then Paint
  else Clear;
 end;

 constructor TSDLComponent.Create;
 begin
  inherited Create;
  target:=t;
  FBounds.x:=x;
  FBounds.y:=y;
  FBounds.w:=w;
  FBounds.h:=h;
  Name:=n;
  FVisible:=true;
  EnableHighlight:=DoHighLight;
  AfterCreate;
  Paint;
 end;

 destructor TSDLComponent.Destroy;
 begin
  BeforeDestroy;
  Clear;
  inherited Destroy;
 end;

 procedure TSDLComponent.FSetBounds;
 begin
  FBounds:=Value;
  Paint;
 end;

 function TSDLComponent.IsInBounds(x,y:integer):boolean;
 begin
  Result:= (x>FBounds.x) and (y>FBounds.y) and
           (x<FBounds.x+FBounds.w) and
           (y<FBounds.y+FBounds.h);
 end;

 (* --------------------------------- *)
 (*        TSDLTextComponent          *)
 (* --------------------------------- *)

 procedure TSDLTextComponent.SetFontColor(const Value:TSDL_Color);
 begin
  FFontColor:=Value;
  AfterCreate;
  Paint;
 end;

 procedure TSDLTextComponent.SetText(const Value:string);
 begin
  FText:=Value;
  AfterCreate;
  Paint;
 end;

 (* --------------------------------- *)
 (*           TSDLTextButton          *)
 (* --------------------------------- *)

 procedure TSDLTextButton.AfterCreate;
 begin
  if Content<>nil then
   SDL_FreeSurface(Content);
  if ContentHL<>nil then
   SDL_FreeSurface(ContentHL);
  Content:=SDL_CreateRGBSurface(SDL_SWSURFACE,FBounds.w,FBounds.h,16,$00,$00,$00,$00);
  ContentHL:=SDL_CreateRGBSurface(SDL_SWSURFACE,FBounds.w,FBounds.h,16,$00,$00,$00,$00);
  TextOut(FText,ContentHL,0,0,FBounds.h,GameFont,FFontColor.r+50,FFontColor.g+50,FFontColor.b+50);
  TextOut(FText,Content,0,0,FBounds.h,GameFont,FFontColor.r,FFontColor.g,FFontColor.b);
 end;

 procedure TSDLTextButton.BeforeDestroy;
 begin
  if Content<>nil then
   SDL_FreeSurface(Content);
  if ContentHL<>nil then
   SDL_FreeSurface(ContentHL);
 end;


 procedure TSDLTextButton.HighLight;
 begin
  FHighLighted:=true;
 end;

 procedure TSDLTextButton.LowLight;
 begin
  FHighLighted:=false;
 end;

 procedure TSDLTextButton.Paint;
 var rect:TSDL_Rect;
 begin
  if FVisible then begin
   rect:=SDLRect(0,0,FBounds.w,FBounds.h);
   if FHighLighted and EnableHighLight then
    SDL_BlitSurface(ContentHL,@rect,target,@FBounds)
   else
    SDL_BlitSurface(Content,@rect,target,@FBounds);
  end;
 end;

 (* --------------------------------- *)
 (*           TSDLImageButton         *)
 (* --------------------------------- *)

 procedure TSDLImageButton.SetImage;
 var x:integer;
     Rect:TSDL_Rect;
 begin
  DispImg:=0;
  if Length(Contents)>0 then
   for x:=0 to Length(Contents)-1 do
    if Contents[x]<>nil then
     SDL_FreeSurface(Contents[x]);
   SetLength(Contents,0);
   for x:=0 to (Value.h div FBounds.h) do begin
    SetLength(Contents,x+1);
    Contents[x]:=SDL_CreateRGBSurface(SDL_SWSURFACE,FBounds.w,FBounds.h,16,$00,$00,$00,$00);
    Rect:=SDLRect(0,x*FBounds.h,FBounds.w,FBounds.h);
    SDL_BlitSurface(Value,@Rect,Contents[x],nil);
   end;
  Paint; 
 end;

 procedure TSDLImageButton.AfterCreate;
 begin
  DispImg:=0;
 end;

 procedure TSDLImageButton.BeforeDestroy;
 var x:integer;
 begin
 if Length(Contents)>0 then
  for x:=0 to Length(Contents)-1 do
   if Contents[x]<>nil then
    SDL_FreeSurface(Contents[x]);
  SetLength(Contents,0);
 end;


 procedure TSDLImageButton.HighLight;
 begin
  FHighLighted:=true;
 end;

 procedure TSDLImageButton.ProcessMouseDown;
 begin
  DispImg:=(DispImg+1) mod (Length(Contents)-1);
  Paint;
 end;

 procedure TSDLImageButton.SetDisp;
 begin
  DispImg:=Value;
  Paint;
 end;


 procedure TSDLImageButton.LowLight;
 begin
  FHighLighted:=false;
 end;

 procedure TSDLImageButton.Paint;
 var rect:TSDL_Rect;
 begin
  if FVisible then begin
   rect:=SDLRect(0,0,FBounds.w,FBounds.h);
   if Length(Contents)>DispImg then
    SDL_BlitSurface(Contents[DispImg],@rect,target,@FBounds);
  end;
 end;


 (* --------------------------------- *)
 (*         TSDLValueSetting          *)
 (* --------------------------------- *)
 procedure TSDLValueSetting.ProcessMouseDown(relx,rely:integer);
 begin
  if (rely<=(FBounds.h-4)) and (relx<=(FBounds.w-4)) then begin
   FProgress:=round(((relx)*100)/(FBounds.w-4));
   AfterCreate;
  end;
 end;

 procedure TSDLValueSetting.SetFillColor(const Value:TSDL_Color);
 begin
  FFillColor:=Value;
  AfterCreate;
 end;

 procedure TSDLValueSetting.AfterCreate;
 var Rect:TSDL_Rect;
 begin
  Rect:=SDLRect(2,
                2,
                Round(FProgress*((FBounds.w-4)/100)),
                (FBounds.h-4));
  if Content<>nil then
   SDL_FreeSurface(Content);
  if ContentHL<>nil then
   SDL_FreeSurface(ContentHL);
  Content:=SDL_CreateRGBSurface(SDL_SWSURFACE,FBounds.w,FBounds.h,16,$00,$00,$00,$00);
  ContentHL:=SDL_CreateRGBSurface(SDL_SWSURFACE,FBounds.w,FBounds.h,16,$00,$00,$00,$00);
  SDL_GradientFillRect(Content,nil,SDLColor(0,0,50),SDLColor(0,0,200),gsVertical);
  SDL_GradientFillRect(ContentHL,nil,SDLColor(0,0,100),SDLColor(0,0,255),gsVertical);
  SDL_FillRect(Content,@Rect,SDL_MapRGB(target.format,FFillColor.r,FFillColor.g,FFillColor.b));
  SDL_FillRect(ContentHL,@Rect,SDL_MapRGB(target.format,FFillColor.r+50,FFillColor.g+50,FFillColor.b+50));
  TextOut(FText,ContentHL,0,0,FBounds.h,GameFont,FFontColor.r+50,FFontColor.g+50,FFontColor.b+50);
  TextOut(FText,Content,0,0,FBounds.h,GameFont,FFontColor.r,FFontColor.g,FFontColor.b);
 end;

 (* --------------------------------- *)
 (*            TSDLCheckBox           *)
 (* --------------------------------- *)

procedure TSDLCheckBox.AfterCreate;
begin
 inherited AfterCreate;
  if Yes<>nil then
   SDL_FreeSurface(Yes);
  if YesHL<>nil then
   SDL_FreeSurface(YesHL);
  if No<>nil then
   SDL_FreeSurface(No);
  if NoHL<>nil then
   SDL_FreeSurface(NoHL);
  Yes:=SDL_CreateRGBSurface(SDL_SWSURFACE,100,FBounds.h,16,$00,$00,$00,$00);
   TextOut(GetText('Ja'),Yes,0,0,FBounds.h,GameFont,FFontColor.r,FFontColor.g,FFontColor.b);
  YesHL:=SDL_CreateRGBSurface(SDL_SWSURFACE,100,FBounds.h,16,$00,$00,$00,$00);
   TextOut(GetText('Ja'),YesHL,0,0,FBounds.h,GameFont,FFontColor.r+50,FFontColor.g+50,FFontColor.b+50);
  No:=SDL_CreateRGBSurface(SDL_SWSURFACE,100,FBounds.h,16,$00,$00,$00,$00);
   TextOut(GetText('Nein'),No,0,0,FBounds.h,GameFont,FFontColor.r,FFontColor.g,FFontColor.b);
  NoHL:=SDL_CreateRGBSurface(SDL_SWSURFACE,100,FBounds.h,16,$00,$00,$00,$00);
   TextOut(GetText('Nein'),NoHL,0,0,FBounds.h,GameFont,FFontColor.r+50,FFontColor.g+50,FFontColor.b+50);
end;

procedure TSDLCheckBox.BeforeDestroy;
begin
 inherited BeforeDestroy;
  if Yes<>nil then
   SDL_FreeSurface(Yes);
  if YesHL<>nil then
   SDL_FreeSurface(YesHL);
  if No<>nil then
   SDL_FreeSurface(No);
  if NoHL<>nil then
   SDL_FreeSurface(NoHL);
end;

procedure TSDLCheckBox.SetChecked(const Value:boolean);
begin
 FChecked:=Value;
 Paint;
end;

procedure TSDLCheckBox.Paint;
var Rect:TSDL_Rect;
    Rect2:TSDL_Rect;
begin
 if FVisible then begin
  inherited Paint;
  Rect.x:=FBounds.x+FBounds.w+25;
  Rect.y:=FBounds.y;
  Rect.h:=FBounds.h;
  Rect.w:=80;
  Rect2.x:=0;
  Rect2.y:=0;
  Rect2.h:=FBounds.h;
  Rect2.w:=80;
  if FChecked then begin
   if FHighLighted and EnableHighLight then
    SDL_BlitSurface(YesHL,@rect2,target,@rect)
   else
    SDL_BlitSurface(Yes,@rect2,target,@rect);
  end
  else begin
   if FHighLighted and EnableHighLight then
    SDL_BlitSurface(NoHL,@rect2,target,@rect)
   else
    SDL_BlitSurface(No,@rect2,target,@rect);
  end;
 end;
end;

 (* --------------------------------- *)
 (*              TSDLEdit             *)
 (* --------------------------------- *)


procedure TSDLEdit.ProcessKeyDown;
begin
 if FHighlighted then begin
  if (event.key.keysym.sym=SDLK_BACKSPACE) then begin
   if (Length(FText)>0) then
    SetText(Copy(FText,1,Length(FText)-1))
  end else if event.key.keysym.sym>31 then begin
   if event.key.keysym.modifier = 2 then
    SetText(FText+UpCase(Chr(event.key.keysym.sym)))
   else
    SetText(FText+Chr(event.key.keysym.sym));
  end;
 end;
end;

procedure TSDLEdit.AfterCreate;
begin
  if Content<>nil then
   SDL_FreeSurface(Content);
  if ContentHL<>nil then
   SDL_FreeSurface(ContentHL);
  Content:=SDL_CreateRGBSurface(SDL_SWSURFACE,FBounds.w,FBounds.h,16,$00,$00,$00,$00);
  ContentHL:=SDL_CreateRGBSurface(SDL_SWSURFACE,FBounds.w,FBounds.h,16,$00,$00,$00,$00);
  SDL_FillRect(Content,nil,$000F);
  SDL_FillRect(ContentHL,nil,$001F);
  TextOut(FText,ContentHL,2,2,FBounds.h-2,GameFont,FFontColor.r+50,FFontColor.g+50,FFontColor.b+50);
  TextOut(FText,Content,2,2,FBounds.h-2,GameFont,FFontColor.r,FFontColor.g,FFontColor.b);
end;




 (* --------------------------------- *)
 (*            TSDLKeyEdit            *)
 (* --------------------------------- *)


procedure TSDLKeyEdit.ProcessKeyDown;
begin
 if FHighlighted then begin
  SetText(SDL_GetKeyName(event.key.keysym.sym));
  KeyVal:=event.key.keysym.sym;
 end;
end;





 (* --------------------------------- *)
 (*         TSDLComponentOwner        *)
 (* --------------------------------- *)


procedure TSDLComponentOwner.OnCreate;
begin

end;

constructor TSDLComponentOwner.Create;
begin
 inherited Create;
 TheCursor:=TSDLCursor.Create(Cur,targ,mx,my);
 SetLength(ComponentList,0);
 OnCreate;
 Paint(true);
end;

function TSDLComponentOwner.FindComponent(Name:string):TSDLComponent;
var x:integer;
begin
 Result:=nil;
 for x:=0 to Length(ComponentList)-1 do
  if ComponentList[x].Name=Name then begin
   Result:=ComponentList[x];
   Break;
  end;
end;

function TSDLComponentOwner.AddComponent(Component:TSDLComponent):integer;
begin
 SetLength(ComponentList,Length(ComponentList)+1);
 ComponentList[Length(ComponentList)-1]:=Component;
 Result:=Length(ComponentList)-1;
end;

destructor TSDLComponentOwner.Destroy;
var x:integer;
begin
 for x:=0 to Length(ComponentList)-1 do
  ComponentList[x].Free;
 SetLength(ComponentList,0);
 TheCursor.Free;
end;

procedure TSDLComponentOwner.Paint;
var x:integer;
    ev:array of TSDL_Event;
begin
 if force then
  MouseY:=-MouseY;
 if (TheCursor.X<>MouseX)or(TheCursor.Y<>MouseY) then begin
  if MouseY>0 then begin
   TheCursor.X:=MouseX;
   TheCursor.Y:=MouseY;
  end;
//  for x:=0 to Length(ComponentList)-1 do
//   ComponentList[x].Paint;
  if MouseY<0 then begin
   MouseY:=-MouseY;
   TheCursor.Paint(true);
  end;
  TheCursor.Paint;
 end;
 SetLength(ev,1);
 while SDL_PollEvent(@ev[Length(ev)-1])<>0 do begin
  case ev[Length(ev)-1].type_ of
    SDL_MOUSEMOTION:begin
                     for x:=0 to Length(ComponentList)-1 do
                      with ComponentList[x] do begin
                       if IsInBounds(ev[Length(ev)-1].motion.x,ev[Length(ev)-1].motion.y) then begin
                        if not ComponentList[x].FHighLighted then begin
                         HighLight;
                         TheCursor.Clear;
                         ComponentList[x].Paint;
                         TheCursor.GrabBG;
                        end;
                       end
                       else begin
                        if ComponentList[x].FHighLighted then begin
                         LowLight;
                         TheCursor.Clear;
                         ComponentList[x].Paint;
                         TheCursor.GrabBG;
                        end;
                       end;
                      end;
                      MouseX:=ev[Length(ev)-1].motion.x;
                      if ev[Length(ev)-1].motion.y>60 then
                       MouseY:=ev[Length(ev)-1].motion.y;
                    end;
    SDL_MOUSEBUTTONDOWN:begin
                     for x:=0 to Length(ComponentList)-1 do
                      with ComponentList[x] do begin
                       if IsInBounds(ev[Length(ev)-1].button.x,ev[Length(ev)-1].button.y) then begin
                        ProcessMouseDown(ev[Length(ev)-1].button.x-ComponentList[x].Bounds.x,ev[Length(ev)-1].button.y-ComponentList[x].Bounds.y);
                        MouseY:=-MouseY;
                        if @OnClick<>nil then
                         OnClick;
                        if ComponentList[x]<>nil then begin
                         TheCursor.Clear;
                         ComponentList[x].Paint;
                         TheCursor.GrabBG;
                        end;
                       end;
                      end;
                    end;
    SDL_KEYDOWN:begin
                     for x:=0 to Length(ComponentList)-1 do
                      with ComponentList[x] do begin
                       if IsInBounds(MouseX,abs(MouseY)) then begin
                         ProcessKeyDown(ev[Length(ev)-1]);
                         TheCursor.Clear;
                         ComponentList[x].Paint;
                         TheCursor.GrabBG;
                       end;
                      end;
                    end;
  end;
  SetLength(ev,Length(ev)+1);
 end;

 if Length(ev)>1 then begin
  for x:=0 to Length(ev)-2 do
   SDL_PushEvent(@ev[x]);
 end;
 SetLength(ev,0);
end;

end.

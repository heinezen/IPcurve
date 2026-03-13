unit constants;

interface
uses SDL;
const
// fonts
{$IFDEF MSWINDOWS}
  GameFont = '.\curve.ttf';
{$ELSE}
  GameFont = './curve.ttf';
{$ENDIF}
{$IFDEF MSWINDOWS}
  CursorBMP = '.\cursor.bmp';
{$ELSE}
  CursorBMP = './cursor.bmp';
{$ENDIF}
// Spielfeld-Metrik
 MapWidth = 1024;
 MapHeight = 768;
 SpielfeldWidth = 900;
 SpielfeldHeight = 748;
 ScoreHeight = 50;
 SpecialHeight = 20;
// Grafik
 Fullscreen = true;
 CatchMouse = true;
// Maxima
 MaxSpieler = 7;
 MaxEvents  = 20; // maximaler event-stack
// Spieler
 MausSteuerung = 7;
// Standart-Einstellungen
 DoAnimTextStd = false;
 FarbDefaults:array[1..MaxSpieler] of Word=
  ($E000,$DF00,$FB00,$FFFF,$8878,$2545,$3456);
 TastenDefaults:array[1..MaxSpieler,1..3] of Word=
  ((SDLK_1,SDLK_q,SDLK_A),(SDLK_LCTRL,SDLK_LALT,SDLK_LESS),(SDLK_M,SDLK_COMMA,SDLK_RSHIFT),
   (SDLK_4,SDLK_5,SDLK_6),(SDLK_PAGEDOWN,SDLK_PAGEUP,SDLK_HOME),
   (SDLK_LEFT,SDLK_DOWN,SDLK_RIGHT),($0,$0,$0));
 LochAbstandMaxStd = 400;
 LochAbstandMinStd = 300;
 LochSizeStd = 6;
 LoecherStd = true;
 GameSpeedStd = 15;
 BlinkZeitStd = 50;
 KIWaitZeitStd = 1;
 CalcStepSizeStd = 20;

// Debug
{$IFDEF MSWINDOWS}
  LogFile = '.\kurvesdl.log';
{$ELSE}
  LogFile = './kurvesdl.log';
{$ENDIF}


// GUI
 TitelText = 'kurveSDL 0.6 BETA';
 LocalTitelText = 'Lokales Spiel';
 LocalSiegerText = 'Ausgekurvt';
 LanHostText = 'LAN Host BETA';
 LanClientText = 'LAN Client BETA';
 LanClientConnectText = 'Verbinden mit Host BETA';
 SettingsText = 'kurveSDL Einstellungen';
 ReadyText = 'Bereit';
 KIText = 'KI';

// TCP/IP
 MaxConnections = 20;
 function GetText(text:string):string;
 type TLang=(de,en);
 var lang:TLang=de;
implementation

function GetText(text:string):string;
begin
 Result:=text;
 if lang=en then begin
  if text=LocalTitelText then
   Result:='Local Game'
  else if text=LocalSiegerText then
   Result:='Game Over'
  else if text=LANClientConnectText then
   Result:='Connect to HOST'
  else if text=SettingsText then
   Result:='kurveSDL Settings'
  else if text=ReadyText then
   Result:='READY'
  else if text=KIText then
   Result:='bot'
  else if text='Einstellungen' then
   Result:='Settings'
  else if text='LAN Spiel Hosten' then
   Result:='Host a LAN Game'
  else if text='LAN Spiel Betreten' then
   Result:='Join a LAN Game'
  else if text='Beenden' then
   Result:='Quit'
  else if text='Links' then
   Result:='Left'
  else if text='Ja' then
   Result:='YES'
  else if text='Nein' then
   Result:='NO'
  else if text='Rechts' then
   Result:='Right'
  else if text='RMaus' then
   Result:='RMouse'
  else if text='MidMaus' then
   Result:='MidMouse'
  else if text='LMaus' then
   Result:='LMouse'
  else if text='SpielSpeed' then
   Result:='Game Speed'
  else if text='BlinkZeit' then
   Result:='Blinking Time'
  else if text='HoleAbstand Min' then
   Result:='HoleDistance Min'
  else if text='HoleAbstand Toleranz' then
   Result:='HoleDistance Random'
  else if text='Animierte Titel' then
   Result:='Animated Titles'
  else if text='Fertig' then
   Result:='Done'
  else if text='Kontrollieren' then
   Result:='Overtake'
  else if text='Spiel Starten' then
   Result:='StartGame'
  else if text='Abbrechen' then
   Result:='Cancel'
  else if text='Verbindung' then
   Result:='Connection'
  else if text='Kontrolliert Spieler:' then
   Result:='Controls Players: '
  else if text='English Please' then
   Result:='Deutsch, bitte'
  else if text='ist jetzt ein KI' then
   Result:='is now a BOT'
  else if text='ist jetzt kein KI mehr' then
   Result:='is no longer a BOT'
  else Result:=text;
 end;
end;

end.

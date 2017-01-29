#ifndef CONFIG_H
#define CONFIG_H
#include <SDL/SDL.h>

#define DEFAULT_PORT 8080
#define INFO_PORT 7777
#define LOOKUP_PORT 7778
#define METASERVER_PORT 7779
#define MAX_CONNECTIONS 20
#define MAX_PLAYERS 9
#define VERSION 0xF050

// multiply transforms degrees to radians
#define RAD 0.0174533

#define DEFAULT_METASERVER "ozone.game-host.org"

#define MAX_NICK_LENGTH 20

#define METASERVERS_CONNECTIONS 50
#define METASERVERS_TIMEOUT 50

#define SERVER_IN_LIMIT 0
#define SERVER_OUT_LIMIT 0
#define SERVER_TIMEOUT 20

#define CLIENT_IN_LIMIT 0
#define CLIENT_OUT_LIMIT 0
#define CLIENT_CONNECTIONS 1
#define CLIENT_MAX_HOSTS 42
#define CLIENT_TIMEOUT 120

#define CONNECT_TIMEOUT 5000
#define LOOKUP_TIMEOUT 500
#define METASERVER_TIMEOUT 1000
#define DISCONNECT_TIMEOUT 200

#define FONT_TITLEFONT "curve.ttf"
#define FONT_TITLESIZE 64
#define FONT_NORMALFONT "curve.ttf"
#define FONT_NORMALSIZE 24
#define FONT_TINYFONT "curve.ttf"
#define FONT_TINYSIZE 18
#define FONT_MENUFONT "curve.ttf"
#define FONT_MENUSIZE 28
#define FONT_MENUBUTTONFONT "curve.ttf"
#define FONT_MENUBUTTONSIZE 34
#define RENDERGAMEFONT(a,b,c) TTF_RenderText_Solid(a,b,c)
#define RENDERMENUFONT(a,b,c) TTF_RenderText_Blended(a,b,c)

#define CHECKBOX_IMAGE_FILE "checkbox.dat"

#define SCREENSHOT_IMAGE_FILE "ipcurve.bmp"

#define CONFIG_FILE "ipcurve.cfg"

#define SCREENWIDTH 1024
#define SCREENHEIGHT 768
#define SCREENBPP 16

#define SCOREBOARDWIDTH 50
#define SCOREOFFSET 100

#define MESSAGEBOARDWIDTH 20
#define MESSAGEOFFSET 37

#define ABORT_KEY SDLK_ESCAPE
#define START_KEY SDLK_SPACE
#define CHAT_KEY SDLK_RETURN
#define SCREENSHOT_KEY SDLK_PRINT

// keys to be ORed
#define KEY_LEFT 1 
#define KEY_RIGHT 2
#define KEY_SPECIAL 4

#define WANTED_DELAY_STD 50
#define STDSPEED 3

#define LANGUAGE_GERMAN 0
#define LANGUAGE_ENGLISH 1

const SDLKey stdkeys[MAX_PLAYERS][3] = {{SDLK_1,SDLK_2,SDLK_3},
                                        {SDLK_x,SDLK_c,SDLK_v},
                                        {SDLK_m,SDLK_COMMA,SDLK_PERIOD},
                                        {SDLK_LEFT,SDLK_DOWN,SDLK_RIGHT},
                                        {SDLK_F9,SDLK_F10,SDLK_F11},
                                        {SDLK_f,SDLK_g,SDLK_h},
                                        {SDLK_u,SDLK_i,SDLK_o},
                                        {SDLK_4,SDLK_5,SDLK_6},
										{SDLK_WORLD_43,SDLK_WORLD_45,SDLK_WORLD_44}};

const unsigned char stdcolors[MAX_PLAYERS][3] = {{0xFF,0x00,0x00},
                                                 {0xFF,0xFF,0x00},
                                                 {0xFF,0x7F,0x00},
                                                 {0x00,0xFF,0x00},
                                                 {0xFF,0x00,0xFF},
                                                 {0xFF,0xFF,0xFF},
                                                 {0x00,0xFF,0xFF},
                                                 {0x7F,0x7F,0xFF},
																 {0xFF,0x7F,0x7F}/*,
																 {0x7F,0xFF,0x7F}*/};

#define STDHOLESIZE 15
#define STDHOLEDIST 25000
#define STDHOLEDISTRND 10000

#define ABILITY_NONE 0
#define ABILITY_WALLS 1
#define ABILITY_ACCELERATE 2
#define ABILITY_RADIUS 3
#define ABILITY_MAKEHOLE 4
#define ABILITY_JUMP 5
#define ABILITY_BARRIER 6
#define ABILITY_NOHOLE 7
#define ABILITY_90DEGS 8
#define ABILITY_WIDTH 9
#define ABILITY_INVISIBLE 10
#define ABILITY_BOTSCHOICE 42

#define ABI_ACCEL_LENGTH 15000
#define ABI_RADIUS_LENGTH 3333
#define ABI_MAKEHOLE_PERIOD 1555
#define ABI_MAKEHOLE_LENGTH 300
#define ABI_JUMP_PERIOD 1555
#define ABI_JUMP_LENGTH 33
#define ABI_BARRIER_PERIOD 999
#define ABI_BARRIER_LENGTH 50
#define ABI_90DEGS_LENGTH 25000
#define ABI_WIDTH_LENGTH 7500
#define ABI_WIDTH_MAXWIDTH 15
#define ABI_INVISIBLE_LENGTH 5000

#define SPECIALS_IMAGE_FILE "specials.dat"

#define BOTALG_NOBOT 0
#define BOTALG_NORMAL 1
#define BOTALG_ENHANCED 2
#define BOTALG_FORCEFIELD 3
#define BOTALG_DUMB 4
#define BOTALG_MAX BOTALG_DUMB

#define STDFIELDWIDTH 907
#define STDFIELDHEIGHT 710

#define MAXFIELDWIDTH STDFIELDWIDTH
#define MAXFIELDHEIGHT STDFIELDHEIGHT

#endif

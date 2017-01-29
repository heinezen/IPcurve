GCC = g++
WINFLAGS = -O3 -ffast-math -lmingw32 -lSDLmain -lSDL -lSDL_ttf -lenet -lws2_32 -lwinmm -mwindows
LINFLAGS = -O3 -ffast-math -lSDL -lSDL_ttf -lenet
DEBUGFLAGS = -ggdb -lSDL -lSDL_ttf -lenet
CXXFLAGS = -O3 -ffast-math

FILES= main.o \
       client.o \
       server.o \
       snake.o \
       game.o \
       player.o \
       clientgui.o \
       menu.o \
       gamemenus.o \
       settings.o \
       text.o \
       metaserver.o
TARGET = ipcurve

all: $(FILES)
	$(GCC) -o $(TARGET) $(FILES) $(LINFLAGS)

debug: $(FILES)
	$(GCC) -o $(TARGET) $(FILES) $(DEBUGFLAGS)

win32: $(FILES)
	$(GCC) -o $(TARGET) $(FILES) $(WINFLAGS)

clean:
	rm -rf *.o *~

main.o: main.cpp clientgui.h config.h settings.h client.h message.h \
  server.h player.h gamemenus.h menu.h text.h
client.o: client.cpp client.h config.h clientgui.h settings.h message.h
server.o: server.cpp server.h config.h player.h message.h settings.h \
  game.h snake.h text.h
snake.o: snake.cpp snake.h player.h config.h server.h message.h \
  settings.h game.h
game.o: game.cpp game.h server.h config.h player.h message.h settings.h \
  snake.h text.h
player.o: player.cpp player.h config.h
clientgui.o: clientgui.cpp config.h text.h client.h clientgui.h \
  settings.h message.h
menu.o: menu.cpp config.h menu.h text.h
gamemenus.o: gamemenus.cpp config.h settings.h menu.h text.h clientgui.h \
  client.h message.h server.h player.h gamemenus.h
settings.o: settings.cpp config.h settings.h
text.o: text.cpp
metaserver.o: metaserver.cpp config.h metaserver.h

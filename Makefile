CC = gcc
CXX = g++
DEBUG = -ggdb
LFLAGS = -Wall -Werror
CFLAGS = $(LFLAGS) $(DEBUG) -c
CXXFLAGS = $(LFLAGS) $(DEBUG) -c
LDFLAGS = -lncurses
OBJS = game.o dungeon.o Player.o queue.o display.o

default: game

game.o: game.cpp game.h Player.h queue.h Cell_Pair.h
	$(CXX) $(CFLAGS) game.cpp
	
queue.o: queue.c queue.h
	$(CXX) $(CFLAGS) queue.c

display.o: display.cpp display.h Cell_Pair.h
	$(CXX) $(CFLAGS) display.cpp
	
dungeon.o: dungeon.cpp game.h Player.h queue.h Cell_Pair.h
	$(CXX) $(CFLAGS) dungeon.cpp
	
Player.o: Player.cpp game.h Player.h queue.h Cell_Pair.h
	$(CXX) $(CXXFLAGS) Player.cpp
	
game: $(OBJS)
	$(CXX) $(LFLAGS) $(DEBUG) $(OBJS) -o game $(LDFLAGS)

clean:
	rm -f a.out cpgame game *.o

cpp: $(OBJS)
	$(CXX) $(LDFLAGS) $(LFLAGS) $(DEBUG) $(OBJS) -o cpgame

CC = gcc
CXX = g++
DEBUG = -ggdb
LFLAGS = -Wall -Werror
CFLAGS = $(LFLAGS) $(DEBUG) -c
CXXFLAGS = $(LFLAGS) $(DEBUG) -c
LDFLAGS = -lncurses
OBJS = game.o dungeon.o Player.o queue.o

default: game

game.o: game.c game.h Player.h queue.h
	$(CC) $(CFLAGS) game.c
	
queue.o: queue.c queue.h
	$(CC) $(CFLAGS) queue.c
	
dungeon.o: dungeon.c game.h Player.h queue.h
	$(CC) $(CFLAGS) dungeon.c
	
Player.o: Player.cpp game.h Player.h queue.h
	$(CXX) $(CXXFLAGS) Player.cpp
	
game: $(OBJS)
	$(CC) $(LDFLAGS) $(LFLAGS) $(DEBUG) $(OBJS) -o game

clean:
	rm -f a.out cpgame game *.o

cpp: $(OBJS)
	$(CXX) $(LDFLAGS) $(LFLAGS) $(DEBUG) $(OBJS) -o cpgame
CC = gcc
CXX = g++
DEBUG = -ggdb
LFLAGS = -Wall -Werror
CFLAGS = $(LFLAGS) $(DEBUG) -c
CXXFLAGS = $(LFLAGS) $(DEBUG) -c
LDFLAGS = -lncurses
OBJS = game.o dungeon.o Character.o queue.o display.o Monster.o Player.o monster_parser.o

default: game

game.o: game.cpp game.h Character.h queue.h Cell_Pair.h
	$(CXX) $(CFLAGS) game.cpp
	
queue.o: queue.c queue.h
	$(CXX) $(CFLAGS) queue.c

monster_parser.o: monster_parser.cpp monster_parser.h
	$(CXX) $(CFLAGS) monster_parser.cpp

display.o: display.cpp display.h Cell_Pair.h
	$(CXX) $(CFLAGS) display.cpp
	
dungeon.o: dungeon.cpp game.h Character.h queue.h Cell_Pair.h
	$(CXX) $(CFLAGS) dungeon.cpp
	
Character.o: Character.cpp game.h Character.h queue.h Cell_Pair.h
	$(CXX) $(CXXFLAGS) Character.cpp

Player.o: Player.cpp game.h Player.h queue.h Cell_Pair.h
	$(CXX) $(CXXFLAGS) Player.cpp

Monster.o: Monster.cpp Character.cpp Monster.h game.h Character.h queue.h Cell_Pair.h
	$(CXX) $(CXXFLAGS) Monster.cpp
	
game: $(OBJS)
	$(CXX) $(LFLAGS) $(DEBUG) $(OBJS) -o game $(LDFLAGS)

clean:
	rm -f a.out cpgame game *.o

cpp: $(OBJS)
	$(CXX) $(LDFLAGS) $(LFLAGS) $(DEBUG) $(OBJS) -o cpgame

CC = gcc
CXX = g++
DEBUG = -ggdb
LFLAGS = -Wall -Werror
CFLAGS = $(LFLAGS) $(DEBUG) -c
CXXFLAGS = $(LFLAGS) $(DEBUG) -c -std=c++11
LDFLAGS = -lncurses
OBJS = game.o dungeon.o Character.o queue.o display.o Monster.o Player.o object_parser.o Item.o

default: game

game.o: game.cpp game.h Character.h queue.h Cell_Pair.h
	$(CXX) $(CXXFLAGS) game.cpp
	
queue.o: queue.cpp queue.h
	$(CXX) $(CXXFLAGS) queue.cpp

Item.o: Item.cpp Item.h
	$(CXX) $(CXXFLAGS) Item.cpp

object_parser.o: object_parser.cpp object_parser.h
	$(CXX) $(CXXFLAGS) object_parser.cpp

display.o: display.cpp display.h Cell_Pair.h
	$(CXX) $(CXXFLAGS) display.cpp
	
dungeon.o: dungeon.cpp dungeon.h game.h Character.h queue.h Cell_Pair.h
	$(CXX) $(CXXFLAGS) dungeon.cpp
	
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

default: game

game:	game.c queue.c game.h queue.h dungeon.c
	gcc -o game game.c queue.c dungeon.c -lncurses -Wall -Werror -ggdb

clean:
	rm -f a.out game cpgame

fast:	game.c queue.c game.h queue.h dungeon.c
	gcc -o game game.c queue.c dungeon.c -lncurses -Wall -ggdb

cpp:	game.c queue.c game.h queue.h dungeon.c
	g++ -lncurses -o cpgame game.c queue.c dungeon.c  -Wall -Werror -ggdb
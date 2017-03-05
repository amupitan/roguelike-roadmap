default: game

game:	game.c queue.c game.h queue.h
	gcc -o game game.c queue.c -lncurses -Wall -Werror -ggdb

clean:
	rm -f a.out game cpgame

fast:	game.c queue.c game.h queue.h
	gcc -o game game.c queue.c -Wall -ggdb

cpp:	game.c queue.c game.h queue.h
	g++ -lncurses -o cpgame game.c queue.c  -Wall -Werror -ggdb
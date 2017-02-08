default: game

game:	game.c queue.c game.h queue.h
	gcc -o game game.c queue.c -Wall -Werror -ggdb

clean:
	rm -f a.out game

fast:	game.c queue.c game.h queue.h
	gcc -o game game.c queue.c -Wall -ggdb

ogame:	game.c
	gcc -o game game.c -Wall -Werror -ggdb


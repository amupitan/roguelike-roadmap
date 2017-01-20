default: game

game:	game.c
	gcc -o game game.c -Wall -Werror -ggdb

clean:
	rm -f a.out game

fast:	game.c
	gcc -o game game.c -Wall -ggdb

# Roguelike Roadmap
##This game is based on the rogue-like map and is implemented in C
The game is in file game.c and has the following specifications:
* The game is run with --load, --save and --seed arguments
* --load loads a file from {HOME}/.rlg327/dungeon, --save saves a dungeon to that location
* --load takes an optional argument to load dungeon from a different path e.g. --load="dungeons/dungeon" 
* --seed takes a required argument of a seed number e.g. --seed=10
* All data read and written is in big endian format
* The game generates a random dungeon which can be seeded using an integer command line argument
* The dungeon is 160 units wide and 105 units long
* There is a minimum of 10 rooms
* Each room is at least 7 units wide and 5 units long
* The outermost cells of the dungeon are always rock
* Room cells are identified by periods, corridor cells with hashes and rock with whitespace
* The dungeon is fully connected i.e you can get to any room from everyroom
* Corridors do not overlay rooms
* C strucutres are sometimes used to identify cells in the game


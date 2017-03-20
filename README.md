# Roguelike Roadmap
## This game is based on the rogue-like map and is implemented in C
The game is in file game.c, a queue that was used in implemting the game is in file queue.c and the game has the following specifications:
* The game is run with --load, --save, --seed, --solo, --pc and --nummon arguments
* --load loads a file from {HOME}/.rlg327/dungeon, --save saves a dungeon to that location
* --load takes an optional argument to load dungeon from a different path e.g. --load="dungeons/dungeon"
* --seed takes a required argument of a seed number e.g. --seed=10
* --solo takes no argumtnts. When set, the game continues as long as the PC is alive, irrespective of the number of monsters
* --pc takes an argument in form of co-ordinates that should be valid within the dungeon which are separated by a comma with x coming first, e.g --pc="65,43"
* --nummon takes an integer argument which determines the number of monsters e.g. --nummon=10 causes the dungeon to have 10 monsters, else a random number of monsters are generated(the number of random monsters generated is >= number of rooms and <= two times the number of rooms
* If no argument is passed in for --pc then the PC is placed in a random position in a random room
* All specifications in the pdf are followed including:
  * Display only 80x21 portion of the screen ignoring the top line and last two lines
  * The dungeon is displayed every PC turn
  * There is a look mode ('L' button) that can be used to navigate the dungeon without using a turn or moving the PC
  * All button specifications in the buttons section are implemented including the following:
    - KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT -> to navigate the dungeon accordingly
    - 'S' -> displays the seed on the top line
    - 'R' -> displays the number of rooms and co-ordinates of the PC
* The game continues to run until the PC is dead or all the monsters are dead. The monsters attempt to kill the PC depending on their abilities
* There are 16 distinct types of monsters which are named from 0-f (using the hex), which are also color-coded
* The game runs using a discrete event simulator which is implemented using a priority queue
* The queue is prioritized based on turn and speed of a monster, monsters' speed randomly ranges from 5 - 20, the PC has a speed of 10
* The priorities are calculated using 1000/speed. Monsters move around the dungeon to the Pc, if two monsters cross each other, the new one kills the old one
* The gradients for the non-tunneling monsters were calculated using breadth first search algorithm implemented with a queue
* The gradients for the tunneling monsters were calculated using djkistra's algorithm implemented with a priority queue
* All data read and written is in big endian format
* The game generates a random dungeon which can be seeded using an integer command line argument
* The dungeon is 160 units wide and 105 units long
* There is a minimum of 10 rooms, each room is at least 7 units wide and 5 units long
* The outermost cells of the dungeon are always rock
* Room cells are identified by periods, corridor cells with hashes, rock with whitespace and the PC with a green colored '@' character
* The dungeon is fully connected i.e you can get to any room from everyroom
* Corridors do not overlay rooms
* C strucutres are used to identify cells and charcters in the game
* game.c and queue.c both have header files game.h and game.c respectively
* There is a fog which makes the PC see only a raduis of 5 units
* Whenever a portion of the dungeon is seen, it remains seen
* The PC only remembers what it sees so it updates its only when the PC is within a range of 5 units to the updated unit


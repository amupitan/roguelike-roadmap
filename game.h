#ifndef GAME_H
#define GAME_H

# ifdef __cplusplus
extern "C" {
# endif

#include <stdint.h>

#define nRows 105
#define nCols 160

typedef struct Cell{
	int x;
	int y;
	char value;
	unsigned char hardness;
}Cell;

typedef struct Room{
  int x;
  int y;
  uint8_t width;
  uint8_t height;
}Room;

typedef struct Dungeon{
  Room* rooms;
  int num_rooms;
}Dungeon;


int create_room(Cell map[][nCols], int x, int y, uint8_t* width, uint8_t* height);
int rand_gen(int min, int max);
int connect_rooms(Cell map[][nCols], Cell p, Cell q);
int update_cell(Cell* p, char value, unsigned char hardness);
void render(Cell map[][nCols], int x, int y);
int write_room(Cell map[][nCols], Room room);
int add_room(Dungeon* rlg, Room room);

int cell_equals(void* c1, void* c2);

# ifdef __cplusplus
}
# endif

#endif
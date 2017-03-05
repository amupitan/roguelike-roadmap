#ifndef GAME_H
#define GAME_H

# ifdef __cplusplus
extern "C" {
# endif

#include <stdint.h>

#define nRows 105
#define nCols 160

/*TODO Get rid of these?*/
#define PC "\x1B[32m@\x1B[0m"
#define RESET "\x1B[0m"

typedef struct Pair{
  int x;
  int y;
} Pair;

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

typedef struct Player{
  int x;
  int y;
  char value;
  uint8_t type:4;
  int: 4;
  uint8_t speed; //can use :5?
  uint8_t id; //int?
} Player;

struct event {
  unsigned int time;
  Player being;
  // enum event_type type;
  // union {
  //   pc_t *pc;
  //   npc_t *npc;
  //   trap_t *trap;
  //   effect_t *effect;
  // } p;
};

int create_room(Cell map[][nCols], int x, int y, uint8_t* width, uint8_t* height);
int rand_gen(int min, int max);
int connect_rooms(Cell map[][nCols], Cell p, Cell q);
int update_cell(Cell* p, char value, unsigned char hardness);
void render(Cell map[][nCols], int x, int y);
int write_room(Cell map[][nCols], Room room);
int add_room(Dungeon* rlg, Room room);
void render_dungeon(Cell map[][nCols], int chars[][nCols], Player monsts[]);
int getRoom(Dungeon d, int x, int y);
Pair determine_position(Room room);

int cell_equals(void* c1, void* c2);
void BFS_impl(int dist[][nCols], Cell map[][nCols], Queue* q, Cell pc);
void Djikstra_impl(int t_dist[][nCols], Cell map[][nCols], Queue* q, Cell pc);

void print_player(void* player);
int char_equals(void* c1, void* c2);

Pair* getInputC(Pair* target);
int getInputL(Pair* curr);
void render_partial(Cell map[][nCols], int chars[][nCols], Player monsts[], Pair start);

/*ncurses*/
void ncurses_init();
void printmon(Player player);
void nrender_dungeon(Cell map[][nCols], int chars[][nCols], Player monsts[]);
void endgame(Dungeon* dungeon, Queue* game_queue, char* endmessage);

# ifdef __cplusplus
}
# endif

#endif
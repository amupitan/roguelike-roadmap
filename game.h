#ifndef GAME_H
#define GAME_H

#include "Player.h" /*TODO remove?*/
#include <stdint.h>
#include "Cell_Pair.h"

# ifdef __cplusplus
extern "C" {
# endif

#define nRows 105
#define nCols 160

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
void render_dungeon(Cell map[][nCols], int chars[][nCols], Player* monsts[]);
int getRoom(Dungeon d, int x, int y);
Pair determine_position(Room room);
void create_dungeon(Dungeon* dungeon, Cell map[][nCols], Cell room_cells[]);
void load_dungeon(FILE* dungeon_file, Dungeon* dungeon, Cell map[][nCols], char* dungeon_title, uint32_t* version, uint32_t* size_dungeon_file);

int cell_equals(void* c1, void* c2);
void BFS_impl(int dist[][nCols], Cell map[][nCols], Queue* q, Cell pc);
void Djikstra_impl(int t_dist[][nCols], Cell map[][nCols], Queue* q, Cell pc);

void print_player(void* player);
int char_equals(void* c1, void* c2);
Player* pc_init(Player* pc, Room room);

Pair* getInputC(Pair* target);
void render_partial(Cell map[][nCols], int chars[][nCols], Player* monsts[], Pair start, Pair* newPos);
Pair* look_mode(Pair *target, int* control_mode);
void addCharcters(Dungeon* dungeon, Queue* evt, int nummon, Player* characters[], int chars[][nCols], unsigned int pace[]);
void delete_dungeon(Dungeon* dungeon, Queue* evt, Cell map[][nCols]);
void add_stairs(Dungeon* dungeon, Cell map[][nCols]);
void delete_players(Player* characters[], int num_characters);
void updateSight(Player* pc, Cell map[][nCols]);

/*ncurses*/
void ncurses_init();
void printmon(Player* player);
void log_message(const char* message);
void nrender_dungeon(Cell map[][nCols], int chars[][nCols], Player* monsts[]);

void pc_render_partial(Cell map[][nCols], int chars[][nCols], Player* monsts[], Pair start, Pair* newPos);
void endgame(Dungeon* dungeon, Queue* game_queue, const char* endmessage);

# ifdef __cplusplus
}
# endif

#endif

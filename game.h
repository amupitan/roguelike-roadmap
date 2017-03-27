#ifndef GAME_H
#define GAME_H

#include "Player.h" /*TODO remove?*/
#include <stdint.h>
#include <queue>

#include "Cell_Pair.h"
#include "dungeon.h"

# ifdef __cplusplus
extern "C" {
# endif

// typedef void Dungeon;

#define nRows 105
#define nCols 160

int rand_gen(int min, int max);

int cell_equals(void* c1, void* c2);

void print_player(void* player);
int char_equals(void* c1, void* c2);

// void delete_dungeon(Dungeon* dungeon,std::priority_queue<Player, std::vector<Player> >* evt, Cell map[][nCols]);
// void delete_dungeon(Dungeon* dungeon, Queue* evt, Cell map[][nCols]);

void delete_players(Player* characters[], int num_characters);

// void endgame(Dungeon* dungeon, std::priority_queue<Player, std::vector<Player> >* game_queue, const char* endmessage);
// void endgame(Dungeon* dungeon, std::priority_queue<Player, std::vector<Player> >* game_queue, const char* endmessage);
void endgame(Dungeon* dungeon, Queue* game_queue, const char* endmessage);

// void clear_queue(std::priority_queue<Player, std::vector<Player>, std::greater<Player>>* q);

# ifdef __cplusplus
}
# endif

#endif

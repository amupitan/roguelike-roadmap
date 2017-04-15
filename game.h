#ifndef GAME_H
#define GAME_H

#include "Character.h" /*TODO remove?*/
#include <stdint.h>
#include <queue>

#include "Cell_Pair.h"
#include "dungeon.h" //TODO find a way to remove

# ifdef __cplusplus
extern "C" {
# endif

// typedef void Dungeon;

#define nRows 105
#define nCols 160

int rand_gen(int min, int max);

int cell_equals(void* c1, void* c2);

void print_Character(void* Character);
int char_equals(void* c1, void* c2);

// void delete_dungeon(Dungeon* dungeon,std::priority_queue<Character, std::vector<Character> >* evt, Cell map[][nCols]);
// void delete_dungeon(Dungeon* dungeon, Queue* evt, Cell map[][nCols]);
bool probability(int chance);
void delete_Characters(Character* characters[], int num_characters);
Pair moveCharacter(Character* curr, const Pair& target, int chars[][nCols]);
// void endgame(Dungeon* dungeon, std::priority_queue<Character, std::vector<Character> >* game_queue, const char* endmessage);
// void endgame(Dungeon* dungeon, std::priority_queue<Character, std::vector<Character> >* game_queue, const char* endmessage);
void endgame(Dungeon* dungeon, Queue* game_queue, const char* endmessage);

// void clear_queue(std::priority_queue<Character, std::vector<Character>, std::greater<Character>>* q);

# ifdef __cplusplus
}
# endif

#endif

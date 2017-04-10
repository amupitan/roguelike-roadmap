#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "Cell_Pair.h"
#include "Player.h"
# ifdef __cplusplus
void log_message(std::string message, int row); //TODO: remove extern C
extern "C" {
# endif

void ncurses_init();
Pair* getInputC(Pair* target);
Pair* look_mode(Pair *target, int* control_mode);
void log_message(const char* message);
void print_inventory(Item **items);
void display_equipment(Item ** items);
int drop_from_inventory(Item ** items);
int wear_equipment(Item ** items);
int take_off_equipment(Item ** items);
int expunge_from_inventory(Item ** items);

# ifdef __cplusplus
}
# endif

#endif

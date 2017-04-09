#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "Cell_Pair.h"
#include "Player.h"
# ifdef __cplusplus
extern "C" {
# endif

void ncurses_init();
Pair* getInputC(Pair* target);
Pair* look_mode(Pair *target, int* control_mode);
void log_message(const char* message);
void print_inventory(Item **items);

# ifdef __cplusplus
}
# endif

#endif

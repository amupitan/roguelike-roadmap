#ifndef DISPLAY_H
#define DISPLAY_H

#include "Player.h" /*TODO remove?*/
#include <stdint.h>
#include "Cell_Pair.h"
# ifdef __cplusplus
extern "C" {
# endif

void ncurses_init();
Pair* getInputC(Pair* target);
Pair* look_mode(Pair *target, int* control_mode);
void log_message(const char* message);

# ifdef __cplusplus
}
# endif

#endif

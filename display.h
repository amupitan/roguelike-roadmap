#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "Cell_Pair.h"
#include "Player.h"
# ifdef __cplusplus
void log_message(std::string message, int row); //TODO: remove extern
void log_message(std::string message, int row, bool left);
void print_inventory(std::vector<Item*>& items);
void display_equipment(std::vector<Item*>& items);
void print_store(std::vector<Item*>& items);
int generic_prompt(std::vector<Item*>& items, const char* prompt, int offset, int max, void (*printer)(std::vector<Item*>& items));
void item_printer(std::vector<Item*>& items, int size, const char* format, int ascii_offset, const char* message);
extern "C" {
# endif

void ncurses_init();
Pair* getInputC(Pair* target);
Pair* look_mode(Pair *target, int* control_mode);
void log_message(const char* message);
void print_inventory(Item **items);
void display_equipment(Item ** items);
int item_info(Item* item, const char* exit_prompt);
int generic_prompt(Item** items, const char* prompt, int offset, int max, void (*printer)(Item ** items));
void display_stats();

# ifdef __cplusplus
}
# endif

#endif

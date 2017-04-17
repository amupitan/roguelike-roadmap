#include <string.h>
#include <ncurses.h>
#include "display.h"
#include "object_parser.h"
#include "Monster.h"

void ncurses_init(){
  initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* To get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
	start_color();			/* Start color 			*/
	/*Initialize colors*/
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);
}


Pair& getInputS(Pair* target){
  switch(getch()){
    case 'y':
    case '7':
      /*upper-left*/
      target->y--;
      target->x--;
      break;
    case 'k':
    case '8':
    case KEY_UP: /*TODO remove?*/
      /*up*/
      target->y--;
      break;
    case 'u':
    case '9':
      /*upper-right*/
      target->y--;
      target->x++;
      break;
    case 'l':
    case '6':
    case KEY_RIGHT: /*TODO remove?*/
      /*right*/
      target->x++;
      break;
    case 'n':
    case '3':
      /*lower-right*/
      target->y++;
      target->x++;
      break;
    case 'j':
    case '2':
    case KEY_DOWN: /*TODO remove?*/
      /*down*/
      target->y++;
      break;
    case 'b':
    case '1':
      /*lower-left*/
      target->x--;
      target->y++;
      break;
    case 'h':
    case '4':
    case KEY_LEFT: /*TODO remove?*/
      /*left*/
      target->x--;
      break;
    case ' ':
    case '5':
      /*select*/
      target->x = target->y = -10;
      break;
        case 27:
        case 'Q':
        case 'q':
      /*Escape*/
      target->x = target->y = -11;
      break;
    default:
      target->x = target->y = -12;
      break;
  }
  return *target;
}

/*Get input in control mode*/
Pair* getInputC(Pair* target){ /*TODO: make void?*/
  // int pos = getch();
  switch(getch()){
    case 'y':
    case '7':
      /*upper-left*/
      target->y--;
      target->x--;
      break;
    case 'k':
    case '8':
    case KEY_UP: /*TODO remove?*/
      /*up*/
      target->y--;
      break;
    case 'u':
    case '9':
      /*upper-right*/
      target->y--;
      target->x++;
      break;
    case 'l':
    case '6':
    case KEY_RIGHT: /*TODO remove?*/
      /*right*/
      target->x++;
      break;
    case 'n':
    case '3':
      /*lower-right*/
      target->y++;
      target->x++;
      break;
    case 'j':
    case '2':
    case KEY_DOWN: /*TODO remove?*/
      /*down*/
      target->y++;
      break;
    case 'b':
    case '1':
      /*lower-left*/
      target->x--;
      target->y++;
      break;
    case 'h':
    case '4':
    case KEY_LEFT: /*TODO remove?*/
      /*left*/
      target->x--;
      break;
    case ' ':
    case '5':
      /*rest*/
      break;
    case '>':
      /*attempt to go downstairs*/
      target->x = target->y = -3;
      break;
    case '<':
      /*attempt to go upstairs*/
      target->x = target->y = -4;
      break;
    case 'L':
      /*Enter look mode*/
      target->x = target->y = -2;
      break;
    case 'Q':
      /*Quit the game*/
      target->x = target->y = -1;
      break;
    case 'S':
      /*Display seed*/
      target->x = target->y = -5;
      break;
    case 'R':
      /*Display number of room*/
      target->x = target->y = -6;
      break;
    case 'i':
      /*display inventory*/
      target->x = target->y = -7;
      break;
    case 'd':
      /*drop item from inventory*/
      target->x = target->y = -8;
      break;
    case 'P':
      /*view PC stats*/
      target->x = target->y = -9;
      break;
    case 'T':
      /*toggle fullscreen*/
      target->x = target->y = -11;
      break;
    case 'w':
      /*wear an item*/
      target->x = target->y = -12;
      break;
    case 'e':
      /*display equipment*/
      target->x = target->y = -13;
      break;
    case 't':
      /*take off equipment*/
      target->x = target->y = -14;
      break;
    case 'x':
      /*expunge inventory item*/
      target->x = target->y = -15;
      break;
    case 'I':
      /*display item descriptiom*/
      target->x = target->y = -16;
      break;
    case 'M':
      /*Go to merchant*/
      target->x = target->y = -17;
      break;
    case '^':
      /*Shoot ranged weapon*/
      target->x = target->y = -18;
      break;
    default:
      target->x = target->y = -10;
      break;
  }
  return target;
}

int generic_prompt(Item** items, const char* prompt, int offset, int max, void (*printer)(Item ** items)){
  printer(items);
  log_message(std::string("PC Inventory: type the index of the item to be ") + prompt + " or press ESC to go back", 0);
  do{
    int select = getch();
    if (select == 27) break; //ESC
    select -= offset;
    if (select >=0 && select < max && items[select]){
      log_message((std::to_string(select) + std::string(") ") + std::string(items[select]->getName()) + std::string(" has been ") + prompt).c_str(), 0);
      return select;
    }else{
      std::string pick;
      pick.push_back(select + offset);
      log_message(pick + std::string(" is invalid. Select a valid number or press ESC to quit"), 0);
    }
  }while(1);
  return -1;
}

Pair* look_mode(Pair *target, int* control_mode){ //TODO: uses hardcoded width/height
  switch(getch()){
    case 'k':
    case '8':
    case KEY_UP: /*TODO remove?*/
      /*move up*/
      target->y -= 10;
      break;
    case 'l':
    case '6':
    case KEY_RIGHT: /*TODO remove?*/
      /*move right*/
      target->x += 40;
      break;
    case 'j':
    case '2':
    case KEY_DOWN: /*TODO remove?*/
      /*move down*/
      target->y += 10;
      break;
    case 'h':
    case '4':
    case KEY_LEFT: /*TODO remove?*/
      /*move left*/
      target->x -= 40;
      break;
    case 'Q':
      /*Quit the game*/
      ungetch('Q');
    case 27:/*ESC*/
      /*go back to control mode*/
      *control_mode = 1;
      break;
    default:
      break;
  }
  return target;
}

void log_message(std::string message, int row){
  int col, temp;
  move(row, 0);
  clrtoeol();
  getmaxyx(stdscr, temp, col);
  mvprintw(row + (temp * 0), (col - message.length())/2, message.c_str()); //0/row is to avoid unused var warning b row
  /*TODO: call refresh()?*/
}

void log_message(const char* message){
  log_message(message, 0);
}

void item_printer(Item ** items, int size, const char* format, int ascii_offset, const char* message){ //legacy
  clear();
  int i, itm_idx, col, num = (22 - 10)/2;
  log_message(message);
  getmaxyx(stdscr, itm_idx, col);
  for (i = num, itm_idx = 0; i < num + size; i++, itm_idx++){
    std::string str = std::string("%") + format + ") ";
    if (items[itm_idx]) {
      str += std::string("(") + items[itm_idx]->getType() + std::string(") ") + std::string(items[itm_idx]->getName());
    }else if (strncmp(message, "PC Equipment", strlen(message)) == 0) 
      str += std::string("(") + object_parser::private_wrapper::get_item_type((itm_idx == 11) ? 8 : itm_idx) + ") ";
    mvprintw(i, (80- 15)/2, str.c_str(), (i - num + ascii_offset));
  }
  refresh();
}

void print_inventory(Item ** items){
  item_printer(items, 10, "d", 0, "PC Inventory");
}

void display_equipment(Item ** items){
  item_printer(items, 12, "c", 97, "PC Equipment");
}

int item_info(Item* item, const char* exit_prompt){
  clear();
  int x_offset = 10, x_initial = x_offset;
  int y_offset = 10;
  mvprintw(y_offset++, x_offset, (std::string("NAME: ") + item->getName()).c_str());
  mvprintw(y_offset++, x_offset, "DESCRIPTION:");
  x_offset = ++x_initial;
  const char* desc = item->getDesc();
  for (unsigned int i = 0; i < strlen(desc); i++){
    if (desc[i] == '\n') {
      y_offset++;
      x_offset = x_initial;
      if (!desc[++i]) break;
    }
    mvaddch(y_offset, x_offset++, desc[i]);
  }
  x_offset = --x_initial;
  mvprintw(y_offset, x_offset, "SYMBOL: ");
  x_offset += 8;
  attron(COLOR_PAIR(item->getColor()));
  mvaddch(y_offset++, x_offset, item->getSymbol());
  attroff(COLOR_PAIR(item->getColor()));
  x_offset = x_initial;
  mvprintw(y_offset++, x_offset, (std::string("TYPE: ") + item->getType()).c_str());
  mvprintw(y_offset++, x_offset, (std::string("DAMAGE: ") + item->getDamage()).c_str());
  mvprintw(y_offset++, x_offset, (std::string("DEFENSE: ") + std::to_string(item->getDefenseBonus())).c_str());
  mvprintw(y_offset++, x_offset, (std::string("SPEED: ") + std::to_string(item->getSpeedBonus())).c_str());
  mvprintw(y_offset++, x_offset, (std::string("HIT: ") + std::to_string(item->getHit())).c_str());
  mvprintw(y_offset++, x_offset, (std::string("DODGE: ") + std::to_string(item->getDodge())).c_str());
  mvprintw(y_offset++, x_offset, (std::string("WEIGHT: ") + std::to_string(item->getWeight())).c_str());
  int x = item->getValue();
  mvprintw(y_offset++, x_offset, (std::string("VALUE: $") + std::to_string(x)).c_str());
  log_message(exit_prompt, 0);
  refresh();
  return getch();/*true if ESC is not pressed*/
}

void display_stats(){
  clear();
  int y_offset = 10;
  log_message("PC", y_offset++);
  log_message("Bio: Managed to survive CS 327. Now a master of C++ and in a quest to find the Software Bug, destroy the Hardware bug and save the C++ Program", y_offset++);
  log_message(std::string("HP: ") + std::to_string(Player::getPlayer()->getHp()), y_offset++);
  log_message(std::string("Speed: ") + std::to_string(Player::getPlayer()->getSpeed()), y_offset++);
  log_message(std::string("Possible damage: ") + std::to_string(Player::getPlayer()->attack()), y_offset++);
  Item** items = Player::getPlayer()->equipment();
  int def = 0;
  for (int i = 0; i < 12; i++){
    // if (items[i]) def += items[i]->getDefenseBonus();
    if (items[i]) def += (items[i]->getDefenseBonus() * ((items[i]->getDodge() + 100.0)/100.0));
  }
  log_message(std::string("Defense: ") + std::to_string(def), y_offset++);
  log_message(std::string("Weight: ") + std::to_string(Player::getPlayer()->getWeight()) + "/" + std::to_string(Player::getPlayer()->getMaxWeight()), y_offset++);
  log_message(std::string("Pesos: ") + std::to_string(Player::getPlayer()->getPesos()), y_offset++);
}

void character_info(Character* character){
  if (character->getId() != 0){
    Monster* monst = static_cast<Monster*>(character);
    clear();
    int y_offset = 10;
    log_message(monst->getName(), y_offset++);
    /*mvprintw(y_offset, x_offset, "SYMBOL: ");
    x_offset += 8;
    attron(COLOR_PAIR(item->getColor()));
    mvaddch(y_offset++, x_offset, item->getSymbol());
    attroff(COLOR_PAIR(item->getColor()));*/
    log_message(monst->getDesc(), y_offset++);
    log_message(std::string("HP: ") + std::to_string(monst->getHp()), y_offset++);
    log_message(std::string("Speed: ") + std::to_string(monst->getSpeed()), y_offset++);
    log_message(std::string("Possible damage: ") + std::to_string(monst->attack()), y_offset++);
    // Item** items = Player::getPlayer()->equipment();
    // int def = 0;
    // for (int i = 0; i < 12; i++){
      // if (items[i]) def += items[i]->getDefenseBonus();
      // if (items[i]) def += (items[i]->getDefenseBonus() * ((items[i]->getDodge() + 100.0)/100.0));
    // }
    // log_message(std::string("Defense: ") + std::to_string(def), y_offset++);
    // log_message(std::string("Weight: ") + std::to_string(Player::getPlayer()->getWeight()) + "/" + std::to_string(Player::getPlayer()->getMaxWeight()), y_offset++);
    // log_message(std::string("Pesos: ") + std::to_string(Player::getPlayer()->getPesos()), y_offset++);
  }else display_stats();
  refresh();
}

void log_message(std::string message, int row, bool left){
  int col, max_row;
  move(row, 0);
  clrtoeol();
  getmaxyx(stdscr, max_row, col);
  // int start  = left ? 0 : col - message.length();
  mvprintw(row + 0/max_row, left ? 0 : col - message.length(), message.c_str()); //max_row is only used because getmaxyx requires all three variables
  /*TODO: call refresh()?*/
}

/*Conversion*/
void print_inventory(std::vector<Item*>& items){
  item_printer(items, 10, "d", 0, "PC Inventory");
}
void display_equipment(std::vector<Item*>& items){
  item_printer(items, 12, "c", 97, "PC Equipment");
}
int generic_prompt(std::vector<Item*>& items, const char* prompt, int offset, int max, void (*printer)(std::vector<Item*>& items)){
  printer(items);
  log_message(std::string("PC Inventory: type the index of the item to be ") + prompt + " or press ESC to go back", 0);
  do{
    int select = getch();
    if (select == 27) break; //ESC
    select -= offset;
    if (select >=0 && select < max && items[select]){
      log_message((std::to_string(select) + std::string(") ") + std::string(items[select]->getName()) + std::string(" has been ") + prompt).c_str(), 0);
      return select;
    }else{
      std::string pick;
      pick.push_back(select + offset);
      log_message(pick + std::string(" is invalid. Select a valid number or press ESC to quit"), 0);
    }
  }while(1);
  return -1;
}
void item_printer(std::vector<Item*>& items, int size, const char* format, int ascii_offset, const char* message){ //legacy
  clear();
  int i, itm_idx, col, num = (22 - 10)/2;
  log_message(message);
  getmaxyx(stdscr, itm_idx, col);
  for (i = num, itm_idx = 0; i < num + size; i++, itm_idx++){
    std::string str = std::string("%") + format + ") ";
    if (items[itm_idx]) {
      str += std::string("(") + items[itm_idx]->getType() + std::string(") ") + std::string(items[itm_idx]->getName());
    }else if (strncmp(message, "PC Equipment", strlen(message)) == 0) 
      str += std::string("(") + object_parser::private_wrapper::get_item_type((itm_idx == 11) ? 8 : itm_idx) + ") ";
    mvprintw(i, (80- 15)/2, str.c_str(), (i - num + ascii_offset));
  }
  refresh();
}

/*void weapon_store(std::vector<Item*>& wShop){
  int generic_prompt(items, "bought", "d", 48, o, print_store);
}*/

void print_store(std::vector<Item*>& items){
  item_printer(items, items.size(), "c", 97, "Weapon shop");
}



















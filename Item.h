#ifndef ITEM_H_
#define ITEM_H_
#include <stdint.h>
#include "object_parser.h"

class Item {
  private:
    int id;
    int x;
    int y;
  	uint8_t type;
  	int color;
  	int hit;
  	object_parser::private_wrapper::dice damage;
  	int dodge;
  	int defense;
  	int weight;
  	int speed;
  	int special;
  	int value;
  	char symbol;
  	std::string name;
  	std::string desc;
  	Item* next;
  public:
    Item(int id, int x, int y, const object_parser::private_wrapper::item_stub* stub);
    char getSymbol() const;
    char getValue() const;
    int getColor() const;
    const char* getName() const;
    const char* getDesc() const;
    void stack(Item* next);
    void unstack(int& nextId);/*gets picked up by a charcter, unstacks from other items if stacked*/
};
#endif
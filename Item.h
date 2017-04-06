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
  public:
    Item(int id, int x, int y, const object_parser::private_wrapper::item_stub* stub);
    char getSymbol();
    char getValue();
    int getColor();
};
#endif
#ifndef ITEM_H_
#define ITEM_H_
#include <stdint.h>
#include "object_parser.h"
class Item {
  private:
  //   const char* name;
  // 	const char* desc;
    int x;
    int y;
  	uint8_t type;
  	int color;
  	int hit;
  	const char* damage;
  	int dodge;
  	int defense;
  	int weight;
  	int speed;
  	int special;
  	int value;
  	char symbol;
  public:
    Item(int x, int y, const object_parser::private_wrapper::item_stub* stub);
};
#endif
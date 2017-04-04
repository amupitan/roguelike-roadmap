#ifndef MONSTER_H
#define MONSTER_H
#include "Character.h"

#include "object_parser.h"

class Monster : public Character{
  private:
    int color;
    int abilities; //enum?
  	int hp;
  	const char* damage;
  public:
    Monster(uint8_t id, int x, int y, const object_parser::private_wrapper::monster_stub* stub);
    Monster(uint8_t id, int x, int y, int speed, uint8_t type);
    Monster(uint8_t id, int x, int y, int speed, char symb, uint8_t type, int color, int hp, const char* damage);
};
#endif
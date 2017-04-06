#ifndef MONSTER_H_
#define MONSTER_H_
#include "Character.h"
#include "object_parser.h"

class Monster : public Character{
  private:
  	int hp;
  	object_parser::private_wrapper::dice damage;
  	const char* name;
  	const char* desc;
  public:
    Monster(uint8_t id, int x, int y, const object_parser::private_wrapper::monster_stub* stub);
    Monster(uint8_t id, int x, int y, int speed, uint8_t type);
};
#endif
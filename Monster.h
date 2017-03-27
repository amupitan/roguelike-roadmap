#ifndef MONSTER_H
#define MONSTER_H
#include "Character.h"
class Monster : public Character{
  public:
    Monster(uint8_t id, int x, int y, int speed, uint8_t type);
};
#endif
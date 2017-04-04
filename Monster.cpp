#include "Monster.h"

namespace mon_stub = object_parser::private_wrapper;
Monster::Monster(uint8_t id, int x, int y, const mon_stub::monster_stub* stub) : Character(id, x, y, mon_stub::parse_dice(stub->speed), mon_stub::getAbility(stub->abilities)){
  value = stub->symb.front();
  color = mon_stub::getColor(stub->color);
  hp = mon_stub::parse_dice(stub->hp);
  damage = ""; //TODO
}

Monster::Monster(uint8_t id, int x, int y, int speed, uint8_t type) : Character(id, x, y, speed, type){}

Monster::Monster(uint8_t id, int x, int y, int speed, char symb, uint8_t type, int color, int hp, const char* damage) : Character(id, x, y, speed, type){
  this->color = color;
  value = symb;
}

// unsigned short getAbilities(const char* abilities){ //TODO: change return type
//   return 0;
// }
#include "Item.h"

namespace item_stub = object_parser::private_wrapper;
Item::Item(int id, int x, int y, const item_stub::item_stub* stub) : id(id), x(x), y(y), color(item_stub::getColor(stub->color)), hit(item_stub::parse_dice(stub->hit)), damage(item_stub::dice(stub->damage)),
                                                            dodge(item_stub::parse_dice(stub->dodge)), defense(item_stub::parse_dice(stub->defense)), weight(item_stub::parse_dice(stub->weight)),
                                                            speed(item_stub::parse_dice(stub->speed)), special(item_stub::parse_dice(stub->special)), value(item_stub::parse_dice(stub->value)),
                                                            symbol(item_stub::symbolize(stub->type)), name(stub->name), desc(stub->desc), next(0) {}
                                                            
char Item::getSymbol() const{
  return next ? '&' : symbol;
}

int Item::getColor() const{
  return color;
}

char Item::getValue() const{
  return value;
}

const char* Item::getName() const{
  return name.c_str();
}

const char* Item::getDesc() const{
  return desc.c_str();
}
    
int Item::getId() const{
  return id;
}

int Item::getDamageBonus() const{
  return damage.roll();
}

int Item::getDefenseBonus() const{
  return defense;
}

int Item::getSpeedBonus() const{
  return speed;
}

void Item::stack(Item* nextItem){
  if (next) {
    /*TODO: remove. There should never already be a next*/
    exit(-2);
  }
  next = nextItem;
}

void Item::unstack(int& nextId){
  nextId = next ? next->id : -1;
  next = 0;
}
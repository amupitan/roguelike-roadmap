#include "Item.h"

namespace item_stub = object_parser::private_wrapper;
Item::Item(int id, const item_stub::item_stub* stub) : id(id), color(item_stub::getColor(stub->color)), hit(item_stub::parse_dice(stub->hit)), damage(item_stub::dice(stub->damage)),
                                                            dodge(item_stub::parse_dice(stub->dodge)), defense(item_stub::parse_dice(stub->defense)), weight(item_stub::parse_dice(stub->weight)),
                                                            speed(item_stub::parse_dice(stub->speed)), special(item_stub::parse_dice(stub->special)), value(item_stub::parse_dice(stub->value)),
                                                            symbol(item_stub::symbolize(stub->type)), name(stub->name), desc(stub->desc), next(0), equiped(false) {}
                                                            
Item::Item(int id, const Item& item) : id(id), color(item.color), hit(item.hit), damage(item.damage), dodge(item.dodge), defense(item.defense), 
                                       weight(item.weight), speed(item.speed), special(item.special), value(item.value), symbol(item.symbol), name(item.name), desc(item.desc),
                                       next(0), equiped(false){}
                                                            
char Item::getSymbol() const{
  return next ? '&' : symbol;
}

int Item::getColor() const{
  return color;
}

int Item::getValue() const{
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

std::string Item::getDamage() const{
  return damage.to_string();
}

const char* Item::getType() const{
  return item_stub::item_type(symbol);
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

bool Item::isEquiped() const{
  return equiped;
}

int Item::getWeight() const{
  return weight;
}

int Item::getDodge() const{
  return dodge;
}

int Item::getHit() const{
  return hit;
}

int Item::getSpecial() const{
  return special;
}

void Item::revalue(double rate){
  value *= rate/100.0;
}

void Item::equip(){
  equiped = true;
}

void Item::unequip(){
  equiped = false;
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

void Item::resetId(int new_id){
  id = new_id;
}

void Item::setSymbol(char symbol){
  this->symbol = symbol;
}

Item::~Item(){}
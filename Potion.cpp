#include "Potion.h"

Potion::Potion(int id, const object_parser::private_wrapper::item_stub* stub) : Item(id, stub){
  Item::setSymbol('*');
}

int Potion::getRadius() const{
  return 5;
}

Potion::Potion(int id, const Potion& potion) : Item::Item(id, /**static_cast<Item*>(potion)*/ potion){
//   *this = potion;//TODO: do i need this? is it correct?
//   this->id = id;
// }
// const char* Potion::getType() const{
//   return Item::getType();
}
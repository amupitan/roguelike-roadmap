#ifndef POTION_H_
#define POTION_H_
#include "Item.h"
// #include "object_parser.h"

class Potion : public Item{
  public:
    Potion(int id, const object_parser::private_wrapper::item_stub* stub);
    Potion(int id, const Potion& potion);
    int getRadius() const;
    // virtual const char* getType() const;
};
#endif
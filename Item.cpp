#include "Item.h"

namespace item_stub = object_parser::private_wrapper;
Item::Item(int x, int y, const item_stub::item_stub* stub) : x(x), y(y), color(item_stub::getColor(stub->color)), hit(item_stub::parse_dice(stub->hit)), damage(""),
                                                            dodge(item_stub::parse_dice(stub->dodge)), defense(item_stub::parse_dice(stub->defense)), weight(item_stub::parse_dice(stub->weight)),
                                                            speed(item_stub::parse_dice(stub->speed)), special(item_stub::parse_dice(stub->special)), value(item_stub::parse_dice(stub->value)),
                                                            symbol(item_stub::symbolize(stub->type)){}
                                                            
char Item::getSymbol(){
  return symbol;
}

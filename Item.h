#ifndef ITEM_H_
#define ITEM_H_
#include <stdint.h>
#include "object_parser.h"

class Item {
  private:
    int id;
    // int x;
    // int y;
  	uint8_t type;
  	int color;
  	int hit;
  	object_parser::private_wrapper::dice damage;
  	int dodge;
  	int defense;
  	int weight;
  	int speed;
  	int special;
  	int value;
  	char symbol;
  	std::string name;
  	std::string desc;
  	Item* next;
  	bool equiped;
  protected:
    void setSymbol(char symbol);
  public:
    Item(int id, const object_parser::private_wrapper::item_stub* stub);
    Item(int id, const Item& item);
    char getSymbol() const;
    int getValue() const;
    int getColor() const;
    const char* getName() const;
    const char* getDesc() const;
    int getId() const;
    std::string getDamage() const;
    virtual const char* getType() const;
    int getDamageBonus() const;
    int getDefenseBonus() const;
    int getSpeedBonus() const;
    bool isEquiped() const;
    int getWeight() const;
    int getDodge() const;
    int getHit() const;
    int getSpecial() const;
    void revalue(double rate);
    void equip();
    void unequip();
    void stack(Item* next);
    void resetId(int new_id);
    void unstack(int& nextId);/*gets picked up by a charcter, unstacks from other items if stacked*/
    virtual ~Item();
};
#endif
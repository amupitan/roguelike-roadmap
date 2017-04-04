#include <cstdio>
#include <cstdlib>
#include "Character.h"

Character::Character(uint8_t id, int x, int y) : speed(0), pace(0){
  this->id = id;
  this->x = x;
  this->y = y;
}

Character::Character(uint8_t id, int x, int y, int speed, uint8_t type){
  this->id = id;
  this->x = x;
  this->y = y;
  this->speed = speed;
  this->type = type;
  char temp_val[2];
  sprintf(temp_val, "%x", type);
  value = *temp_val;
  pace = 1000/speed;
}

/**
 *This initializes to PC and should only be called by PC
 *
 **/
Character::Character() :  speed(10), id(0), value('@') {}//TODO: add functionality that only PC type can call this

void Character::setPos(void* x, void* y){
  if (x)
    this->x = *(int *)x;
  if (y)
    this->y = *(int *)y;
}

void Character::setType(uint8_t type){
  this->type = type;
}

void Character::printCharacter() const{
  printf("x: %d, y: %d\n", x, y);
}

int Character::getX() const{
  return x;
}

int Character::getY() const{
  return y;
}

int Character::getId() const{
  return id;
}

char Character::getValue() const{
  return value;
}

uint8_t Character::getSpeed() const{
  return speed;
}

void Character::killCharacter(){
  value = -1;
}

bool Character::checkType(uint8_t type) const{
  return (type & this->type) != 0;
}

Character::~Character(){}

bool Character::operator<(const Character& rhs) const{
	return pace < rhs.pace;
}

bool Character::operator>(const Character& rhs) const{
	return pace > rhs.pace;
}

bool Character::operator==(const Character& rhs) const{
	return id < rhs.id;
}



void Character::updatePace(){
	pace += 1000/speed;
}

// struct CharacterComparator{
  // bool operator<(const Character& lhs, const Character& rhs){
  // 	return lhs.pace < rhs.pace;
  // }
// }

/*C Wrapper functions*/
void csetPos(Character* p, void* x, void* y){
  p->setPos(x, y);
}

void csetType(Character* p, uint8_t type){
  p->setType(type);
}

void cprintCharacter(Character* p){
  p->printCharacter();
}

Character* c_construct(uint8_t id, int x, int y){
  Character* temp = (Character *)malloc(sizeof(Character));
  *temp = Character(id, x, y);
  return temp;
}

Character* construct_Character(uint8_t id, int x, int y, int speed, uint8_t type){
  return new Character(id, x, y, speed, type);
}

int cgetX(Character* p){
  return p->getX();
}

int cgetY(Character* p){
  return p->getY();
}

int cgetId(Character* p){
  return p->getId();
}

uint8_t cgetSpeed(Character* p){
  return p->getSpeed();
}

char cgetValue(Character *p){
  return p->getValue();
}

int ccheckType(Character* p, uint8_t type){
  return (p->checkType(type)) ? 1 : 0;
}

void ckillCharacter(Character* p){
  p->killCharacter();
}

void deleteCharacter(Character* p){
  delete p;
}

#include <cstdio>
#include <cstdlib>
#include "Player.h"

Player::Player(uint8_t id, int x, int y){
  this->id = id;
  this->x = x;
  this->y = y;
}

void Player::setPos(void* x, void* y){
  if (x)
    this->x = *(int *)x;
  if (y)
    this->y = *(int *)y;
}

void Player::printPlayer(){
  printf("x: %d, y: %d\n", x, y);
}

void csetPos(Player* p, void* x, void* y){
  p->setPos(x, y);
}

void cprintPlayer(Player* p){
  p->printPlayer();
}

Player* c_construct(uint8_t id, int x, int y){
  Player* temp = (Player *)malloc(sizeof(Player));
  *temp = Player(id, x, y);
  return temp;
}
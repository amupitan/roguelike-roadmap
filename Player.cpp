#include <cstdio>
#include <cstdlib>
#include "Player.h"

Player::Player(uint8_t id, int x, int y){
  this->id = id;
  this->x = x;
  this->y = y;
}

Player::Player(uint8_t id, int x, int y, int speed, uint8_t type){
  this->id = id;
  this->x = x;
  this->y = y;
  this->speed = speed;
  this->type = type;
  char temp_val[2];
  sprintf(temp_val, "%x", type);
  value = *temp_val;
  if (id == 0) value = '@'; /*TODO: find a better way to change PC's value. Consider a setValue method*/
  sight = NULL;
}

void Player::setPos(void* x, void* y){
  if (x)
    this->x = *(int *)x;
  if (y)
    this->y = *(int *)y;
}

void Player::setType(uint8_t type){
  this->type = type;
}

char** Player::setSight(int height, int width){
  if (width < 1 || height < 1 || sight){
    // fprintf(stderr, "Height %d < 1 Width %d < 1 or sight is not null", height, width);
    return sight; /*TODO: handle this in a better way, remove fprintf, consider merging both if blocks*/
  }
  if ((sight = (char **)malloc(sizeof(char*) * height))){
    for (int i = 0; i < height; i++)
      *(sight + i) = (char* )malloc(sizeof(char) * width);
  }
  return sight;
}

// void Player::updateSight(int height, int width, char map[][width]){
//   // char** sight = csetSight(pc, nRows, nCols); /*TODO: check value of sight*/
//   if (sight && id == 0){ /*Checks that Player is PC*/
//     int startX = (x - 5 > 1) ? x - 5 : 1;
//     int startY = (y - 5 > 1) ? y - 5 : 1;
//     int endX = (x + 5 < width - 1) ? x + 5 : width - 1;
//     int endY = (y + 5 < height - 1) ? y + 5 : height - 1;
//     for (int i = startY; i < endY; i++){
//       for (int j = startX; j < endX; j++){
//         sight[i][j] = map[i][j];
//       }
//     }
//   }
  
// }

void Player::freeSight(int height){
  if (sight){
    for (int i = 0; i < height; i++)
      free(*(sight + i)); //vs free(*(sight + i*sizeof(sight[0]));
    free(sight);
    sight = NULL;
  }
}

void Player::printPlayer(){
  printf("x: %d, y: %d\n", x, y);
}

int Player::getX(){
  return x;
}

int Player::getY(){
  return y;
}

int Player::getId(){
  return id;
}

char Player::getValue(){
  return value;
}

uint8_t Player::getSpeed(){
  return speed;
}

void Player::killPlayer(){
  value = -1;
}

bool Player::checkType(uint8_t type){
  return (type & this->type) != 0;
}

Player::~Player(){}


/*C Wrapper functions*/
void csetPos(Player* p, void* x, void* y){
  p->setPos(x, y);
}

void csetType(Player* p, uint8_t type){
  p->setType(type);
}

char** csetSight(Player* p, int height, int width){
  return p->setSight(height, width);
}

// void cupdateSight(Player* p, int height, int width, char map[][width]){
//   p->updateSight(height, width, map);
// }

void cfreeSight(Player* p, int height){
  return p->freeSight(height);
}


void cprintPlayer(Player* p){
  p->printPlayer();
}

Player* c_construct(uint8_t id, int x, int y){
  Player* temp = (Player *)malloc(sizeof(Player));
  *temp = Player(id, x, y);
  return temp;
}

Player* construct_player(uint8_t id, int x, int y, int speed, uint8_t type){
  // Player* temp = (Player *)malloc(sizeof(Player));
  // *temp = Player(id, x, y, speed, type);
  // return temp;
  return new Player(id, x, y, speed, type);
}

int cgetX(Player* p){
  return p->getX();
}

int cgetY(Player* p){
  return p->getY();
}

int cgetId(Player* p){
  return p->getId();
}

uint8_t cgetSpeed(Player* p){
  return p->getSpeed();
}

char cgetValue(Player *p){
  return p->getValue();
}

int ccheckType(Player* p, uint8_t type){
  return (p->checkType(type)) ? 1 : 0;
}

void ckillPlayer(Player* p){
  p->killPlayer();
}

void deletePlayer(Player* p){
  delete p;
}
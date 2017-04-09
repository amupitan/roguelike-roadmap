#include <iostream>
#include "Player.h"
#include "display.h"

Player* Player::player = 0;

Player* Player::getPlayer(){
  if (!player) //player has not been created
    player = new Player();
  return player;
}

Player::Player() : sight(0), carry(), equip(), item_no(0) {
  symbol = '@';
  color = 2;//GREEN
  damage = "0+1d4";
  hp = 100;
}

Pair Player::getPos() const{
  Pair position = {this->getX(), this->getY()};
  return position;
}

Player::Player(Player const& player_copy){}//TODO why?

int** Player::setSight(int height, int width){
  if (width < 1 || height < 1 || sight){
    // fprintf(stderr, "Height %d < 1 Width %d < 1 or sight is not null", height, width);
    return sight; /*TODO: handle this in a better way, remove fprintf, consider merging both if blocks*/
  }
  if ((sight = (int **)malloc(sizeof(int*) * height))){
    for (int i = 0; i < height; i++)
      *(sight + i) = (int* )malloc(sizeof(int) * width);
  }
  return sight;
}

// void Character::updateSight(int height, int width, char map[][width]){
//   // char** sight = csetSight(pc, nRows, nCols); /*TODO: check value of sight*/
//   if (sight && id == 0){ /*Checks that Character is PC*/
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
Item ** Player::inventory(){ //TODO: player drops inventory when using the stairs
  return carry;
}

bool Player::pick(Item* item){
  for (int i = 0; i < 10; i++){
      if (!carry[i]){
        carry[i] = item;
        return true;
      }
  }
  return false;
}

int Player::drop(int itm_idx){
  if (!carry[itm_idx]) return -1; //NOTE: -1 not 0 swap maybe?
  int itm_id = carry[itm_idx]->getId();
  carry[itm_idx] = 0;
  return itm_id;
}

void Player::freeSight(int height){
  if (sight){
    for (int i = 0; i < height; i++)
      free(*(sight + i)); //vs free(*(sight + i*sizeof(sight[0]));
    free(sight);
    sight = 0;
  }
}

void Player::deletePlayer(){ //TODO: is it standard to have such a member?
  delete player;
  player = 0;
}

/*C Wrapper functions*/
int** csetSight(Character* p, int height, int width){
  return static_cast<Player*>(p)->setSight(height, width);//TODO: check if type is player
}

// void cupdateSight(Player* p, int height, int width, char map[][width]){
//   p->updateSight(height, width, map);
// }

void cfreeSight(Character* p, int height){
  return static_cast<Player*>(p)->freeSight(height);
}

/**
 * TODO readings:
 * Look into why protected members don't work in the initization lists
 *
 */
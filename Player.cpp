#include "Player.h"
#include <cstring>
#include "display.h"

Player* Player::player = 0; /*TODO: not necessary since static objects are set to 0 by default*/

Player* Player::getPlayer(){
  if (!player) //player has not been created
    player = new Player();
  return player;
}

Player::Player() : sight(0), carry(), equip() {
  damage = "0+1d4";
  hp = 500;
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

Item ** Player::equipment(){
  return equip;
}

bool Player::pick(Item* item){
  for (int i = 0; i < 10; i++){
      if (!carry[i]){
        carry[i] = item;
        carry[i]->equip();
        return true;
      }
  }
  return false;
}

int Player::drop(int itm_idx){
  if (!carry[itm_idx]) return -1; //NOTE: -1 not 0 swap maybe?
  carry[itm_idx]->unequip();
  int itm_id = carry[itm_idx]->getId();
  carry[itm_idx] = 0;
  return itm_id;
}

void Player::expunge(int itm_idx){
  carry[itm_idx] = 0;
}

void Player::wear(int itm_idx){
  if (!carry[itm_idx]) return;
  Item* add = carry[itm_idx];
  int slot = object_parser::private_wrapper::item_number(add->getSymbol());
  if (slot != -1 && slot <= 10){
    if (slot == 8){ /*TODO: item 8 is never swapped out except if there's nothing there*/
      slot = equip[slot] ? 11 : 8;
    }
    Item* current = equip[slot];
    equip[slot] = add;
    speed += add->getSpeedBonus();
    carry[itm_idx] = current;
  }
}

bool Player::take_off(int index){
  if (pick(equip[index])){
    speed -= equip[index]->getSpeedBonus();
    equip[index] = 0;
    return true;
  }
  return false;
}

int Player::attack() const{
  int dam = Character::attack();
  for (unsigned int i = 0; i < sizeof(equip)/sizeof(equip[0]); i++){
    if (equip[i]) dam += equip[i]->getDamageBonus();
  }
  return dam;
}

bool Player::takeDamage(int damage_in){
  int temp = damage_in;
  for (unsigned int i = 0; i < sizeof(equip)/sizeof(equip[0]); i++, temp += 0){
    if (equip[i]) damage_in -= equip[i]->getDefenseBonus();
  }
  return (damage_in < 0) ? false : Character::takeDamage(damage_in);
}

void Player::freeSight(int height){
  if (sight){
    for (int i = 0; i < height; i++)
      free(*(sight + i)); //vs free(*(sight + i*sizeof(sight[0]));
    free(sight);
    sight = 0;
  }
}

void Player::clearSlots(bool inventory, bool equipment){
  if (inventory) memset(carry, 0, sizeof(carry));
  if (equipment){
    speed = 10;
    memset(equip, 0, sizeof(equip));
  } 
}

void Player::unequip_all(){
  for (auto item : carry){
    if (item)
      item->unequip();
  }
  for (auto item : equip){
    if (item)
      item->unequip();
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

void cfreeSight(Character* p, int height){
  return static_cast<Player*>(p)->freeSight(height);
}

/**
 * TODO readings:
 * Look into why protected members don't work in the initization lists
 *
 */
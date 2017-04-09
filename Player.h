#ifndef PLAYER_H
#define PLAYER_H
#include "Character.h"
#include "Item.h"

#ifndef __cplusplus
typedef void Player;
#endif

#ifdef __cplusplus

class Player : public Character{
  private:
    Player();
    Player(Player const&);
    Player& operator=(Player const&);
    static Player* player;
    int** sight;
    Item* carry[10];
    Item* equip[14];
    int item_no;
  public:
    static Player* getPlayer();
    Item ** inventory();
    int** setSight(int height, int width);
    bool pick(Item* item);
    int drop(int itm_idx);
    // void updateSight(int height, int width, char map[][width]);
    void freeSight(int height);
    static void deletePlayer();
};


extern "C" {
#endif /*EXTERN OPEN*/
  int** csetSight(Character* p, int height, int width);
  // void cupdateSight(Character* p, int height, int width, char map[][width]);
  void cfreeSight(Character* p, int height);
  
  
#ifdef __cplusplus
}
#endif /*EXTERN CLOSE*/

#endif /*PLAYER_H*/


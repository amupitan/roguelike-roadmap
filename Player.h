#ifndef PLAYER_H
#define PLAYER_H
#include "Character.h"
#include "Item.h"
#include "Cell_Pair.h"

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
    Item* equip[12];
  public:
    static Player* getPlayer();
    Pair getPos() const;
    Item ** inventory();
    Item ** equipment();
    int** setSight(int height, int width);
    bool pick(Item* item);
    int drop(int itm_idx);
    // bool expunge(int itm_idx);
    void wear(int itm_idx);
    bool take_off(int index);
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


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
    Item* potions[5];
    int weight;
    int max_weight;
    int pesos;
  public:
    static Player* getPlayer();
    Pair getPos() const;
    int getWeight() const;
    int getMaxWeight() const;
    int getPesos() const;
    Item ** inventory();
    Item ** equipment();
    Item ** purse();
    int** setSight(int height, int width);
    Item* buy(int index, Item* item);
    bool pick(Item* item);
    int drop(int itm_idx);
    void expunge(int itm_idx);
    void wear(int itm_idx);
    bool take_off(int index);
    virtual int attack() const;
    void earn(int amount);
    virtual bool takeDamage(int damage);
    // void updateSight(int height, int width, char map[][width]);
    void freeSight(int height);
    void clearSlots(bool inventory, bool equipment);
    void unequip_all();
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


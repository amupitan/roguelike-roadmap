#ifndef PLAYER_H
#define PLAYER_H
#include "Character.h"

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
  public:
    static Player* getPlayer();
    int** setSight(int height, int width);
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


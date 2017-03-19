#ifndef PLAYER_H
#define PLAYER_H

/*External libs*/
#include <stdint.h>

#ifndef __cplusplus
typedef void Player;
#endif

#ifdef __cplusplus
//using namespace std;
class Player{
  private:
    int x;
    int y;
    char value;
    uint8_t type:4;
    int: 4;
    uint8_t speed; //can use :5?
    uint8_t id; //int?
  public:
    Player(uint8_t id, int x, int y);
    void setPos(void* x, void* y);
    void printPlayer();
};


extern "C" {
#endif /*EXTERN OPEN*/
  /*Place wrapper functions here*/
  Player* c_construct(uint8_t id, int x, int y);
  void csetPos(Player* p, void* x, void* y);
  void cprintPlayer(Player* p);
#ifdef __cplusplus
}
#endif /*EXTERN CLOSE*/

#endif /*PLAYER_H*/
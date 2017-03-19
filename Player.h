#ifndef PLAYER_H
#define PLAYER_H

/*External libs*/
#include <stdint.h>

#ifndef __cplusplus
typedef void Player;
#endif

#ifdef __cplusplus

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
    Player(uint8_t id, int x, int y, int speed, uint8_t type);
    void setPos(void* x, void* y);
    void setType(uint8_t type);
    void printPlayer();
    int getX();
    int getY();
    int getId();
    char getValue();
    uint8_t getSpeed();
    bool checkType(uint8_t type);
    void killPlayer();
};


extern "C" {
#endif /*EXTERN OPEN*/

  Player* c_construct(uint8_t id, int x, int y);
  Player* construct_player(uint8_t id, int x, int y, int speed, uint8_t type);
  void csetPos(Player* p, void* x, void* y);
  void csetType(Player* p, uint8_t type);
  void cprintPlayer(Player* p);
  int cgetX(Player* p);
  int cgetY(Player* p);
  int cgetId(Player* p);
  char cgetValue(Player *p);
  int ccheckType(Player* p, uint8_t type);
  void ckillPlayer(Player *p);
  uint8_t cgetSpeed(Player* p);
  
  
#ifdef __cplusplus
}
#endif /*EXTERN CLOSE*/

#endif /*PLAYER_H*/
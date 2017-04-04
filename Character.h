#ifndef CHARACTER_H
#define CHARACTER_H

/*External libs*/
#include <stdint.h>

#ifndef __cplusplus
typedef void Character;
#endif

#ifdef __cplusplus

class Character{
  private:
    int x;
    int y;
    
    uint8_t type:4;
    int: 4;
    uint8_t speed; //can use :5?
    uint8_t id; //int?
    uint32_t pace;
    
    
    
  protected:
    Character();
    char value;
    
  public:
    Character(uint8_t id, int x, int y);
    Character(uint8_t id, int x, int y, int speed, uint8_t type);
    void setPos(void* x, void* y);
    void setType(uint8_t type);

    void updatePace();
    void printCharacter() const;
    int getX() const;
    int getY() const;
    int getId() const;
    // Pair getPos() const; //remove?
    char getValue() const;
    uint8_t getSpeed() const;
    bool checkType(uint8_t type) const;
    void killCharacter();
    
    bool operator<(const Character& rhs) const;
    bool operator>(const Character& rhs) const;
    bool operator==(const Character& rhs) const;
    ~Character();
};


extern "C" {
#endif /*EXTERN OPEN*/
  // struct CharacterComparator;
  // bool operator<(const Character& lhs, const Character& rhs);
  Character* c_construct(uint8_t id, int x, int y);
  Character* construct_Character(uint8_t id, int x, int y, int speed, uint8_t type);
  void csetPos(Character* p, void* x, void* y);
  void csetType(Character* p, uint8_t type);
  char** csetSight(Character* p, int height, int width);
  // void cupdateSight(Character* p, int height, int width, char map[][width]);
  void cfreeSight(Character* p, int height);
  void cprintCharacter(Character* p);
  int cgetX(Character* p);
  int cgetY(Character* p);
  int cgetId(Character* p);
  char cgetValue(Character *p);
  int ccheckType(Character* p, uint8_t type);
  void ckillCharacter(Character *p);
  uint8_t cgetSpeed(Character* p);
  void deleteCharacter(Character* p);
  
  
#ifdef __cplusplus
}
#endif /*EXTERN CLOSE*/

#endif /*Character_H*/

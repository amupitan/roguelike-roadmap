#ifndef CHARACTER_H
#define CHARACTER_H

/*External libs*/
#include <stdint.h>
#include "object_parser.h"

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
    uint8_t id; //int?
    uint32_t pace;

  protected:
    Character();
    uint8_t speed;
    char symbol;
    int color;
    int hp;
    object_parser::private_wrapper::dice damage;
    
  public:
    Character(uint8_t id, int x, int y);
    Character(uint8_t id, int x, int y, int speed, uint8_t type);
    void setPos(const void* x, const void* y);
    void setType(uint8_t type);

    void updatePace();
    void printCharacter() const;
    int getX() const;
    int getY() const;
    int getId() const;
    // Pair getPos() const; //remove?
    char getSymbol() const;
    uint8_t getSpeed() const;
    int getColor() const;
    int getHp() const;
    bool checkType(uint8_t type) const;
    void killCharacter();
    virtual int attack() const;
    
    /**
     * Reduces the characters hp by @damage and returns true
     * if character dies after damage else false
     * @damage amount of damage dealt to the character
     */
    virtual bool takeDamage(int damage);
    
    bool operator<(const Character& rhs) const;
    bool operator>(const Character& rhs) const;
    bool operator==(const Character& rhs) const;
    virtual ~Character();
};


extern "C" {
#endif /*EXTERN OPEN*/
  // struct CharacterComparator;
  // bool operator<(const Character& lhs, const Character& rhs);
  Character* c_construct(uint8_t id, int x, int y);
  Character* construct_Character(uint8_t id, int x, int y, int speed, uint8_t type);
  void csetPos(Character* p, void* x, void* y);
  void csetType(Character* p, uint8_t type);
  // char** csetSight(Character* p, int height, int width);
  // void cupdateSight(Character* p, int height, int width, char map[][width]);
  void cprintCharacter(Character* p);
  int cgetX(Character* p);
  int cgetY(Character* p);
  int cgetId(Character* p);
  char cgetSymbol(Character *p);
  int ccheckType(Character* p, uint8_t type);
  void ckillCharacter(Character *p);
  uint8_t cgetSpeed(Character* p);
  void deleteCharacter(Character* p);
  
  
#ifdef __cplusplus
}
#endif /*EXTERN CLOSE*/

#endif /*Character_H*/

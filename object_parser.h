#ifndef OBJECT_PARSER_H_
#define OBJECT_PARSER_H_
// #include <cstdint>
#include <string>
#include <vector>
#include <fstream>

namespace object_parser{
	namespace private_wrapper{
  	class stub{
      public:
        stub(){}
        virtual bool build(std::ifstream& file, const char* line) = 0;
        virtual bool complete() = 0;
        virtual ~stub(){}
  	};
  	class monster_stub : public stub{
	    public:
  	    std::string name;
      	std::string desc;
      	std::string color; //make enum? use array
      // 	int speed;
      // 	std::vector<std::string> abilities; //enum?
      // 	int hp;
      // 	int damage;
      	std::string speed;
      	std::string abilities; //enum?
      	std::string hp;
      	std::string damage;
      	std::string symb;
        monster_stub(){}
        monster_stub operator=(monster_stub& copy);
        bool complete();
        bool build(std::ifstream& file, const char* line);
        friend std::ostream& operator<< (std::ostream& stream, const monster_stub& monster);
  	};
  	class item_stub : public stub{
	    public:
	      std::string name;
      	std::string desc;
      	std::string type;
      	std::string color; //make enum? use array
      	std::string hit;
      	std::string damage; //enum?
      	std::string dodge;
      	std::string defense;
      	std::string weight;
      	std::string speed;
      	std::string symb;
      	std::string special;
      	std::string value;
        item_stub(){}
        item_stub operator=(item_stub& copy);
        bool complete();
        bool build(std::ifstream& file, const char* line);
        friend std::ostream& operator<< (std::ostream& stream, const item_stub& item);
  	};
  	
  	/*global varaibles and helper functions*/
  	extern std::vector<stub*> objects;
  	extern const char* path;
  	extern std::ifstream object_file;
  	extern const char* header;
  	extern const char* start;
  	extern stub* (*getStub)();
  	stub* getMonsterStub();
  	stub* getItemStub();
  	bool startsWith(const char* str, const char* start);
  	void delete_objects();
  	int parse_dice(std::string dice);
  	char symbolize(std::string name);
  	
  	bool assign_attrbute(std::string& attr, const char* value, int offset);
  // 	monster_stub* getMonsterStubs(monster_stub* monsters);
  	item_stub* getItemStubs(item_stub* items);
  	
  // 	std::vector<Monster*> getMonsterStubs(std::vector<Monster*>& monsters);
  	unsigned short getAbility(std::string abilities);
  	unsigned short getColor(std::string color);
  // 	Monster* createMonster(monster_stub* stub);
	}
	
	/*Parser methods*/
	void parser_init(const char* parser);
	void start_parser(const char* chosen_path);
	void complete_parse();
	bool parse_object();
	private_wrapper::monster_stub* getCompleteMonsterStub(unsigned int num);
	private_wrapper::item_stub* getCompleteItemStub(unsigned int num);
	int getNumMonstubs();
	int getNumItemstubs();
// 	monster_stub* getCompleteMonsterStub(int num);
// 	std::vector<private_wrapper::monster_stub> getMonsterStubs();
}

#endif

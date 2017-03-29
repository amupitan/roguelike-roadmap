#ifndef MONSTER_PARSER_H_
#define MONSTER_PARSER_H_
#include <string>
#include <vector>
#include <fstream>

namespace monster_parser{
	namespace private_wrapper{
	  class monster_stub{
	    private:
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
      public:
        bool build_monster(std::ifstream& file, const char* line);
        monster_stub();
        void print();
        bool complete();
        friend std::ostream& operator<< (std::ostream& stream, const monster_stub& monster);
  	};
  	std::vector<monster_stub> monsters;
  	extern const char* path;
  	extern std::ifstream monster_file;
  	bool startsWith(const char* str, const char* start);
  	char* mstrcat(char* des, const char* src);
	}
	void start_parser(const char* chosen_path);
	void complete_parse();
	bool parse_monster();
	int addMonster();
	
  
}

#endif

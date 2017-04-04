#include "object_parser.h"
#include <iostream>
#include <cstring>


#include <cstdlib>

namespace object_parser{
  namespace private_wrapper{
    bool monster_stub::complete(){
      return (name != "") && (desc != "") && (color != "") && (speed != "") && (abilities != "") && (hp != "") && (damage != "");
    }
    bool monster_stub::build(std::ifstream& file, const char* line){
      if (private_wrapper::startsWith(line, "NAME") && name == ""){
        return assign_attrbute(name, line, strlen("NAME"));
      }
      else if (private_wrapper::startsWith(line, "DESC") && desc == ""){
        std::string description;
        char nextline[79];
        file.getline(nextline, 79);
        while (!((strlen(nextline) == 1) && (*nextline == '.')) && file.good()){ //is it possible for nextline to be null? or have a length of 0? Empty line
          description.append(nextline);
          description.append("\n");
          file.getline(nextline, 79);
        }
        if (*nextline != '.') return false;
        desc = description;
        return true;
      }
      else if (private_wrapper::startsWith(line, "COLOR") && color == ""){
        return assign_attrbute(color, line, strlen("COLOR"));
      }
      else if (private_wrapper::startsWith(line, "SPEED") && speed == ""){
        return assign_attrbute(speed, line, strlen("SPEED"));
      }
      else if (private_wrapper::startsWith(line, "ABIL") && abilities == ""){
        return assign_attrbute(abilities, line, strlen("ABIL"));
      }
      else if (private_wrapper::startsWith(line, "DAM") && damage == ""){
        return assign_attrbute(damage, line, strlen("DAM"));
      }
      else if (private_wrapper::startsWith(line, "SYMB") && symb == ""){
        return assign_attrbute(symb, line, strlen("SYMB"));
      }
      else if (private_wrapper::startsWith(line, "HP") && hp == ""){
        return assign_attrbute(hp, line, strlen("HP"));
      }
      return false;
    }
    
    monster_stub monster_stub::operator=(monster_stub& copy){
      copy.name = name;
    	copy.desc = desc;
    	copy.color = color; //make enum? use array
    	copy.speed = speed;
    	copy.abilities = abilities; //enum?
    	copy.hp = hp;
    	copy.damage = damage;
    	copy.symb = symb;
    	return copy;
    }
    item_stub item_stub::operator=(item_stub& copy){
      copy.name = name;
    	copy.desc = desc;
    	copy.type = type;
    	copy.color = color; //make enum? use array
    	copy.hit = hit;
    	copy.damage = damage; //enum?
    	copy.dodge = dodge;
    	copy.defense = defense;
    	copy.weight = weight;
    	copy.speed = speed;
    	copy.symb = symb;
    	copy.special = special;
    	copy.value = value;
    	return copy;
    }
    bool item_stub::build(std::ifstream& file, const char* line){
      if (private_wrapper::startsWith(line, "NAME") && name == ""){
        return assign_attrbute(name, line, strlen("NAME"));
      }
      else if (private_wrapper::startsWith(line, "DESC") && desc == ""){
        std::string description;
        char nextline[79];
        file.getline(nextline, 79);
        while (!((strlen(nextline) == 1) && (*nextline == '.')) && file.good()){ //is it possible for nextline to be null? or have a length of 0? Empty line
          description.append(nextline);
          description.append("\n");
          file.getline(nextline, 79);
        }
        if (*nextline != '.') return false;
        desc = description;
        return true;
      }
      else if (private_wrapper::startsWith(line, "TYPE") && type == ""){
        return assign_attrbute(type, line, strlen("TYPE"));
      }
      else if (private_wrapper::startsWith(line, "COLOR") && color == ""){
        return assign_attrbute(color, line, strlen("COLOR"));
      }
      else if (private_wrapper::startsWith(line, "HIT") && hit == ""){
        return assign_attrbute(hit, line, strlen("HIT"));
      }
      else if (private_wrapper::startsWith(line, "DAM") && damage == ""){
        return assign_attrbute(damage, line, strlen("DAM"));
      }
      else if (private_wrapper::startsWith(line, "DODGE") && dodge == ""){
        return assign_attrbute(dodge, line, strlen("DODGE"));
      }
      else if (private_wrapper::startsWith(line, "DEF") && defense == ""){
        return assign_attrbute(defense, line, strlen("DEF"));
      }
      else if (private_wrapper::startsWith(line, "WEIGHT") && weight == ""){
        return assign_attrbute(weight, line, strlen("WEIGHT"));
      }
      else if (private_wrapper::startsWith(line, "SPEED") && speed == ""){
        return assign_attrbute(speed, line, strlen("SPEED"));
      }
      else if (private_wrapper::startsWith(line, "ATTR") && special == ""){
        return assign_attrbute(special, line, strlen("ATTR"));
      }
      else if (private_wrapper::startsWith(line, "VAL") && value == ""){
        return assign_attrbute(value, line, strlen("VAL"));
      }
      return false;
    }
    bool item_stub::complete(){
      return (name != "") && (desc != "") && (color != "") && (speed != "") && (special != "") && (type != "") && (damage != "") && (dodge != "") && (defense != "") && (weight != "") && (value != "") && (hit != "");
    }
    
    std::vector<stub*> objects;
    std::ifstream object_file;
    const char *path, *start, *header;
    stub* (*getStub)();
    bool startsWith(const char* str, const char* start){
      size_t str_len = strlen(str), start_len = strlen(start);
      return str_len < start_len ? false : strncmp(str, start, start_len) == 0;
    }
    std::ostream& operator<< (std::ostream& stream, const monster_stub& monster){
      return std::cout << monster.name << "\n"
                << monster.desc << monster.symb << "\n"
                << monster.color << "\n"
                << monster.speed << "\n"
                << monster.abilities << "\n"
                << monster.hp << "\n"
                << monster.damage << std::endl;
    }
    bool assign_attrbute(std::string& attr, const char* value, int offset){
          attr = value + offset + 1; //+1 for the space
          return true;
    }
    /* Add look-up functionality
    std::map<const char*, std::string*> attr_lookup;
    void attr_map_init(stub* object, const char* type){
      if (strncmp(type, "monster", 8) == 0) monster_stub* obj = static_cast<monster_stub*>(object);
      else if (strncmp(type, "item", 5) == 0) item_stub* obj = static_cast<item_stub*>(object);
      else stub* obj = object;
      // std::cout << obj->name << std::endl;
    }
    void add_attribute(const char* value, std::string& property){
      
    }
    */
    int parse_dice(std::string dice){
      int pos;
      if ((pos = dice.find("+") ) != -1){
        int pos_d;
        if ((pos_d = dice.find("d")) != -1){
          int first = atoi(dice.substr(0, pos).c_str());
          int second = atoi(dice.substr(pos + 1, pos_d).c_str());
          int third = atoi(dice.substr(pos_d + 1, dice.length()).c_str());
          for (int j = 0; j < second; j++)
            first += (rand() % third) + 1;
          return first;
        }
      }
      return -1;
    }
    stub* getMonsterStub(){
      return new monster_stub();
    }
    
    stub* getItemStub(){
      return new item_stub();
    }
    void delete_objects(){
      for (std::vector<int>::size_type i = 0; i < private_wrapper::objects.size(); i++)
        delete private_wrapper::objects[i];
      private_wrapper::objects.clear();
      std::vector<private_wrapper::stub*>(private_wrapper::objects).swap(private_wrapper::objects);
      private_wrapper::path = 0;
      private_wrapper::start = 0;
      private_wrapper::header = 0;
      private_wrapper::getStub = 0;
    }
    /*TODO: function and fix type*/
    unsigned short getAbility(std::string abilities){
      return 0;
    }
    unsigned short getColor(std::string color){
      return 0;
    }
    // std::vector<Monster*> getMonsterStubs(std::vector<Monster*>& monsters){
    //   //TODO: return monsters not stubs
    //   for (std::vector<int>::size_type i = 0; i < objects.size(); i++){
    //     monster_stub* temp = static_cast<monster_stub*>(objects[i]);
    //     monsters.push_back(new Monster(i, 0, 0, parse_dice(temp->speed),
    //                         temp->symb.front(),
    //                         getAbility(temp->abilities), getColor(temp->color),
    //                         parse_dice(temp->hp), temp->damage.c_str())
    //                       );
    //   }
    //     // monsters[i] = *static_cast<monster_stub*>(objects[i]);
    //   delete_objects();
    //   return monsters;
    // }
  }

  void start_parser(const char* chosen_path){
    private_wrapper::path = (chosen_path) ? chosen_path : private_wrapper::path;//TODO don't default
    private_wrapper::object_file.open(private_wrapper::path, std::ifstream::in);
    int header_len = strlen(private_wrapper::header) + 1; //+1 to include null character
    if (private_wrapper::object_file.good()){
      char first_line[header_len];
      private_wrapper::object_file.getline(first_line, header_len);
      if (strncmp(private_wrapper::header, first_line, header_len) != 0) exit(-1); /*hard-code*/
    }else exit(-1); //could not open file
  }
  
  void complete_parse(){
    if (!private_wrapper::object_file.is_open()) start_parser(private_wrapper::path);
    while (private_wrapper::object_file.good()){
      char nextline[79]; //doesn't need to be 79, doesn't parse the desc
      private_wrapper::object_file.getline(nextline, 79);
      if (strncmp(private_wrapper::start, nextline, strlen(private_wrapper::start)) == 0){
        if (!parse_object()) continue;
      }
      
    }
    private_wrapper::object_file.close();
  }
  
  bool parse_object(){
    private_wrapper::stub* curr = private_wrapper::getStub();
    private_wrapper::objects.push_back(curr);
    char nextline[79];
    while(private_wrapper::object_file.good() && (private_wrapper::object_file.getline(nextline, 79)) && (strncmp("END", nextline, 4) != 0)){
      if (*nextline == 0) continue;
      if (!curr->build(private_wrapper::object_file, nextline)) {
        delete curr;
        private_wrapper::objects.pop_back();
        return false;
      }
    }
    if (!curr->complete()) {
      delete curr;
     private_wrapper::objects.pop_back();
     return false;
    }
    return true;
     //doesn't need to be 79, doesn't parse the desc
  }
  
  private_wrapper::monster_stub* getCompleteMonsterStub(unsigned int num){
      return (num < private_wrapper::objects.size())  ? static_cast<private_wrapper::monster_stub*>(private_wrapper::objects[num]) : 0;
  }
  
  /*private_wrapper::monster_stub* private_wrapper::getMonsterStubs(private_wrapper::monster_stub* monsters){
    //TODO: return monsters not stubs
    for (std::vector<int>::size_type i = 0; i < private_wrapper::objects.size(); i++)
      monsters[i] = *static_cast<monster_stub*>(private_wrapper::objects[i]);
    private_wrapper::delete_objects();
    return monsters;
  }*/

  private_wrapper::item_stub* private_wrapper::getItemStubs(private_wrapper::item_stub* items){
    /*TODO: return items not stubs*/
    for (std::vector<int>::size_type i = 0; i < private_wrapper::objects.size(); i++)
      items[i] = *static_cast<item_stub*>(private_wrapper::objects[i]);
    private_wrapper::delete_objects();
    return items;
  }
  
  void parser_init(const char* parser){
    if (strncmp(parser, "monster", 8) == 0){
      private_wrapper::path = "monster_desc.txt";
      private_wrapper::start = "BEGIN MONSTER";
      private_wrapper::header = "RLG327 MONSTER DESCRIPTION 1";
      private_wrapper::getStub = private_wrapper::getMonsterStub;
    }else if (strncmp(parser, "item", 5) == 0){
      private_wrapper::path = "object_desc.txt";
      private_wrapper::start = "BEGIN OBJECT";
      private_wrapper::header = "RLG327 OBJECT DESCRIPTION 1";
      private_wrapper::getStub = private_wrapper::getItemStub;
    }
  }
  
  
};

/*int main(){
  srand(100);
  object_parser::parser_init("monster");
  object_parser::complete_parse();
  // std::vector<Monster*> mons;
  // mons = object_parser::private_wrapper::getMonsterStubs(mons);
  // for (std::vector<int>::size_type i = 0; i < mons.size(); i++){
    // printf("%c\n", mons[i]->getValue());
    // std::cout << mons[i] << 0 << std::endl;
  // }
  object_parser::private_wrapper::delete_objects();
}*/
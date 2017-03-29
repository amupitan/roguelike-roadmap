#include "monster_parser.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace monster_parser{
  namespace private_wrapper{
    monster_stub::monster_stub (){}
    bool monster_stub::complete(){
      return (name != "") && (desc != "") && (color != "") && (speed != "") && (abilities != "") && (hp != "") && (damage != "");
    }
    void monster_stub::print(){ //add algo to render description nicely
      std::cout << name << "\n"
                << desc << symb << "\n"
                << color << "\n"
                << speed << "\n"
                << abilities << "\n"
                << hp << "\n"
                << damage << std::endl;
    } //remove
    bool monster_stub::build_monster(std::ifstream& file, const char* line){
      if (private_wrapper::startsWith(line, "NAME")){
        if (name == ""){
          name = line + strlen("NAME") + 1; //+1 for the space
          return true;
        }
      }
      else if (private_wrapper::startsWith(line, "DESC")){
        if (desc == ""){
          std::string description;
          char nextline[79];
          file.getline(nextline, 79);
          while (!((strlen(nextline) == 1) && (*nextline == '.')) && file.good()){ //is it possible for nextline to be null? or have a length of 0? Empty line
            description.append(nextline);
            file.getline(nextline, 79);
          }
          if (*nextline != '.') return false;
          if (description[description.length() - 1] != '\n') description.append("\n");
          desc = description;
          return true;
        }
      }
      else if (private_wrapper::startsWith(line, "COLOR")){
        if (color == ""){
          color = line + strlen("COLOR") + 1; //+1 for the space
          return true;
        }
      }
      else if (private_wrapper::startsWith(line, "SPEED")){
        if (speed == ""){
          speed = line + strlen("SPEED") + 1; //+1 for the space
          return true;
        }
      }
      else if (private_wrapper::startsWith(line, "ABIL")){
        if (abilities == ""){
          abilities = line + strlen("ABIL") + 1; //+1 for the space
          return true;
        }
      }
      else if (private_wrapper::startsWith(line, "DAM")){
        if (damage == ""){
          damage = line + strlen("DAM") + 1; //+1 for the space
          return true;
        }
      }
      else if (private_wrapper::startsWith(line, "SYMB")){
        if (symb == ""){
          symb = line + strlen("SYMB") + 1; //+1 for the space
          return true;
        }
      }
      else if (private_wrapper::startsWith(line, "HP")){
        if (hp == ""){
          hp = line + strlen("HP") + 1; //+1 for the space
          return true;
        }
      }
      return false;
    }
    
    std::vector<monster_stub> monsters;
    std::ifstream monster_file;
    const char* path;
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
  }

  void start_parser(const char* chosen_path){
    private_wrapper::path = (chosen_path) ? chosen_path : "monster_desc.txt";//TODO don't default
    private_wrapper::monster_file.open(private_wrapper::path, std::ifstream::in);
    if (private_wrapper::monster_file.good()){
      char first_line[29];
      private_wrapper::monster_file.getline(first_line, 29);
      if (strncmp("RLG327 MONSTER DESCRIPTION 1", first_line, 29) != 0) exit(-1); /*hard-code*/
    }else exit(-1); //could not open file
  }
  
  void complete_parse(){
    if (!private_wrapper::path) start_parser("monster_desc.txt");
    while (private_wrapper::monster_file.good()){
      char nextline[79]; //doesn't need to be 79, doesn't parse the desc
      private_wrapper::monster_file.getline(nextline, 79);
      if (strncmp("BEGIN MONSTER", nextline, 13) == 0){
        if (!parse_monster()) continue;
      }
      
    }
    private_wrapper::monster_file.close();
  }
  
  bool parse_monster(){
    private_wrapper::monsters.emplace_back();
    private_wrapper::monster_stub& curr = private_wrapper::monsters.back();
    char nextline[79];
    while(private_wrapper::monster_file.good() && (private_wrapper::monster_file.getline(nextline, 79)) && (strncmp("END", nextline, 4) != 0)){
      if (!curr.build_monster(private_wrapper::monster_file, nextline)) {
        private_wrapper::monsters.pop_back();
        return false;
      }
    }
     if (!curr.complete()) {
       private_wrapper::monsters.pop_back();
       return false;
     }
     return true;
     //doesn't need to be 79, doesn't parse the desc
  }
  
  std::vector<private_wrapper::monster_stub> getMonsterStubs(){
    return private_wrapper::monsters;
  }
};


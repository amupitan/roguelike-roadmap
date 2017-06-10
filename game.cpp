#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <sys/stat.h>
#include <stdint.h>
#include <endian.h>
#include <limits.h>
#include <unistd.h>
#include <ncurses.h>
#include <iostream>

#include "display.h"
#include "queue.h"

#include "Monster.h"
#include "game.h"
#include "dungeon.h"
#include "Item.h"
#include "Player.h"
#include "Potion.h"
int main(int argc, char *argv[]){
  //regular
  Dungeon dungeon;
  dungeon.num_rooms = 0;
  
  //monster
  int chars[nRows][nCols];
  memset(chars, -1, sizeof(int)*nRows*nCols);
  int t_dist[nRows][nCols];
  int dist[nRows][nCols];
  int nummon_flag = 0;

	Cell map[nRows][nCols];
	int i = 0, j = 0;
	Cell room_cells[50];
	Pair pc = {-1, -1}; //TODO see if it's better to have in struct or as a variable 2.
	uint8_t pc_type = 0;
	
  //dungeon
  FILE *dungeon_file;
  char dungeon_title[20];
  uint32_t version;
  uint32_t size_dungeon_file = 0;
  
  //handle options an set seed
  int longindex; //TODO remove this and set the args part to NULL
  int option, load = 0, save = 0, seed = (unsigned) time(NULL), nummon = 0, solo = 0, numitem = 0;
  char load_file[100];
  
  static char monster_file[100], item_file[100];
  strcat(strcpy(monster_file, getenv("HOME")), "/.rlg327/monster_desc.txt");
  strcat(strcpy(item_file, getenv("HOME")), "/.rlg327/object_desc.txt");
  struct option longopts[] = {
    {"load", optional_argument, NULL, 'l'},
    {"save", no_argument, &save, 1},
    {"seed", optional_argument, NULL, 'e'},
    {"pc", optional_argument, NULL, 'p'},
    {"nummon", optional_argument, NULL, 'm'},
    {"numitem", optional_argument, NULL, 'i'},
    {"npcload", optional_argument, NULL, 'n'},
    {"itemload", optional_argument, NULL, 'o'},
    {"solo", no_argument, &solo, 1},
    {0,0,0,0}
  };
  while ((option = getopt_long(argc, argv, ":e:", longopts, &longindex)) != -1){
    switch(option){
      case 'l':
        load = 1;
        if (optarg) strcpy(load_file, optarg);
        else
          strcat(strcpy(load_file, getenv("HOME")), "/.rlg327/dungeon");
        break;
      case 'n':
        if (optarg) strcpy(monster_file, optarg);
        break;
      case 'o':
        if (optarg) strcpy(item_file, optarg);
        break;
      case 'e':
        if (optarg) seed = atoi(optarg);
        else fprintf(stderr, "%s: option seed requires an argument\n", argv[0]);;
        break;
      case 'p':
        if (optarg){
          char* co_ord = strtok(optarg, ",");
          pc.x = atoi(co_ord);
          co_ord = strtok(NULL, ",");
          pc.y = atoi(co_ord);
          if (pc.x == 0 || pc.y == 0 || pc.x >= nCols || pc.y >= nRows)
            fprintf(stderr, "%s: Invalid pc co-ordinates: (x: %d, y: %d) \n", argv[0], pc.x, pc.y);
          else pc_type = 1; /*successfully received PC position*/
        }
        break;
      case 'm':
        if (optarg) {
          nummon = atoi(optarg);
          nummon_flag = 1;
        }
        break;
      case 'i':
        if (optarg){
          numitem = atoi(optarg) * -1;
        }
      case 0:
        break;
      case '?':
          fprintf(stderr, "%s: Invalid option: %c ignored\n", argv[0], optopt);
        break;
      case ':':
        fprintf(stderr, "%s: option -%c requires an argument\n", argv[0], optopt);
        break;
      default:
        fprintf(stderr, "%s: option -%c is ignored because it is invalid\n", argv[0], optopt);
        
    }
  }
  
  srand(seed);
  /*Initialize dungoen with white space and hardness*/
  for (i = 0; i < nRows; i++){
		for (j =0; j < nCols; j++){
			map[i][j].value = 32;
			map[i][j].hardness = (i == 0 || j == 0 || i == nRows - 1 || j == nCols - 1) ? 255 : rand_gen(1,254);
		}
	}
	
  if (load){
    if (!(dungeon_file = fopen(load_file, "r"))){
      fprintf(stderr, "The file: %s couldn't be opened\n", load_file);
      return -1;
    }else load_dungeon(dungeon_file, &dungeon, map, dungeon_title, &version, &size_dungeon_file);
  }else create_dungeon(&dungeon, map, room_cells);
  if (save) {
    char path[100];
    mkdir(strcat(strcpy(path, getenv("HOME")), "/.rlg327"),0766);
    strcat(path, "/dungeon");
    FILE* dungeon_file_l;
    if (!(dungeon_file_l = fopen(path, "w"))){
      fprintf(stderr, "Could not write map to file\n");
      return -1;
    }else{
      uint32_t temp = 0;
      //write dungeon title
      strcpy(dungeon_title, "RLG327-S2017");
      fwrite(dungeon_title, 12, 1, dungeon_file_l); //cs: 12
      
      //write verison
      temp = 0;
      version = htobe32(temp);
      fwrite(&version, sizeof(version), 1, dungeon_file_l); //cs: 4
      
      //write size of file
      temp = strlen(dungeon_title) + sizeof(version) + sizeof(uint32_t) + nRows * nCols + sizeof(uint32_t) * dungeon.num_rooms;
      size_dungeon_file = htobe32(temp);
      fwrite(&size_dungeon_file, 4, 1, dungeon_file_l); //cs:4
      
      //write hardness
      for (i = 0; i < nRows; i++){
    		for (j =0; j < nCols; j++){
    		  fwrite(&(map[i][j].hardness), sizeof(unsigned char), 1, dungeon_file_l); //cs:8
    		}
      }
      
      //write rooms
      for (i = 0; i < dungeon.num_rooms; i++){
        Room room_l = dungeon.rooms[i];
        fwrite(&room_l.x, sizeof(uint8_t), 1, dungeon_file_l);
        fwrite(&room_l.y, sizeof(uint8_t), 1, dungeon_file_l);
        fwrite(&room_l.width, sizeof(uint8_t), 1, dungeon_file_l);
        fwrite(&room_l.height, sizeof(uint8_t), 1, dungeon_file_l);
      }
      fclose(dungeon_file_l);
    }
  }
  
  object_parser::parser_init("monster");
  object_parser::start_parser(monster_file);
  object_parser::complete_parse();
  /*Ncurses start*/
  int col = 0;
  bool fullscreen = false;
  ncurses_init();
  
  /*Monster magic and initialize random pc if no valid command line argument was entered*/
  Queue evt;
  queue_init(&evt, char_equals, print_Character);
  
  /*Initialize all Characters (PC and monster)*/
  int chances[6] = {10/*item spawn*/, 80/*item revalue*/, 5/*PC radius*/, 5/*shop max*/, 20/*pesos drop*/,0};
  if (nummon_flag == 0) nummon = rand_gen(dungeon.num_rooms, dungeon.num_rooms*2);
  int l_monsters = nummon;
  Character* characters[nummon + 1];
  Pair last_seen[nummon];
  memset(last_seen, - 1, sizeof(Pair)*nummon);

  /*Dungeon monsters setup*/
  unsigned int pace[nummon + 1];
  addCharcters(&dungeon, &evt, nummon, characters, chars, pace);
  /*Change PC position to user's choice if available*/
  if (pc_type){/*TODO: there might be a bug with this flag, try seed:1489964342 on --pc="30,30"*/
    chars[cgetY(characters[0])][cgetX(characters[0])] = -1;
    csetPos(characters[0], &(pc.x), &(pc.y));
    chars[cgetY(characters[0])][cgetX(characters[0])] = 0;
  }
  
  /*Items*/
  object_parser::parser_init("item");
  object_parser::start_parser(item_file);
  object_parser::complete_parse();
  int item_map[nRows][nCols];
  std::vector<Item*> items;
  addItems(&dungeon, items, item_map, numitem);

  add_stairs(&dungeon, map);
  add_shops(&dungeon, map);
  char recalculate = 1;
  Player* pcp = static_cast<Player*>(characters[0]);
  do{
    pcp = static_cast<Player*>(characters[0]);
    Character* curr;
    curr = (Character *)peek(&evt, &curr);
    Pair target = {cgetX(curr), cgetY(curr)};// {cgetX(curr), cgetY(curr)};
    if (cgetSymbol(curr) == -1){ //it was killed
      dequeue(&evt);
      continue;
    }
    int new_dungeon = 0;
    /*Determine next position of character*/
    if (curr == pcp){/* PC stuff */
      // while (target.x < 1 || (target.x > nCols - 1) || target.y < 1 || target.y > nRows - 1 || (target.x == cgetX(curr) && target.y == cgetY(curr))){ //Reverse psych lol :) and the PC must move, cannot stay in the same spot, just because it's lame to stay in the same place
      //   target.x = rand_gen(cgetX(curr) - 1, cgetX(curr) + 1);
      //   target.y = rand_gen(cgetY(curr) - 1, cgetY(curr) + 1);
      // }
      updateSight(pcp, map, item_map);
      Pair start = {cgetX(curr) - 40, cgetY(curr) - 10};
      
      generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
      do{/*handle input*/
        target = *(Pair *)getInputC(&target);
        if (target.x == -1 && target.y == -1){
          // items = (Item**)delete_items(items, numitem);
          pcp->unequip_all();
          delete_Characters(characters, nummon + 1);
          remove_items(items);
          // clearShop(w);
          endgame(&dungeon, &evt, "Game ended");
          return 0;
        }
        if (target.x == -2 && target.y == -2) {
          /*Enter look mode*/
          Pair look = start;
          do{
            int ctrl = 0; /*1- end look mode*/
            look = *(look_mode(&look, &ctrl));
            if (ctrl == 1){
              generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
              break;
            }
            // pc_render_partial(map, chars, characters, look, &look);
            // render(chars, characters, item_map, items, look, &look);
            generic_render(map, chars, characters, item_map, items, look, &look, fullscreen);
          }while(true);
        }
        else if ((target.x == -3 && target.y == -3) || (target.x == -4 && target.y == -4)){
          /*use stairs: if not stair continue*/
          if ((target.x == -3 && map[cgetY(curr)][cgetX(curr)].value != '>') || (target.x == -4 && map[cgetY(curr)][cgetX(curr)].value != '<')) {/*check up/down stairs*/
            target.x = cgetX(curr);
            target.y = cgetY(curr); /*set target to original position*/
            continue;
          }
          // csetType(pcp, 0);
          l_monsters = nummon;
          // items = (Item**)delete_items(items, numitem);
          remove_items(items);
          delete_dungeon(&dungeon, &evt, map);
          delete_Characters(characters, nummon + 1);
          
          create_dungeon(&dungeon, map, room_cells);
          add_stairs(&dungeon, map);
          add_shops(&dungeon, map);
          addCharcters(&dungeon, &evt, nummon, characters, chars, pace);
          // items = addItems(&dungeon, items, item_map, &numitem); //TODO
          addItems(&dungeon, items, item_map, numitem);
          recalculate = 1;

          /*START get rid of*/
          getmaxyx(stdscr, longindex, col); /*Longindex is passed here because this macro function requires an argument*/
          move(0, 0);
          clrtoeol();
          mvprintw(0, col/2, "pc x %d y: %d %d", cgetX(characters[0]), cgetY(characters[0]), seed);
          
          /*END*/
          new_dungeon = 1;
          break;
        }else if (target.x == -5 && target.y == -5) {
          char* seed_msg = (char* )malloc(20);/*cpp watch*/
          sprintf(seed_msg, "Seed is %d", seed);
          log_message(seed_msg);
          free(seed_msg);
        }else if (target.x == -6 && target.y == -6){
          char* stat_msg = (char* )malloc(40);/*cpp watch*/
          sprintf(stat_msg, "PC is at %d, %d. Number of rooms: %d", cgetX(curr), cgetY(curr), dungeon.num_rooms);
          log_message(stat_msg);
          free(stat_msg);
        }else if (target.x == -7 && target.y == -7){
          do{
            print_inventory(pcp->inventory());
          }while(getch() != 27); /*ESC*/
          target = pcp->getPos();
          // render(chars, characters, item_map, items, start, 0);
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
          log_message((std::string("PC is at ") + std::to_string(pcp->getX()) + ", " + std::to_string(pcp->getY())).c_str()); //TODO: log some useful stats
        }else if (target.x == -8 && target.y == -8){
          int drop = generic_prompt(pcp->inventory(), "removed", 48, 10, "a valid number", print_inventory, 0);//drop_from_inventory(pcp->inventory());
          target = pcp->getPos();
          log_message("Your inventory is unchanged"); //TODO: change to you did not drop an item?
          if (drop >= 0){
            int itm_id = pcp->drop(drop);
            if (item_map[target.y][target.x] != -1){
              items[itm_id]->stack(items[item_map[target.y][target.x]]);
            }
            item_map[target.y][target.x] = itm_id;
            log_message((std::string("You dropped ") + items[itm_id]->getName()).c_str());
          }
          updateSight(pcp, map, item_map);
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
          continue;
        }else if (target.x == -9 && target.y == -9){
          target = pcp->getPos();
          display_stats();
          log_message("Press any key to continue", 0);
          getch();
          log_message("Welcome to the dungeon", 0);
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
          continue;
        }else if (target.x == -10 && target.y == -10){
          target = pcp->getPos();//TODO: this happens in case -3 and shuld prolly happen in every case, refactor?
          continue;
        }else if (target.x == -11 && target.y == -11){
          target = pcp->getPos();
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen = !fullscreen);
          log_message((std::string("Switched to ") + (fullscreen ? "complete" : "partial") + " dungeon display").c_str());
          continue;
        }else if (target.x == -12 && target.y == -12){ /*Wear equipment*/
          int wear = generic_prompt(pcp->inventory(), "worn", 48, 10, "a valid number", print_inventory, 0);
          target = pcp->getPos();
          if (wear >= 0){
            pcp->wear(wear);
          }else log_message("No new equipment was worn", 0);
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
          continue;
        }else if (target.x == -13 && target.y == -13){
          do{
            display_equipment(pcp->equipment());
          }while(getch() != 27); /*ESC*/
          target = pcp->getPos();
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
          log_message((std::string("PC is at ") + std::to_string(pcp->getX()) + ", " + std::to_string(pcp->getY())).c_str()); //TODO: log some useful stats
        }else if (target.x == -14 && target.y == -14){
          target = pcp->getPos();
          int takeoff = generic_prompt(pcp->equipment(), "taken off", 97, 12, "a valid letter", display_equipment, 0);
          if (takeoff >= 0){
            if(!pcp->take_off(takeoff)) log_message("The PC's inventory is full. Drop item(s) using 'd' or expunge item(s) using 'x' and try again" ,0);
          }else log_message("No equipment was removed"); //TODO: check all messages in this block
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
        }else if (target.x == -15 && target.y == -15){
          /*Expunge item*/
          int expunge = generic_prompt(pcp->inventory(), "expunged", 48, 10, "a valid number", print_inventory, 0);
          target = pcp->getPos();
          if (expunge >= 0){
            pcp->drop(expunge);
          }else log_message("No item was expunged");
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
        }else if (target.x == -16 && target.y == -16){
          /*Display item description*/
          target = pcp->getPos();
          bool describing;
          do{
            int display = generic_prompt(pcp->inventory(), "displayed", 48, 10, "a valid number", print_inventory, 0);
            if (display >= 0){
              describing = (item_info(pcp->inventory()[display], "Press any key to go back or ESC to go back to the dungeon") != 27);
            }else break;
          }while(describing);
          
          log_message("Welcome back to the dungeon!");
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
        
        }else if (target.x == -17 && target.y == -17){
          /*visit merchant*/
          target = pcp->getPos();
          if (map[pcp->getY()][pcp->getX()].value != '$') continue;
          std::vector<Item*> wShop;
          int i = chances[SHOP_MAX];
          while (i-->0) addItem(-1, wShop);
          int response = 0;
          do{
            int item_choice = generic_prompt(wShop, "bought", 97, wShop.size(), print_store);
            if (item_choice >= 0){
              log_message("Press 'y' to buy the item, ESC to go to the dungeon or any other button to quit");
              response = item_info(wShop[item_choice], "Press y to accept and buy, ESC to go to the dungeon or any other key to go back");
              if (response == 'y'){
                /*process transaction and break*/
                if (pcp->getPesos() >= wShop[item_choice]->getValue()){
                  Item* new_item = pcp->buy(items.size(), wShop[item_choice]);
                  if (new_item){
                    items.push_back(new_item);
                    log_message(std::string("You just bought ") + new_item->getName() + " for $" + std::to_string(new_item->getValue()) + ", You have $" + std::to_string(pcp->getPesos()) + " left. Hit any key to go back", 0);
                    new_item->revalue(chances[ITEM_REVALUE]);
                    getch();
                    break;
                  }else {
                    log_message(std::string("You dont't have enough space in your inventory to buy ") + wShop[item_choice]->getName() + " hit any key to continue or ESC to quit", 0);
                    if(getch() == 27) break;
                  } 
                }else {
                  log_message(std::string("You dont't have enough resources to buy ") + wShop[item_choice]->getName() + " hit any key to continue or ESC to quit", 0);
                  if(getch() == 27) break;
                }
              }
            }else break;
          }while(response != 27);
          clearShop(wShop);
          log_message("Welcome back to the dungeon!");
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
        }else if (target.x == -18 && target.y == -18){
          target = pcp->getPos();
          do{
            Room boundary = {0, 0, nCols - 1, nRows - 1};
            target = select_position(target, boundary, chars, characters, item_map, items, start, 0);
            if (target.x >= 0 && target.y >= 0){
              int** sight = pcp->setSight(nRows, nCols);
              if (sight[target.y][target.x] != 0){
                bool in_range = (target.x <= pcp->getX() + chances[PC_RAD]) && (target.x >= pcp->getX() - chances[PC_RAD]) && (target.y <= pcp->getY() + chances[PC_RAD]) && (target.y >= pcp->getY() - chances[PC_RAD]);
                if (chars[target.y][target.x] != -1 && in_range){
                  character_info(characters[chars[target.y][target.x]]);
                  getch();
                }
                else if (sight[target.y][target.x] < 0)
                  item_info(items[sight[target.y][target.x] * -1], "Press any key to go back");
                else{
                  std::string cell_type = "Cell type: ";
                  switch(sight[target.y][target.x]){
                    case '.':
                      cell_type += "room cell. Hardness: 0";
                      break;
                    case '#':
                      cell_type += "corridor cell. Hardness: 0";
                      break;
                    case '<':
                      cell_type += "lower-level stair cell type. Hardness: 0";
                      break;
                    case '>':
                      cell_type += "upper-level stair cell type. Hardness: 0";
                      break;
                    case '$':
                      cell_type += "Shop cell. Hardness: 0";
                      break;
                    case ' ':
                      cell_type += "Wall cell. Hardness: " + std::to_string(map[target.y][target.x].hardness); //TODO: hardness is gotten from map not sight. i.e PC is getting info that it can't really see
                      break;
                    default:
                      cell_type += "??? I wonder what that is";
                  }
                  log_message(cell_type + " hit any key to continue",0);
                  getch();
                }
              }else{
                log_message("??? Hmm I can't see that. I wonder what's there, I should go check it out! Hit any key to continue");
                getch();
              }
            }else break;
          }while(true);
          log_message("Welcome back to the dungeon");
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
          target = pcp->getPos();
        }else if (target.x == -19 && target.y == -19){
          target = pcp->getPos();
          //select ranged weapon
          Item* ranged_weapon;
          if (!(ranged_weapon = pcp->equipment()[2])){ //TODO: NOTE: ranged weapon slot is hardcoded
            log_message("You have no ranged weapon equiped", 0);
            continue;
          } 
          //select spot
          uint8_t rad = chances[PC_RAD] + ranged_weapon->getSpecial()/1000;
          Room boundary = {target.x - rad, target.y - rad, (uint8_t)(rad * 2), (uint8_t)(rad * 2)};
          do{
            target = select_position(target, boundary, chars, characters, item_map, items, start, 0); //get input from user
          }while(target.x == -12 && target.y == -12);
          if (target.x != -11 && target.y != -11){
            if (chars[target.y][target.x] != -1){
              if (chars[target.y][target.x] != 0){
                //simulate attack
                int attack = ranged_weapon->getDamageBonus() + ((100.0 + ranged_weapon->getHit())/100.0) * ranged_weapon->getSpecial();
                log_message(std::string("You dealt ") + std::to_string(attack) + " damage to " + static_cast<Monster*>(characters[chars[target.y][target.x]])->getName(), 22);
                if (characters[chars[target.y][target.x]]->takeDamage(attack)){
                  l_monsters--;
                  log_message(std::string("You just killed ") + static_cast<Monster*>(characters[chars[target.y][target.x]])->getName(), 0);
                  chars[target.y][target.x] = -1;
                  //Spawn item if you're lucky
                  item_drop(items, chances[ITEM_SPAWN]*(1 + (ranged_weapon->getSpecial())/25000.0), item_map, target);
                }
              }else{
                log_message("The PC cannot shoot itself with ranged weapons. Hit any key to continue", 0);
                getch();
              }
            }else{
              log_message("There was no character on that cell. Hit any key to continue", 0);
              if (map[target.y][target.x].hardness != 0){
                if (map[target.y][target.x].hardness - (ranged_weapon->getSpecial()/1500) <= 0){
                  map[target.y][target.x].hardness = 0;
                  map[target.y][target.x].value = '#';
                }else map[target.y][target.x].hardness -= (ranged_weapon->getSpecial()/1500);
              }
              getch();
            }
          }else{
            target = pcp->getPos();
            log_message("No position was selected", 0);
            continue;
          }
          target = pcp->getPos();
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
          break;
        }else if (target.x == -20 && target.y == -20){/*potion*/
          Potion* potion = 0;
          target = pcp->getPos();
          //select potion
          int item_idx = generic_prompt(pcp->purse(), "used", 97, 5, "a slot occupied by a potion", display_purse, 0);
          if (item_idx >= 0){
            potion = static_cast<Potion*>(pcp->purse()[item_idx]);
            log_message(std::string("You selected ") + pcp->purse()[item_idx]->getName() + ". Type: " + pcp->purse()[item_idx]->getType(), 0);
            //select spot
            uint8_t rad = chances[PC_RAD] + potion->getRadius(); //TODO: create potion radius to replace PC_RAD
            Room boundary = {target.x - rad, target.y - rad, (uint8_t)(rad * 2), (uint8_t)(rad * 2)};
            do{
              target = select_position(target, boundary, chars, characters, item_map, items, start, 0); //get input from user
            }while(target.x == -12 && target.y == -12);
            if (target.x != -11 && target.y != -11){
              if (chars[target.y][target.x] != -1){
                if (chars[target.y][target.x] != 0){
                  //simulate spell
                  //int attack = ranged_weapon->getDamageBonus() + ((100.0 + ranged_weapon->getHit())/100.0) * ranged_weapon->getSpecial();
                  log_message(std::string("You cast a spell on ") + static_cast<Monster*>(characters[chars[target.y][target.x]])->getName(), 22);
                  // if (characters[chars[target.y][target.x]]->takeDamage(attack)){
                  //   l_monsters--;
                  //   log_message(std::string("You just killed ") + static_cast<Monster*>(characters[chars[target.y][target.x]])->getName(), 0);
                  //   chars[target.y][target.x] = -1;
                  //   //Spawn item if you're lucky
                  //   item_drop(items, chances[ITEM_SPAWN]*(1 + (ranged_weapon->getSpecial())/25000.0), item_map, target);
                  // }
                }else{
                  log_message("The PC cast a spell on itself. Hit any key to continue", 0);
                  getch();
                }
              }else{
                log_message("There was no character on that cell. Hit any key to continue", 0);
                // if (map[target.y][target.x].hardness != 0){
                //   if (map[target.y][target.x].hardness - (ranged_weapon->getSpecial()/1500) <= 0){
                //     map[target.y][target.x].hardness = 0;
                //     map[target.y][target.x].value = '#';
                //   }else map[target.y][target.x].hardness -= (ranged_weapon->getSpecial()/1500);
                // }
                getch();
              }
            }else{
              target = pcp->getPos();
              log_message("No position was selected", 0);
              continue;
            }
            target = pcp->getPos();
            generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
            break;
          }
        }
        else if (map[target.y][target.x].hardness == 0) break;
        target.x = cgetX(curr);
        target.y = cgetY(curr);
      }while(1);
      if (new_dungeon) continue;
      log_message(std::string("Monsters left: ") + std::to_string(l_monsters), 0);
    }else{
      
      /*Telephathy*/
      int curr_room_no = getRoom(dungeon, cgetX(curr), cgetY(curr));
      if (ccheckType(curr, 0x2) || (curr_room_no == getRoom(dungeon, cgetX(pcp), cgetY(pcp)))){ //smart or line of sight //TODO might want to check corridor, this makes the PC visible to all monsters on a corridor when it's on the corridor because getRoom for both will return -1
        target.x = cgetX(pcp);
        target.y = cgetY(pcp);
        last_seen[cgetId(curr) - 1] = target;
        recalculate = recalculate && ccheckType(curr, 0x1);
      }else{ //change to its own if
        if(ccheckType(curr, 0x1) && last_seen[cgetId(curr) - 1].x != -1){
          target = last_seen[cgetId(curr) - 1];
          recalculate = recalculate && ccheckType(curr, 0x1);
        }else{
          /*if the Character is not in a room, it's target is a random posotion in room 0*/
          curr_room_no = (curr_room_no == -1) ? /*rand_gen(0, dungeon.num_rooms)*/0 : curr_room_no;
          target = determine_position(dungeon.rooms[curr_room_no]);
        }
      }
      
      /*Calculate distance to PC*/
      if (recalculate){
        /*Create and initialize queue for calculating monster distances*/
        Queue q;
        queue_init(&q, cell_equals, NULL);
        
        /*Breadth First Search for non-tunelling characters*/
        Cell temp_pc = {target.x, target.y, 0, 0};
        if (target.x > 1000)
          temp_pc.x = temp_pc.x;
        BFS_impl(dist, map, &q, temp_pc);
        
        /* Djikstra for tunelling characters */
        Djikstra_impl(t_dist, map, &q, temp_pc);
        recalculate = 0;
      }
      
      /*Intelligent*/
      if (ccheckType(curr, 0x1)){
          int curr_dist = INT_MAX;
          for (i = cgetY(curr) - 1; i <= cgetY(curr)+1; i++){
            for (j = cgetX(curr) - 1; j <= cgetX(curr)+1; j++){
              if (!(i== cgetY(curr) && j == cgetX(curr)) && (i > 0 && j > 0 && i < nRows - 1 && j < nCols - 1)){ //TODO nRows-1 vs nRows check the whole code
                int es_dist = (ccheckType(curr, 0x4)) ? t_dist[i][j] : dist[i][j]; //if tunnelling
                if (es_dist < curr_dist){
                  curr_dist = es_dist;
                  target.x = j;
                  target.y = i;
                }
              }
            }
          }
      }else{/*Determine monster's next position*/
          if (cgetX(curr) < target.x){
            target.x = (((cgetX(curr) + 1) < nCols-1) && ((ccheckType(curr, 0x4)) || map[cgetY(curr)][cgetX(curr) + 1].hardness == 0) ) ? cgetX(curr) + 1 : cgetX(curr); //can move and/or tunnel
          }else if (cgetX(curr) > target.x){
            target.x = (((cgetX(curr) - 1) > 0) && ((ccheckType(curr, 0x4)) || map[cgetY(curr)][cgetX(curr) - 1].hardness == 0)) ? cgetX(curr) - 1 : cgetX(curr);
          }
          if (cgetY(curr) < target.y){
            target.y = (((cgetY(curr) + 1) < nRows-1) && ((ccheckType(curr, 0x4)) || map[cgetY(curr) + 1][cgetX(curr)].hardness == 0)) ? cgetY(curr) + 1 : cgetY(curr);
          }else if (cgetY(curr) > target.y){
            target.y = (((cgetY(curr) - 1) > 0) && ((ccheckType(curr, 0x4)) || map[cgetY(curr) - 1][cgetX(curr)].hardness == 0)) ? cgetY(curr) - 1 : cgetY(curr);
          }
      }
      
      /*Erratic*/
      if ((ccheckType(curr, 0x8)) && rand_gen(0,1)){
        char found = 0;
        while(!found){
          target.x = rand_gen(cgetX(curr) - 1,cgetX(curr) + 1);
          target.y = rand_gen(cgetY(curr) - 1, cgetY(curr) + 1);
          found = ((!(target.y == cgetY(curr) && target.x == cgetX(curr)) && (target.x > 0 && target.y > 0 && target.y < nRows-1 && target.x < nCols-1)) && (ccheckType(curr, 0x4) || map[target.y][target.x].hardness == 0));
        }
      }
      
    }
    
    /*Attempt to move*/
    int attack = -1;
    if (map[target.y][target.x].hardness != 0){
      map[target.y][target.x].hardness = ((map[target.y][target.x].hardness - 85) < 0) ? 0 : map[target.y][target.x].hardness - 85;
      recalculate = 1;
    }
    if (map[target.y][target.x].hardness == 0){
      /*There is someone on the space to move to*/
      if (chars[target.y][target.x] != -1){
        if (chars[target.y][target.x] != curr->getId()){ //Checking if a character is staying on the same spot OR curr->getPos == target
            //PC is attacking or getting attacked
            if (curr == pcp || characters[chars[target.y][target.x]] == pcp){
              int prevPC_HP = pcp->getHp();
              attack = curr->attack();
              std::string char_name = (curr == pcp) ? static_cast<Monster*>(characters[chars[target.y][target.x]])->getName() : "";
              if (characters[chars[target.y][target.x]]->takeDamage(attack)){
                if (curr == pcp){
                  log_message(std::string("You just killed ") + static_cast<Monster*>(characters[chars[target.y][target.x]])->getName(), 0);
                  chars[curr->getY()][curr->getX()] = -1;
                  curr->setPos(&target.x, &target.y);
                  chars[curr->getY()][curr->getX()] = curr->getId();
                  /*Killed all monsters. Final render*/
                  if(!(--l_monsters)) {
                    Pair start = {cgetX(curr) - 40, cgetY(curr) - 10};
                    generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
                    break;
                  }
                  //Spawn item/earn money if you're lucky
                  item_drop(items, chances[ITEM_SPAWN], item_map, target);
                  if (probability(chances[PESOS_DROP])) pcp->earn(characters[chars[target.y][target.x]]->attack()/2);
                }else {
                  log_message(std::string("You got killed by ") + static_cast<Monster*>(curr)->getName(), 23);
                  //TODO: just break?
                  moveCharacter(curr, target, chars);
                  Pair start = {curr->getX() - 40, curr->getY() - 10};
                  generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
                  // items = (Item**)delete_items(items, numitem);
                  pcp->unequip_all();
                  delete_Characters(characters, nummon + 1);
                  remove_items(items);
                  endgame(&dungeon, &evt, "The PC is dead :(");
                  return 0;
                }
                if (curr != pcp) log_message(std::string("Current HP:") + std::to_string(prevPC_HP) + " --> " + std::to_string(pcp->getHp()), 22);
              }
              /*Log messages*/
              if (curr == pcp)
                log_message(std::string("You dealt ") + std::to_string(attack) + " damage to " + char_name, 22);
              else {
                log_message(std::string(static_cast<Monster*>(curr)->getName()) + " dealt "  + std::to_string(attack) + " damage to you. (HP: " + std::to_string(prevPC_HP) + "-->" + std::to_string(pcp->getHp()) + ")", 23);
              }
            }else{/*Monster push/swap monster*/
            // log_message((std::string(static_cast<Monster*>(curr)->getName()) + " kicked out " + static_cast<Monster*>(characters[chars[target.y][target.x]])->getName()).c_str(), 23, false);
            //TODO: make kick position random
            bool swap = true;
            for (int p = target.y - 1; (p <= target.y + 1) && swap; p++){
              for (int q = target.x - 1; q <= target.x + 1; q++){
		            if (p == 0 || q == 0 || p == (nRows - 1) || q == (nCols - 1) || chars[p][q] != -1 || map[p][q].hardness != 0) continue;
		            Pair kick_spot = {q, p};
		            moveCharacter(characters[chars[target.y][target.x]], kick_spot, chars); //move the monster that's on the way
		            moveCharacter(curr, target, chars); //move the monster to it's target position
		            swap = false;
		            break;
              }
            }
            if (swap){
              Pair prev = {curr->getX(), curr->getY()};
              moveCharacter(characters[chars[target.y][target.x]], prev, chars);
              chars[target.y][target.x] = curr->getId();
              curr->setPos(&target.x, &target.y);
            }
          }
          
        }
      }else {
        moveCharacter(curr, target, chars);
      }
      /*Pick/Destroy/walk over items*/
      if (/*attack == -1 && */item_map[target.y][target.x] != -1 && (curr == pcp/* || monster can destroy objects*/)){
        /*TODO: add item to inventory or destroy*/
        if(curr == pcp && pcp->pick(items[item_map[target.y][target.x]])) {
          log_message((std::string("You just picked ") + items[item_map[target.y][target.x]]->getName()).c_str());
          items[item_map[target.y][target.x]]->unstack(item_map[target.y][target.x]);
        }else if (curr != pcp/* && check for destroy*/){
          items[item_map[target.y][target.x]]->unstack(item_map[target.y][target.x]);
        }
      }
      if (map[target.y][target.x].value != '.' && map[target.y][target.x].value != '<' && map[target.y][target.x].value != '>' && map[target.y][target.x].value != '$') map[target.y][target.x].value = '#'; //TODO add condition to check if it was a wall passing monster
    }
    recalculate = (cgetId(curr) == 0) ? 1 : 0; //TODO: don not recalculate if no move was made
    pace[cgetId(curr)] +=  1000/(curr->getSpeed() ? curr->getSpeed() : 1);
    change_priority(&evt, curr, pace[cgetId(curr)]);
  }while(l_monsters || solo);
  
  /*Print win message*/
  if (nummon && !l_monsters) {
    delete_Characters(characters, nummon + 1);
    // items = (Item**)delete_items(items, numitem);
    pcp->unequip_all();
    remove_items(items);
    endgame(&dungeon, &evt, "PC killed em all");
    return 0;
  }
  
  empty_queue(&evt); //This queue is emptied in end_game
  
  /*Free all mallocs before exiting*/
  free(dungeon.rooms);
  endwin();
	return 0;
}

int rand_gen(int min, int max){
	return (max >= min) ? (rand() % (max-min+1)) + min : -1;
}

int cell_equals(void* c1, void* c2){
  Cell oldCell = *(Cell *)c1;
  Cell newCell = *(Cell *)c2;
  return (oldCell.x == newCell.x && oldCell.y == newCell.y);
}

int char_equals(void* c1, void* c2){
  return cgetId((Character *)c1) == cgetId((Character *)c2);
  // return (*(Character *)c1).id == (*(Character *)c2).id;
}

void print_Character(void* character){
  printf("id: %d => %c speed is %d\n",cgetId((Character *)character), cgetSymbol((Character *)character), cgetSpeed((Character *)character));
}

void endgame(Dungeon* dungeon, Queue* game_queue, const char* endmessage){
  Player::deletePlayer();
  free(dungeon->rooms);
  empty_queue(game_queue);
  object_parser::private_wrapper::delete_objects();
  log_message("Game ended. Hit any button to quit");
  getch();/*TODO, i don't call refersh but it works*/
  endwin();
  system("clear");
  /*Display some nice stats*/
  puts(endmessage);
}

void delete_Characters(Character* characters[], int num_characters){
  int i;
  cfreeSight(characters[0], nRows); /*NOTE: PC's sight is freed here*/
  // Player::getPlayer()->clearSlots(true, true);
  for (i = 1; i < num_characters; i++){
    deleteCharacter(characters[i]);
    characters[i] = 0;
  }
}

Pair moveCharacter(Character* curr, const Pair& target, int chars[][nCols]){
	Pair oldPos = {curr->getX(), curr->getY()};
	chars[curr->getY()][curr->getX()] = -1;
	curr->setPos(&target.x, &target.y);
	chars[curr->getY()][curr->getX()] = curr->getId();
	return oldPos;
}

bool probability(int chance){
  return rand_gen(1, 100) <= chance;
}

//1490045401 --nummon=10
//1490063401 visible monster
//new stairs 1490647963, 1491369464, 1492046428
//1491422112 stairs
//1491429247  stairs
//1491608195 new stairs
//1491689514 generates monsters in room 1
//149176584 an invisible nerf dagger on your way up
//1491769640 --numitem=300 -nummon=2 test 2 monsters kicking each other out
//1492046620 fast
//1492365593 pc doesn't initially show up
//1492375222 close shop
//1492395756 shop/vorpal blade
//1492485638 corner
//1492605482 PC is invisible in turn 1 in fow mode
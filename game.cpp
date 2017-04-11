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
// #include "Character.h"

#include "Monster.h"
#include "game.h" //TODO: forwatd declration problems when this is the first include
#include "dungeon.h"
// #include "object_parser.h"
#include "Item.h"
#include "Player.h"
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
  struct option longopts[] = {
    {"load", optional_argument, NULL, 'l'},
    {"save", no_argument, &save, 1},
    {"seed", optional_argument, NULL, 'e'},
    {"pc", optional_argument, NULL, 'p'},
    {"nummon", optional_argument, NULL, 'm'},
    {"numitem", optional_argument, NULL, 'i'},
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
  
  object_parser::parser_init("monster");
  object_parser::start_parser(strcat(strcpy(load_file, getenv("HOME")), "/.rlg327/monster_desc.txt"));
  object_parser::complete_parse();

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
  
  /*Ncurses start*/
  int col = 0;
  bool fullscreen = false;
  ncurses_init();
  
  /*Monster magic and initialize random pc if no valid command line argument was entered*/
  Queue evt;
  queue_init(&evt, char_equals, print_Character);
  
  /*Initialize all Characters (PC and monster)*/
  if (nummon_flag == 0) nummon = rand_gen(dungeon.num_rooms, dungeon.num_rooms*2);
  int l_monsters = nummon;
  Character* characters[nummon + 1];
  Pair last_seen[nummon];
  memset(last_seen, - 1, sizeof(Pair)*nummon);

  /*Dungeon monster setup*/
  unsigned int pace[nummon+1];
  addCharcters(&dungeon, &evt, nummon, characters, chars, pace);
  /*Change PC position to user's choice if available*/
  if (pc_type){/*TODO: there might be a bug with this flag, try seed:1489964342 on --pc="30,30"*/
    chars[cgetY(characters[0])][cgetX(characters[0])] = -1;
    csetPos(characters[0], &(pc.x), &(pc.y));
    chars[cgetY(characters[0])][cgetX(characters[0])] = 0;
  }
  
  /*Items*/
  object_parser::parser_init("item");
  object_parser::start_parser(strcat(strcpy(load_file, getenv("HOME")), "/.rlg327/object_desc.txt"));
  object_parser::complete_parse();
  // int numitem = 20; /*Will be changed in add items TODO: change initializer to declaration*/
  int item_map[nRows][nCols];
  Item** items = 0;
  items = addItems(&dungeon, items, item_map, &numitem);

  add_stairs(&dungeon, map);
  char recalculate = 1;
  // Character* pcp = characters[0];
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
          items = (Item**)delete_items(items, numitem);
          delete_Characters(characters, nummon + 1);
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
          items = (Item**)delete_items(items, numitem);
          delete_dungeon(&dungeon, &evt, map);
          delete_Characters(characters, nummon + 1);
          
          create_dungeon(&dungeon, map, room_cells);
          add_stairs(&dungeon, map);
          addCharcters(&dungeon, &evt, nummon, characters, chars, pace);
          items = addItems(&dungeon, items, item_map, &numitem); //TODO
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
        }else if(target.x == -7 && target.y == -7){
          do{
            print_inventory(pcp->inventory());
          }while(getch() != 27); /*ESC*/
          target = pcp->getPos();
          // render(chars, characters, item_map, items, start, 0);
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
          log_message((std::string("PC is at ") + std::to_string(pcp->getX()) + ", " + std::to_string(pcp->getY())).c_str()); //TODO: log some useful stats
        }else if(target.x == -8 && target.y == -8){
          int drop = generic_prompt(pcp->inventory(), "removed ", 48, 10, print_inventory);;//drop_from_inventory(pcp->inventory());
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
        }else if (target.x == -10 && target.y == -10){
          target = pcp->getPos();//TODO: this happens in case -3 and shuld prolly happen in every case, refactor?
          continue;
        }else if (target.x == -11 && target.y == -11){
          target = pcp->getPos();
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen = !fullscreen);
          log_message((std::string("Switched to ") + (fullscreen ? "complete" : "partial") + " dungeon display").c_str());
          continue;
        }else if (target.x == -12 && target.y == -12){ /*Wear equipment*/
          int wear = generic_prompt(pcp->inventory(), "worn ", 48, 10, print_inventory);
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
          int takeoff = generic_prompt(pcp->equipment(), "taken off ", 97, 12, display_equipment);
          if (takeoff >= 0){
            if(!pcp->take_off(takeoff)) log_message("The PC's inventory is full. Drop item(s) using 'd' or expunge item(s) using 'x' and try again" ,0);
          }else log_message("No equipment was removed"); //TODO: check all messages in this block
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
        }else if (target.x == -15 && target.y == -15){
          /*Expunge item*/
          int expunge = generic_prompt(pcp->inventory(), "expunged ", 48, 10, print_inventory);
          target = pcp->getPos();
          if (expunge >= 0){
            pcp->drop(expunge);
          }else log_message("No item was expunged");
          generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
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
      // chars[cgetY(curr)][cgetX(curr)] = -1;//TODO: do not do this until fight is over
      /*If PC is killed*/
      /*if ((curr != pcp) && (pcp->getX() == target.x && cgetY(pcp) == target.y)){
        //Move and make final render
        chars[cgetY(curr)][cgetX(curr)] = -1;
        curr->setPos(&(target.x), &(target.y));
        chars[target.y][target.x] = cgetId(curr);

        Pair start = {cgetX(curr) - 40, cgetY(curr) - 10};
        generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
        items = (Item**)delete_items(items, numitem);
        delete_Characters(characters, nummon + 1);
        endgame(&dungeon, &evt, "The PC is dead :(");return 0;
      }
      */
      /*There is someone on the space to move to*/
      if (chars[target.y][target.x] != -1){
        if (chars[target.y][target.x] != curr->getId()){ //Checking if a character is staying on the same spot OR curr->getPos == target
          // ckillCharacter(characters[chars[target.y][target.x]]);

            //PC is attacking or getting attacked
            if (curr == pcp || characters[chars[target.y][target.x]] == pcp){
              attack = curr->attack();
              if (curr == pcp)
                log_message(std::string("You dealt ") + std::to_string(attack) + " damage to " + static_cast<Monster*>(characters[chars[target.y][target.x]])->getName(), 24);
              else {
                log_message(std::string(static_cast<Monster*>(curr)->getName()) + " dealt "  + std::to_string(attack) + " damage to you", 22);
                log_message(std::string("Current HP:") + std::to_string(pcp->getHp()) + " --> " + std::to_string(pcp->getHp() - attack), 23);
              }
              if (characters[chars[target.y][target.x]]->takeDamage(attack)){
                if (curr == pcp){
                  log_message(std::string("You just killed ") + static_cast<Monster*>(characters[chars[target.y][target.x]])->getName(), 0);
                  chars[curr->getY()][curr->getX()] = -1;
                  curr->setPos(&target.x, &target.y);
                  chars[curr->getY()][curr->getX()] = curr->getId();
                  /*Killed all monsters. Final render*/
                  if(!(--l_monsters)) {
                    Pair start = {cgetX(curr) - 40, cgetY(curr) - 10};
                    // chars[target.y][target.x] = pcp->getId();
                    moveCharacter(curr, target, chars);//TODO e no need for this, already done before this if block
                    generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
                    break;
                  }
                }else {
                  log_message(std::string("You got killed by ") + static_cast<Monster*>(characters[chars[target.y][target.x]])->getName(), 0);
                  //TODO: just break?
                  moveCharacter(curr, target, chars);
                  Pair start = {curr->getX() - 40, curr->getY() - 10};
                  generic_render(map, chars, characters, item_map, items, start, 0, fullscreen);
                  items = (Item**)delete_items(items, numitem);
                  delete_Characters(characters, nummon + 1);
                  endgame(&dungeon, &evt, "The PC is dead :(");return 0;
                }
              }
            }else{/*Monster push/swap monster*/
            log_message((std::string(static_cast<Monster*>(curr)->getName()) + " kicked out " + static_cast<Monster*>(characters[chars[target.y][target.x]])->getName()).c_str());
            //TODO: make kick position random
            bool swap = true;
            for (int p = target.y - 1; (p <= target.y + 1) && swap; p++){
              for (int q = target.x - 1; q <= target.x + 1; q++){
		            if (p == 0 || q == 0 || p == (nRows - 1) || q == (nCols - 1) || chars[p][q] != -1 || map[p][q].hardness != 0) continue;
		            // chars[p][q] = chars[target.y][target.x];//get ID of monster characters[chars[target.y][target.x]]->getId();
		            // characters[chars[p][q]]->setPos(&q, &p);
		            // chars[cgetY(curr)][cgetX(curr)] = -1; //TODO e remove
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
        // chars[cgetY(curr)][cgetX(curr)] = -1; //spot is empty TODO e
        // curr->setPos(&target.x, &target.y);
        // // characters[chars[target.y][target.x]]->setPos(&(target.x), &(target.y));
        // chars[target.y][target.x] = curr->getId();
      }
      /*if ((chars[target.y][target.x] != -1) && (chars[target.y][target.x] != cgetId(curr))){ //TODO: change from kill to fighr
        //FIGHT HERE
        ckillCharacter(characters[chars[target.y][target.x]]);
        if (curr == pcp){
          //PC is the one killing
          log_message((std::string("You just killed ") + static_cast<Monster*>(characters[chars[target.y][target.x]])->getName()).c_str());
        }
        if(!(--l_monsters)) break;
      }*/
      // chars[target.y][target.x] = cgetId(curr); //TODO e only if person moves
      /*NOTE: this assumes that you killed the person and moved*/
      if (attack == -1 && item_map[target.y][target.x] != -1/*&& you can pick/destroy && the person chose to move*/){ /*item here, you can pick/destroy, you moved*/
        /*TODO: add item to inventory or destroy*/
        if(curr == pcp && pcp->pick(items[item_map[target.y][target.x]])) {
          log_message((std::string("You just picked ") + items[item_map[target.y][target.x]]->getName()).c_str());
          items[item_map[target.y][target.x]]->unstack(item_map[target.y][target.x]);
        }else if (curr != pcp/* && check for destroy*/){
          items[item_map[target.y][target.x]]->unstack(item_map[target.y][target.x]);
        }
      }
      if (map[target.y][target.x].value != '.' && map[target.y][target.x].value != '<' && map[target.y][target.x].value != '>') map[target.y][target.x].value = '#'; //TODO add condition to check if it was a wall passing monster
      // curr->setPos(&target.x, &target.y); //TODO e: only if fight is won
    }
    recalculate = (cgetId(curr) == 0) ? 1 : 0;
    pace[cgetId(curr)] +=  1000/cgetSpeed(curr);
    change_priority(&evt, curr, pace[cgetId(curr)]);
  }while(l_monsters || solo);
  
  /*Print win message*/
  if (nummon && !l_monsters) {
    delete_Characters(characters, nummon + 1);
    items = (Item**)delete_items(items, numitem);
    endgame(&dungeon, &evt, "PC killed em all"); return 0;
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
  free(dungeon->rooms);
  empty_queue(game_queue);
  object_parser::private_wrapper::delete_objects();
  int row,col;
  getmaxyx(stdscr, row, col); /*Longindex is passed here but this macro function requires an argument*/
  move(0, 0);
  clrtoeol();
  mvprintw(0/row, col/2 - (strlen(endmessage) + 22)/2, "%s %s",endmessage, "hit any button to quit"); /*variable row is only used to avoid variable-not-used-warning*/
  getch();/*TODO, i don't call refersh but it works*/
  endwin();
  system("clear");
  /*Display some nice stats*/
  puts(endmessage);
// 	exit(0);
}

void delete_Characters(Character* characters[], int num_characters){
  int i;
  cfreeSight(characters[0], nRows);
  Player::deletePlayer();
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
//1490045401 --nummon=10
//1490063401 visible monster
//1490647963 stairs
//new stairs 1491369464
//1491422112 stairs
//1491429247  stairs
//1491608195 new stairs
//1491689514 generates monsters in room 1
//149176584 an invisible nerf dagger on your way up
//1491769640 --numitem=300 -nummon=2 test 2 monsters kicking each other out
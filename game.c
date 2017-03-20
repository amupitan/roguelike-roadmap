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

#include "queue.h"
#include "Player.h"
#include "game.h"

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
  int option, load = 0, save = 0, seed = (unsigned) time(NULL), nummon = 0, solo = 0;
  char load_file[100];
  struct option longopts[] = {
    {"load", optional_argument, NULL, 'l'},
    {"save", no_argument, &save, 1},
    {"seed", optional_argument, NULL, 'e'},
    {"pc", optional_argument, NULL, 'p'},
    {"nummon", optional_argument, NULL, 'm'},
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

  /*Ncurses start*/
  int col = 0;
  ncurses_init();
  
  /*Monster magic and initialize random pc if no valid command line argument was entered*/
  Queue evt;
  queue_init(&evt, char_equals, print_player);
  
  /*Initialize all players (PC and monster)*/
  if (nummon_flag == 0) nummon = rand_gen(dungeon.num_rooms, dungeon.num_rooms*2);
  int l_monsters = nummon;
  Player* characters[nummon + 1];
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

  add_stairs(&dungeon, map);
  char recalculate = 1;
  Player* pcp = characters[0];
  do{
    pcp = characters[0];
    Player* p_curr;
    p_curr = (Player *)peek(&evt, &p_curr);
    // Player curr = *p_curr;
    Pair target = {cgetX(p_curr), cgetY(p_curr)};// {cgetX(p_curr), cgetY(p_curr)};
    if (cgetValue(p_curr) == -1){ //it was killed
      dequeue(&evt);
      continue;
    }
    int new_dungeon = 0;
    /*Determine next position of character*/
    if (p_curr == pcp){
      /* PC stuff */
      // while (target.x < 1 || (target.x > nCols - 1) || target.y < 1 || target.y > nRows - 1 || (target.x == cgetX(p_curr) && target.y == cgetY(p_curr))){ //Reverse psych lol :) and the PC must move, cannot stay in the same spot, just because it's lame to stay in the same place
      //   target.x = rand_gen(cgetX(p_curr) - 1, cgetX(p_curr) + 1);
      //   target.y = rand_gen(cgetY(p_curr) - 1, cgetY(p_curr) + 1);
      // }
      updateSight(pcp, map);
      Pair start = {cgetX(p_curr) - 40, cgetY(p_curr) - 10};
      pc_render_partial(map, chars, characters, start, NULL); //TODO!!!
      do{
        target = *(Pair *)getInputC(&target);
        if (target.x == -1 && target.y == -1){
          delete_players(characters, nummon + 1);
          endgame(&dungeon, &evt, "Game ended");
        }
        if (target.x == -2 && target.y == -2) {
          /*Enter look mode*/
          Pair look = start;
          do{
            int ctrl = 0; /*1- end look mode*/
            look = *(look_mode(&look, &ctrl));
            if (ctrl == 1){
              pc_render_partial(map, chars, characters, start, NULL);
              break;
            }
            pc_render_partial(map, chars, characters, look, &look);
          }while(1);
        }
        else if ((target.x == -3 && target.y == -3) || (target.x == -4 && target.y == -4)){
          /*use stairs: if not stair continue*/
          if ((target.x == -3 && map[cgetY(p_curr)][cgetX(p_curr)].value != '>') || (target.x == -4 && map[cgetY(p_curr)][cgetX(p_curr)].value != '<')) {/*check up/down stairs*/
            target.x = cgetX(p_curr);
            target.y = cgetY(p_curr); /*set target to original position*/
            continue;
          }
          // csetType(pcp, 0);
          l_monsters = nummon;
          delete_dungeon(&dungeon, &evt, map);
          delete_players(characters, nummon + 1);
          create_dungeon(&dungeon, map, room_cells);
          add_stairs(&dungeon, map);
          addCharcters(&dungeon, &evt, nummon, characters, chars, pace);
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
          sprintf(stat_msg, "PC is at %d, %d. Number of rooms: %d", cgetX(p_curr), cgetY(p_curr), dungeon.num_rooms);
          log_message(stat_msg);
          free(stat_msg);
        }else if (target.x == -10 && target.y == -10){
          continue;
        }
        else if (map[target.y][target.x].hardness == 0) break;
        target.x = cgetX(p_curr);
        target.y = cgetY(p_curr);
      }while(1);
      if (new_dungeon) continue;
      char mon_log[20];
      sprintf(mon_log, "Monsters alive: %d", l_monsters);
      log_message(mon_log);
    }else{
      
      /*Telephathy*/
      int curr_room_no = getRoom(dungeon, cgetX(p_curr), cgetY(p_curr));
      if (ccheckType(p_curr, 0x2) || (curr_room_no == getRoom(dungeon, cgetX(pcp), cgetY(pcp)))){ //smart or line of sight //TODO might want to check corridor
        target.x = cgetX(pcp);
        target.y = cgetY(pcp);
        last_seen[cgetId(p_curr) - 1] = target;
        recalculate = recalculate && ccheckType(p_curr, 0x1);
      }else{ //change to its own if
        if(ccheckType(p_curr, 0x1) && last_seen[cgetId(p_curr) - 1].x != -1){
          target = last_seen[cgetId(p_curr) - 1];
          recalculate = recalculate && ccheckType(p_curr, 0x1);
        }else{
          /*if the player is not in a room, it's target is a random posotion in room 0*/
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
      if (ccheckType(p_curr, 0x1)){
          int curr_dist = INT_MAX;
          for (i = cgetY(p_curr) - 1; i <= cgetY(p_curr)+1; i++){
            for (j = cgetX(p_curr) - 1; j <= cgetX(p_curr)+1; j++){
              if (!(i== cgetY(p_curr) && j == cgetX(p_curr)) && (i > 0 && j > 0 && i < nRows - 1 && j < nCols - 1)){ //TODO nRows-1 vs nRows check the whole code
                int es_dist = (ccheckType(p_curr, 0x4)) ? t_dist[i][j] : dist[i][j]; //if tunnelling
                if (es_dist < curr_dist){
                  curr_dist = es_dist;
                  target.x = j;
                  target.y = i;
                }
              }
            }
          }
      }else{/*Determine monster's next position*/
          if (cgetX(p_curr) < target.x){
            target.x = (((cgetX(p_curr) + 1) < nCols-1) && ((ccheckType(p_curr, 0x4)) || map[cgetY(p_curr)][cgetX(p_curr) + 1].hardness == 0) ) ? cgetX(p_curr) + 1 : cgetX(p_curr); //can move and/or tunnel
          }else if (cgetX(p_curr) > target.x){
            target.x = (((cgetX(p_curr) - 1) > 0) && ((ccheckType(p_curr, 0x4)) || map[cgetY(p_curr)][cgetX(p_curr) - 1].hardness == 0)) ? cgetX(p_curr) - 1 : cgetX(p_curr);
          }
          if (cgetY(p_curr) < target.y){
            target.y = (((cgetY(p_curr) + 1) < nRows-1) && ((ccheckType(p_curr, 0x4)) || map[cgetY(p_curr) + 1][cgetX(p_curr)].hardness == 0)) ? cgetY(p_curr) + 1 : cgetY(p_curr);
          }else if (cgetY(p_curr) > target.y){
            target.y = (((cgetY(p_curr) - 1) > 0) && ((ccheckType(p_curr, 0x4)) || map[cgetY(p_curr) - 1][cgetX(p_curr)].hardness == 0)) ? cgetY(p_curr) - 1 : cgetY(p_curr);
          }
      }
      
      /*Erratic*/
      if ((ccheckType(p_curr, 0x8)) && rand_gen(0,1)){
        char found = 0;
        while(!found){
          target.x = rand_gen(cgetX(p_curr) - 1,cgetX(p_curr) + 1);
          target.y = rand_gen(cgetY(p_curr) - 1, cgetY(p_curr) + 1);
          found = ((!(target.y == cgetY(p_curr) && target.x == cgetX(p_curr)) && (target.x > 0 && target.y > 0 && target.y < nRows-1 && target.x < nCols-1)) && (ccheckType(p_curr, 0x4) || map[target.y][target.x].hardness == 0));
        }
      }
      
    }
    
    /*Attempt to move*/
    if (map[target.y][target.x].hardness != 0){
      map[target.y][target.x].hardness = ((map[target.y][target.x].hardness - 85) < 0) ? 0 : map[target.y][target.x].hardness - 85;
      recalculate = 1;
    }
    if (map[target.y][target.x].hardness == 0){
      chars[cgetY(p_curr)][cgetX(p_curr)] = -1;
      /*If PC is killed*/
      if ((cgetId(p_curr) != 0)&&(cgetX(pcp) == target.x && cgetY(pcp) == target.y)){
        /*Move and make final render*/
        csetPos(p_curr,  &(target.x), NULL);
        csetPos(p_curr, NULL,  &(target.y));
        chars[target.y][target.x] = cgetId(p_curr);

        Pair start = {cgetX(p_curr) - 40, cgetY(p_curr) - 10};
        pc_render_partial(map, chars, characters, start, NULL); //TODO, fix start position!!!

        delete_players(characters, nummon + 1);
        endgame(&dungeon, &evt, "The PC is dead :(");
      }
      if ((chars[target.y][target.x] != -1) && (chars[target.y][target.x] != cgetId(p_curr))){ //weird stuff.
        ckillPlayer(characters[chars[target.y][target.x]]);
        if(!(--l_monsters)) break;
      }
      chars[target.y][target.x] = cgetId(p_curr);
      if (map[target.y][target.x].value != '.' && map[target.y][target.x].value != '<' && map[target.y][target.x].value != '>') map[target.y][target.x].value = '#';
      csetPos(p_curr,  &(target.x), NULL);
      csetPos(p_curr, NULL,  &(target.y));
    }
    recalculate = (cgetId(p_curr) == 0) ? 1 : 0;
    pace[cgetId(p_curr)] +=  1000/cgetSpeed(p_curr);
    change_priority(&evt, p_curr, pace[cgetId(p_curr)]);
  }while(l_monsters || solo);
  
  /*Print win message*/
  if (nummon && !l_monsters) {
    delete_players(characters, nummon + 1);
    endgame(&dungeon, &evt, "PC killed em all");
  }
  
  empty_queue(&evt);
  
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
  return cgetId((Player *)c1) == cgetId((Player *)c2);
  // return (*(Player *)c1).id == (*(Player *)c2).id;
}

void print_player(void* player){
//   Player player_c = *(Player *)player;
//   printf("id: %d => %c speed is %d\n",player_c.id, player_c.value, player_c.speed);
  printf("id: %d => %c speed is %d\n",cgetId((Player *)player), cgetValue((Player *)player), cgetSpeed((Player *)player));
}


void ncurses_init(){
  initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* To get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
	start_color();			/* Start color 			*/
	/*Initialize colors*/
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);
}

void endgame(Dungeon* dungeon, Queue* game_queue, const char* endmessage){
  free(dungeon->rooms);
  empty_queue(game_queue);
  int row,col;
  getmaxyx(stdscr, row, col); /*Longindex is passed here but this macro function requires an argument*/
  move(0, 0);
  clrtoeol();
  mvprintw(0/row, col/2 - (strlen(endmessage) + 22)/2, "%s %s",endmessage, "hit any button to quit"); /*variable row is only used to avoid variable-not-used-warning*/
  getch();/*TODO, i don't call refersh but it works*/
  endwin();
  // system("clear");
  /*Display some nice stats*/
  puts(endmessage);
	exit(0);
}

/*Get input in control mode*/
Pair* getInputC(Pair* target){ /*TODO: make void?*/
  // int pos = getch();
  switch(getch()){
    case 'y':
    case '7':
      /*upper-left*/
      target->y--;
      target->x--;
      break;
    case 'k':
    case '8':
    case KEY_UP: /*TODO remove?*/
      /*up*/
      target->y--;
      break;
    case 'u':
    case '9':
      /*upper-right*/
      target->y--;
      target->x++;
      break;
    case 'l':
    case '6':
    case KEY_RIGHT: /*TODO remove?*/
      /*right*/
      target->x++;
      break;
    case 'n':
    case '3':
      /*lower-right*/
      target->y++;
      target->x++;
      break;
    case 'j':
    case '2':
    case KEY_DOWN: /*TODO remove?*/
      /*down*/
      target->y++;
      break;
    case 'b':
    case '1':
      /*lower-left*/
      target->x--;
      target->y++;
      break;
    case 'h':
    case '4':
    case KEY_LEFT: /*TODO remove?*/
      /*left*/
      target->x--;
      break;
    case ' ':
    case '5':
      /*rest*/
      break;
    case '>':
      /*attempt to go downstairs*/
      target->x = target->y = -3;
      break;
    case '<':
      /*attempt to go upstairs*/
      target->x = target->y = -4;
      break;
    case 'L':
      /*Enter look mode*/
      target->x = target->y = -2;
      break;
    case 'Q':
      /*Quit the game*/
      target->x = target->y = -1;
      break;
    case 'S':
      /*Display seed*/
      target->x = target->y = -5;
      break;
    case 'R':
      /*Display number of room*/
      target->x = target->y = -6;
      break;
    default:
      target->x = target->y = -10;
      break;
  }
  return target;
}

Pair* look_mode(Pair *target, int* control_mode){ //TODO: uses hardcoded width/height
  switch(getch()){
    case 'k':
    case '8':
    case KEY_UP: /*TODO remove?*/
      /*move up*/
      target->y -= 10;
      break;
    case 'l':
    case '6':
    case KEY_RIGHT: /*TODO remove?*/
      /*move right*/
      target->x += 40;
      break;
    case 'j':
    case '2':
    case KEY_DOWN: /*TODO remove?*/
      /*move down*/
      target->y += 10;
      break;
    case 'h':
    case '4':
    case KEY_LEFT: /*TODO remove?*/
      /*move left*/
      target->x -= 40;
      break;
    case 'Q':
      /*Quit the game*/
      ungetch('Q');
    case 27:
      /*go back to control mode*/
      *control_mode = 1;
      break;
    default:
      break;
  }
  return target;
}

void delete_dungeon(Dungeon* dungeon, Queue* evt, Cell map[][nCols]){
  int i,j;
  /*free dungeon rooms*/
  free(dungeon->rooms);
  dungeon->num_rooms = 0;
  /*free queue*/
  empty_queue(evt);
  /*Reset map*/
  for (i = 0; i < nRows; i++){
		for (j =0; j < nCols; j++){
			map[i][j].value = 32;
			map[i][j].hardness = (i == 0 || j == 0 || i == nRows-1 || j == nCols-1) ? 255 : rand_gen(1,254);
		}
	}
	
}

/*This function assumes that there are at least 2 rooms*/
void add_stairs(Dungeon* dungeon, Cell map[][nCols]){
  int i;
	int n_stairs = rand_gen(2,4);
	for(i = 0; i < n_stairs; i++){
	  /*create upstair in random room then downstair in the next room */
	  int room_no = rand_gen(0, dungeon->num_rooms - 2);
	  Pair pos = determine_position(dungeon->rooms[room_no]);
	  map[pos.y][pos.x].value = '<';
	  pos = determine_position(dungeon->rooms[room_no + 1]);
	  map[pos.y][pos.x].value = '>';
	}
}

void log_message(const char* message){
  int row, col;
  move(0, 0);
  clrtoeol();
  getmaxyx(stdscr, row, col); /*Longindex is passed here but this macro function requires an argument*/
  mvprintw(0/row, (col - strlen(message))/2, message);
  /*TODO: call refresh()?*/
}

void delete_players(Player* characters[], int num_characters){
  int i;
  cfreeSight(characters[0], nRows);
  for (i = 0; i < num_characters; i++){
    free(characters[i]);
    characters[i] = NULL;
  }
}

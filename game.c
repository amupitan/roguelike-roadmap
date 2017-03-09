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
	Player pc = {-1, -1, '@', 0, 10}; //TODO see if it's better to have in struct or as a variable 2.

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
          else pc.type = 0xF;
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
  Player characters[nummon + 1];
  Pair last_seen[nummon];
  memset(last_seen, - 1, sizeof(Pair)*nummon);

  /*Dungeon monster setup*/
  unsigned int pace[nummon+1];
  characters[0] = pc;
  addCharcters(&dungeon, &evt, nummon, characters, chars, pace);

  add_stairs(&dungeon, map);
  char recalculate = 1;
  Player* pcp = &characters[0];
  do{
    Player* p_curr;
    p_curr = (Player *)peek(&evt, &p_curr);
    Player curr = *p_curr;
    Pair target = {curr.x, curr.y};
    if (curr.value == -1){ //it was killed
      dequeue(&evt);
      continue;
    }
    int new_dungeon = 0;
    /*Determine next position of character*/
    if (p_curr == pcp){
      /* PC stuff */
      // while (target.x < 1 || (target.x > nCols - 1) || target.y < 1 || target.y > nRows - 1 || (target.x == curr.x && target.y == curr.y)){ //Reverse psych lol :) and the PC must move, cannot stay in the same spot, just because it's lame to stay in the same place
      //   target.x = rand_gen(curr.x - 1, curr.x + 1);
      //   target.y = rand_gen(curr.y - 1, curr.y + 1);
      // }
      Pair start = {curr.x - 40, curr.y - 10};
      render_partial(map, chars, characters, start, NULL); //TODO!!!
      do{
        target = *(Pair *)getInputC(&target);
        if (target.x == -1 && target.y == -1) endgame(&dungeon, &evt, "Game ended");
        if (target.x == -2 && target.y == -2) {
          /*Enter look mode*/
          Pair look = start;
          do{
            int ctrl = 0; /*1- end look mode*/
            look = *(look_mode(&look, &ctrl));
            if (ctrl == 1){
              render_partial(map, chars, characters, start, NULL);
              break;
            }
            render_partial(map, chars, characters, look, &look);
          }while(1);
        }
        else if ((target.x == -3 && target.y == -3) || (target.x == -4 && target.y == -4)){
          /*use stairs: if not stair conrinue*/
          if ((target.x == -3 && map[curr.y][curr.x].value != '>') || (target.x == -4 && map[curr.y][curr.x].value != '<')) continue; /*check up/down stairs*/
          pcp->type = 0;
          l_monsters = nummon;
          delete_dungeon(&dungeon, &evt, map);
          create_dungeon(&dungeon, map, room_cells);
          add_stairs(&dungeon, map);
          addCharcters(&dungeon, &evt, nummon, characters, chars, pace);
          recalculate = 1;

          /*START get rid of*/
          getmaxyx(stdscr, longindex, col); /*Longindex is passed here but this macro function requires an argument*/
          move(0, 0);
          clrtoeol();
          mvprintw(0, col/2, "pc x %d y: %d %d", characters[0].x, characters[0].y, seed);
          
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
          sprintf(stat_msg, "PC is at %d, %d. Number of rooms: %d", curr.x, curr.y, dungeon.num_rooms);
          log_message(stat_msg);
          free(stat_msg);
        }else if (target.x == -10 && target.y == -10){
          continue;
        }
        else if (map[target.y][target.x].hardness == 0) break;
        target.x = curr.x;
        target.y = curr.y;
      }while(1);
      if (new_dungeon) continue;
      char mon_log[20];
      sprintf(mon_log, "Monsters alive: %d", l_monsters);
      log_message(mon_log);
    }else{
      
      /*Telephathy*/
      int curr_room_no = getRoom(dungeon, curr.x, curr.y);
      if ((curr.type & 0x2) || (curr_room_no == getRoom(dungeon, pcp->x, pcp->y))){ //smart or line of sight //TODO might want to check corridor
        target.x = pcp->x;
        target.y = pcp->y;
        last_seen[curr.id - 1] = target;
        recalculate = recalculate && (curr.type & 0x1);
      }else{ //change to its own if
        if((curr.type & 0x1) && last_seen[curr.id - 1].x != -1){
          target = last_seen[curr.id - 1];
          recalculate = recalculate && (curr.type & 0x1);
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
        BFS_impl(dist, map, &q, temp_pc);
        
        /* Djikstra for tunelling characters */
        Djikstra_impl(t_dist, map, &q, temp_pc);
        recalculate = 0;
      }
      
      /*Intelligent*/
      if (curr.type & 0x1){
          int curr_dist = INT_MAX;
          for (i = curr.y-1; i <= curr.y+1; i++){
            for (j = curr.x-1; j <= curr.x+1; j++){
              if (!(i== curr.y && j == curr.x) && (i > 0 && j > 0 && i < nRows-1 && j < nCols-1)){ //TODO nRows-1 vs nRows check the whole code
                int es_dist = (curr.type & 0x4) ? t_dist[i][j] : dist[i][j]; //if tunnelling
                if (es_dist < curr_dist){
                  curr_dist = es_dist;
                  target.x = j;
                  target.y = i;
                }
              }
            }
          }
      }else{/*Determine monster's next position*/
          if (curr.x < target.x){
            target.x = (((curr.x + 1) < nCols-1) && ((curr.type & 0x4) || map[curr.y][curr.x + 1].hardness == 0)) ? curr.x + 1 : curr.x; //can move and/or tunnel
          }else if (curr.x > target.x){
            target.x = (((curr.x - 1) > 0) && ((curr.type & 0x4) || map[curr.y][curr.x - 1].hardness == 0)) ? curr.x - 1 : curr.x;
          }
          if (curr.y < target.y){
            target.y = (((curr.y + 1) < nRows-1) && ((curr.type & 0x4) || map[curr.y + 1][curr.x].hardness == 0)) ? curr.y + 1 : curr.y;
          }else if (curr.y > target.y){
            target.y = (((curr.y - 1) > 0) && ((curr.type & 0x4) || map[curr.y - 1][curr.x].hardness == 0)) ? curr.y - 1 : curr.y;
          }
      }
      
      /*Erratic*/
      if ((curr.type & 0x8) && rand_gen(0,1)){
        char found = 0;
        while(!found){
          target.x = rand_gen(curr.x - 1,curr.x + 1);
          target.y = rand_gen(curr.y - 1, curr.y + 1);
          found = ((!(target.y == curr.y && target.x == curr.x) && (target.x > 0 && target.y > 0 && target.y < nRows-1 && target.x < nCols-1)) && (curr.type & 0x4 || map[target.y][target.x].hardness == 0));
        }
      }
      
    }
    
    /*Attempt to move*/
    if (map[target.y][target.x].hardness != 0){
      map[target.y][target.x].hardness = ((map[target.y][target.x].hardness - 85) < 0) ? 0 : map[target.y][target.x].hardness - 85;
      recalculate = 1;
    }
    if (map[target.y][target.x].hardness == 0){
      chars[curr.y][curr.x] = -1;
      /*If PC is killed*/
      if ((curr.id != 0)&&(pcp->x == target.x && pcp->y == target.y)){
        /*Move and make final render*/
        p_curr->x = target.x;
        p_curr->y = target.y;
        chars[target.y][target.x] = curr.id;

        Pair start = {curr.x - 40, curr.y - 10};
        render_partial(map, chars, characters, start, NULL); //TODO, fix start position!!!

        endgame(&dungeon, &evt, "The PC is dead :(");
      }
      if ((chars[target.y][target.x] != -1) && (chars[target.y][target.x] != curr.id)){ //weird stuff.
        characters[chars[target.y][target.x]].value = -1;
        if(!(--l_monsters)) break;
      }
      chars[target.y][target.x] = curr.id;
      if (map[target.y][target.x].value != '.' && map[target.y][target.x].value != '<' && map[target.y][target.x].value != '>') map[target.y][target.x].value = '#';
      p_curr->x = target.x;
      p_curr->y = target.y;
    }
    recalculate = (curr.id == 0) ? 1 : 0;
    pace[curr.id] +=  1000/curr.speed;
    change_priority(&evt, &curr, pace[curr.id]);
  }while(l_monsters || solo);
  
  /*Print win message*/
  if (nummon && !l_monsters) endgame(&dungeon, &evt, "PC killed em all");
  
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
  return (*(Player *)c1).id == (*(Player *)c2).id;
}

void print_player(void* player){
  Player player_c = *(Player *)player;
  printf("id: %d => %c speed is %d\n",player_c.id, player_c.value, player_c.speed);
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
  system("clear");
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

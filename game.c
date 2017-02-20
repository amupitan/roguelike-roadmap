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

#include "queue.h"
#include "game.h"

int main(int argc, char *argv[]){
  
  //regular
  Dungeon dungeon;
  dungeon.rooms = (Room *)malloc(sizeof(Room)*2500); //debug this issue
  dungeon.num_rooms = 0;
  
  //monster
  char chars[nRows][nCols];
  memset(chars, -1, sizeof(char)*nRows*nCols);
  char const *values[8] = {"\x1B[31m", "\x1B[33m", "\x1B[34m", "\x1B[35m", "\x1B[36m", "\x1B[37m", "\x1B[32m", "\x1B[31m"};
  int t_dist[nRows][nCols];
  int dist[nRows][nCols];
  
  int area = 0, tries = 0, count = 0;
	Cell map[nRows][nCols];
	int i = 0, j = 0;
	Cell room_cells[50];
	Player pc = {-1, -1, (char *)PC, 0, 10}; //TODO see if it's better to have in struct or as a variable 2. You just straight up copy characters[0] = pc, does that work well with PC being a macro and string?
// 	dungeon.pc = pc;

  //dungeon
  FILE *dungeon_file;
  char dungeon_title[20];
  uint32_t version;
  uint32_t size_dungeon_file = 0;
  
  //handle options an set seed
  int longindex; //TODO remove this and set the args part to NULL
  int option, load = 0, save = 0, seed = (unsigned) time(NULL), nummon = 0;
  char load_file[100];
  struct option longopts[] = {
    {"load", optional_argument, NULL, 'l'},
    {"save", no_argument, &save, 1},
    {"seed", optional_argument, NULL, 'e'},
    {"pc", optional_argument, NULL, 'p'},
    {"nummon", optional_argument, NULL, 'm'},
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
        if (optarg) nummon = atoi(optarg);
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
  printf("%d\n",seed);
  //Monster stuff
  Player characters[nummon+1];
  Pair last_seen[nummon];
  memset(last_seen, -1, sizeof(Pair)*nummon);
  
  unsigned int pace[nummon+1];
  characters[0] = pc;
  int l_monsters = nummon;
  printf("Num of characters: %d\n", nummon);
  //initialize dungoen with white space and hardness
  for (i = 0; i < nRows; i++){
		for (j =0; j < nCols; j++){
			map[i][j].value = 32;
			map[i][j].hardness = (i == 0 || j == 0 || i == nRows-1 || j == nCols-1) ? 255 : rand_gen(1,254);
		}
	}
  
  if (load){
    if (!(dungeon_file = fopen(load_file, "r"))){
      fprintf(stderr, "The file: %s couldn't be opened\n", load_file);
      return 1;
    }else{
      uint32_t temp = 0;
      //read dungeon title
      char temp_name[13];
      temp_name[12] = 0;
      fread(temp_name, 12, 1, dungeon_file); //cs: 12
      strcpy(dungeon_title, temp_name);
      
      //read verison
      fread(&temp, 4, 1, dungeon_file); //cs: 4
      version = be32toh(temp);
      
      //read size of file
      fread(&temp, 4, 1, dungeon_file); //cs:4
      size_dungeon_file = be32toh(temp);
      
      //read hardness
      for (i = 0; i < nRows; i++)
    		for (j =0; j < nCols; j++)
    		  fread(&(map[i][j].hardness), sizeof(unsigned char), 1, dungeon_file); //cs:8
      
      //quickly write corridors
      for (i = 0; i < nRows; i++)
      		for (j =0; j < nCols; j++)
      			if (map[i][j].hardness == 0) update_cell(&map[i][j], 35, 0);
      
      //read rooms
      // int put = 0;
      Room room = {0,0,0,0};
      while((fread(&room.x, sizeof(uint8_t), 1, dungeon_file)) == 1){
        fread(&room.y, sizeof(uint8_t), 1, dungeon_file);
        fread(&room.width, sizeof(uint8_t), 1, dungeon_file);
        fread(&room.height, sizeof(uint8_t), 1, dungeon_file);
        write_room(map, room);
        // printf("%d\n", ++put);
        // if ((dungeon.num_rooms%20 == 0) && (dungeon.num_rooms != 0)){
        //   int ratio = dungeon.num_rooms/20 + 1;
        //   if (!realloc(dungeon.rooms, ratio*20)) return -1;
        // }
        add_room(&dungeon, room);
      }
      
      //display corridor
      fclose(dungeon_file);

    }
  }else{
    //add rooms to dungeon
    while ((area < .2*(nRows*nCols) || count < 10) && (++tries < 2000)){
      Room room; // = {0, 0, 0, 0};
  		room.y = rand()%(nRows-2) + 1;
  		room.x = rand()%(nCols-2) + 1;
  		
  		if (!create_room(map, room.x, room.y, &room.width, &room.height)) continue; //continue loop if roo couldn't be created
  		area += room.width * room.height;
  		
      //get random cell in each room
  		room_cells[count].x = rand_gen(room.x, room.width + room.x -1);
  		room_cells[count].y = rand_gen(room.y, room.height + room.y -1);
  		count++;
  		
  		//add room to rooms array TODO merge this with code above
  		add_room(&dungeon, room);
	  }
    //connect random cells
  	for (i =0; i < count-1; i++){
  		connect_rooms(map, room_cells[i], room_cells[i+1]);
  	}
  }
  
  /*Monster magic and initialize random pc if no valid command line argument was entered*/
  Queue evt;
  queue_init(&evt, char_equals, print_player);
  

  
  for(i = 0; i < nummon+1; i++){
    int rand_room = i ? rand_gen(1, dungeon.num_rooms - 1) : 0;
    if ( (i != 0) || (i == 0 && characters[i].type == 0)){
      characters[i].x = rand_gen(dungeon.rooms[rand_room].x, dungeon.rooms[rand_room].x + dungeon.rooms[rand_room].width - 1); //use determine_position
      characters[i].y = rand_gen(dungeon.rooms[rand_room].y, dungeon.rooms[rand_room].y + dungeon.rooms[rand_room].height - 1);
      if (i != 0 ) {
        characters[i].type = rand() & 0xF;//rand_gen(0x0,0xF);
        characters[i].value = (char *)malloc(sizeof(RESET)*2 + sizeof(char));
        sprintf(characters[i].value, "%s%x%s", values[characters[i].type/2], characters[i].type, RESET);
      }
    }
    characters[i].speed = rand_gen(5, 20);
    characters[i].id = i;
    chars[characters[i].y][characters[i].x] = i;
    pace[i] = 1000/characters[i].speed;//TODO: warning: does not enqueue PC
    add_with_priority(&evt, &characters[i], pace[i]);
    // printf("monster %d: x = %s\n", i, characters[i].value);
  }
  characters[0].speed = 10; //Make sure this actually happens
  puts("Initial queue");
  print_queue(&evt);
  
  Player* pcp = &characters[0]; //TODO make sure this works
  int papi = 400;
  do{
    Pair target = {0,0}; //initilised to 0 because of PC
    Player* p_curr;
    p_curr = (Player *)peek(&evt, &p_curr);
    Player curr = *p_curr;
    if (curr.value == NULL){ //it was killed
      printf("Dead cell: \n");
      print_player(&curr);
      dequeue(&evt);
      continue;
    }

    //check each characteristic and make sure it's not pc
    if (strcmp(curr.value, characters[0].value) == 0){ //might be able to use ==
      /* PC stuff */
      while (target.x < 1 || (target.x > nCols - 1) || target.y < 1 || target.y > nRows - 1){ //Reverse psych lol :)
        target.x = rand_gen(curr.x - 1, curr.x + 1);
        target.y = rand_gen(curr.y - 1, curr.y + 1);
      }
      // printf("PC is the fastest\n");
      // pace[curr.id] = ((pace[curr.id]*2) >= 1000) ? 1000/curr.speed : pace[curr.id]*2;
      // continue;
    }else{
      
      /*Telephathy*/ //TODO check next 6 lines for lame code if there is
      int curr_room_no = getRoom(dungeon, curr.x, curr.y); //TODO split line
      if ((curr.type & 0x2) || (curr_room_no == getRoom(dungeon, pcp->x, pcp->y))){ //smart or line of sight //TODO might want to check corridor
        target.x = pcp->x;
        target.y = pcp->y;
        last_seen[curr.id - 1] = target;
        // printf("New last seen x: %d, y: %d\n", target.x, target.y);
      }else{ //change to its own if
        // target = (last_seen[i].x == -1 && (curr.type & 0x1)) ? determine_position(dungeon.rooms[curr_room_no]) : last_seen[curr.id]; //add a condition that before you can use last seen, you have to intelligent
        if((curr.type & 0x1) && last_seen[curr.id - 1].x != -1){
          target = last_seen[curr.id - 1];
          // printf("Last seen x: %d, y: %d\n", target.x, target.y);
        }else{
          // printf("Current room number: %d\n", curr_room_no);
          curr_room_no = (curr_room_no == -1) ? 0 : curr_room_no; //if the player is not in a room TODO: generate random room instead of sebding it to room 0
          target = determine_position(dungeon.rooms[curr_room_no]);
          // printf("Determined x: %d y: %d\n", target.x, target.y);
        }
        // if(last_seen[i].x == -1 && !(curr.type & 0x1)){
        //   target = determine_position(dungeon.rooms[curr_room_no]);
        //   printf("Determined x: %d y: %d\n", target.x, target.y);
        // }else{
        //   target = last_seen[curr.id];
        //   printf("LAst seen x: %d, y: %d\n", target.x, target.y);
        // }
      }
      
      /*Calculate distance to PC*/
      
      /*Create and initialize queue for calculating monster distances*/
      Queue q;
      queue_init(&q, cell_equals, NULL);
      
      /*Breadth First Search for non-tunelling characters*/
      Cell temp_pc = {target.x, target.y, 0, 0};
      BFS_impl(dist, map, &q, temp_pc);
      
      /* Djikstra for tunelling characters */
      Djikstra_impl(t_dist, map, &q, temp_pc);
      
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
          // for (i = curr.y-1; i <= curr.y+1; i++){
          //     for (j = curr.x-1; j <= curr.x+1; j++){
          //       if (!(i== y && j == x) && (i > 0 && j > 0 && i < nRows-1 && j < nCols-1)){
                  
          //       }
          //     }
          // }
        }
      }
      
    }
    
    //move
    if (map[target.y][target.x].hardness != 0){
      map[target.y][target.x].hardness = ((map[target.y][target.x].hardness - 85) < 0) ? 0 : map[target.y][target.x].hardness - 85;
    }
    if (map[target.y][target.x].hardness == 0){
      chars[curr.y][curr.x] = -1;
      if ((curr.id != 0)&&(pcp->x == target.x && pcp->y == target.y)){
        puts("The PC is dead :(");
        break;
      }
      if ((chars[target.y][target.x] != -1) && (chars[target.y][target.x] != curr.id)){ //weird stuff.
        printf("Chars: %d", chars[target.y][target.x]);
        characters[(int)chars[target.y][target.x]].value = NULL;
        if(!(--l_monsters)) break;
      }
      chars[target.y][target.x] = (char)curr.id; /*TODO WARNING Max monsters have to be 255*/
      if (map[target.y][target.x].value != '.') map[target.y][target.x].value = '#';
      //MOVE Monster
      if (curr.x != target.x || curr.y != target.y){
        
        render_dungeon(map, chars, characters);
        fflush(stdout);
        usleep(80000);
        p_curr->x = target.x;
        p_curr->y = target.y;
      }
      //is this necessary?
      p_curr->x = target.x;
      p_curr->y = target.y;
    }
    
    //dequeue
    // pace[curr.id] = 2 * pace[curr.id];
    pace[curr.id] = ((pace[curr.id]*2) >= 1000) ? 1000/curr.speed : pace[curr.id]*2;
    // puts("before:");
    // print_queue(&evt);
    change_priority(&evt, &curr, pace[curr.id]);
    // puts("Added: ");
    // add_with_priority(&evt, &curr, pace[curr.id]);
  }while(1);
  if (papi == 0 || papi == -1) puts("I killed it myself");
  
  empty_queue(&evt);
  
  //render non-tunneling monster gradients  TODO: remove
  // for (i = 0; i < nRows; i++){
		// for (j =0; j < nCols; j++){
		//   if (i == pc.y && j == pc.x) putchar('@');
		//   else if (map[i][j].value == '#' || map[i][j].value == '.') printf("%d", dist[i][j]%10);
		//   else putchar(' ');
		// }
		// putchar('\n');
  // }
  
  //render tunneling monster gradients TODO: remove
  // for (i = 0; i < nRows; i++){
		// for (j =0; j < nCols; j++){
		//   if (i == pc.y && j == pc.x) putchar('@');
		//   else if (map[i][j].hardness == 255) putchar(' ');
		//   else printf("%d", t_dist[i][j]%10);
		// }
		// putchar('\n');
  // }

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
  
  /*Free all mallocs before exiting*/
  free(dungeon.rooms);
  for(i = 1; i < nummon+1; i++){
    free((char *)characters[i].value);
  }
  
	return 0;
}

int create_room(Cell map[][nCols], int x, int y, uint8_t* width, uint8_t* height){//TODO change args up by just passing in a room pointer
	int i = 0, j= 0;
	//generate random length and width of room by genrating a random co-ord
	int max = nCols-2;
	int min = x + 6;
	int end_x = rand_gen(min,max);
	max = nRows - 2;
	min = y + 4;
	int end_y = rand_gen(min,max);
	if (end_x - x > 25 || end_y - y > 15 ) return 0;//should be 0
	if (end_x == -1 || end_y == -1) return 0; //should be 0

	//check if this part of dungeon already contains a room
	for (i = y-1; i <= end_y + 2; i += end_y - y + 2){ //TODO check logic for this block and the next one
		for (j = x; j <= end_x; j++){
			if (map[i][j].value == 46) return 0;//should be 0
		}
	}
	for (i = x-1; i <= end_x + 2; i += end_x - x + 2){
		for (j = y; j <= end_y; j++){
			if (map[j][i].value == 46) return 0;//should be 0
		}
	}

	//fill room with dots
	for (j = y; j <= end_y; j++){
		for (i = x; i <= end_x; i++){
		  update_cell(&map[j][i], 46, 0);
		}
	}

	//return width and length of room respectively
	*width = end_x - x + 1;
  *height = end_y - y + 1;
	return 1;
}

int rand_gen(int min, int max){
	return (max >= min) ? (rand() % (max-min+1)) + min : -1;
}

int connect_rooms(Cell map[][nCols], Cell p, Cell q){
	while (p.x != q.x){
		if (map[p.y][p.x].value != 46){
		  update_cell(&map[p.y][p.x], 35, 0);
		}
		p.x = (p.x > q.x) ? p.x-1 : p.x+1;
	}
	while (p.y != q.y){
		if (map[p.y][p.x].value != 46){
		  update_cell(&map[p.y][p.x], 35, 0);
		}
		p.y = (p.y > q.y) ? p.y-1 : p.y+1;
	}
	return 1;
}

int update_cell(Cell* p, char value, unsigned char hardness){//TODO make function take hardness as arg
  if (p->hardness == 255) return 1;
  p->value = value;
  p->hardness = hardness;
  return 0;
}

void render_dungeon(Cell map[][nCols], char chars[][nCols], Player monsts[]){
  int i = 0, j = 0;
  for (i = 0; i < nRows; i++){
		for (j =0; j < nCols; j++){
		  if (i == 0 || j == 0 || i == (nRows - 1) || j == (nCols - 1)) putchar('X');//TODO remove
		  else{
  		  int temp = (int)chars[i][j];
  		  if (temp != -1) printf(monsts[temp].value);
  		  else putchar(map[i][j].value);
		  }
		}
		putchar('\n');
	}
}

void render(Cell map[][nCols], int x, int y){
  int i = 0, j = 0;
  for (i = 0; i < nRows; i++){
		for (j =0; j < nCols; j++){
		  if (i == y && j == x) printf(PC);
		  else putchar(map[i][j].value);
		}
		putchar('\n');
	}
}

int write_room(Cell map[][nCols], Room room){
  int i=0,j=0;
  int n = room.x + room.width - 1;
  int m = room.y + room.height - 1;
  for (i = room.y; i <= m; i++){
    for (j = room.x; j <= n; j++){
      update_cell(&map[i][j], 46, 0);
    }
  }
  return 0;
}

int add_room(Dungeon* rlg, Room room){ //TODO debug with 20
  // if ((rlg->num_rooms%30 == 0) && (rlg->num_rooms != 0)){
  //   int ratio = rlg->num_rooms/30 + 1;
  //   if (!realloc(rlg->rooms, ratio*30)) return -1;
  // }
  rlg->rooms[rlg->num_rooms].x = room.x;
  rlg->rooms[rlg->num_rooms].y = room.y;
  rlg->rooms[rlg->num_rooms].width = room.width;
  rlg->rooms[rlg->num_rooms].height = room.height;
  rlg->num_rooms++;
  return 0;
}

int cell_equals(void* c1, void* c2){
  Cell oldCell = *(Cell *)c1;
  Cell newCell = *(Cell *)c2;
  return (oldCell.x == newCell.x && oldCell.y == newCell.y);
}

int char_equals(void* c1, void* c2){
  Player oldCell = *(Player *)c1;
  Player newCell = *(Player *)c2;
  return (oldCell.id == newCell.id);
}

void BFS_impl(int dist[][nCols], Cell map[][nCols], Queue* q, Cell pc){
    int i = 0,j = 0;
    uint8_t marked[nRows][nCols];
    // printf("Pc is x: %d, y:%d\n", pc.x,pc.y);
    //initialize distance of all cells to infinity & initialized all cells as unmarked
    for (i = 0; i < nRows; i++){
  		for (j =0; j < nCols; j++){
  		  dist[i][j] = INT_MAX;
  			marked[i][j] = 0;
  		}
  	}
  	
    /*Create mask cells to be used with the queue*/
    Cell* temps[nRows][nCols];
    for (i = 0; i < nRows; i++){
      for (j = 0; j< nCols; j++){
        temps[i][j] = NULL;
      }
    }
  
  	/*BFS implementation with linear queue*/
    enqueue(q, &pc); //NOTE: In case of any warnings, pc is not type cell but is expected to evaluate type cell
    marked[pc.y][pc.x] = 1;
    dist[pc.y][pc.x] = 0;
    while (q->size > 0){
      Cell curr = *(Cell *)peek(q, &curr);
      int x = curr.x, y = curr.y;
      dequeue(q);
  
      for (i=y-1; i <= y+1; i++){
        for (j=x-1; j <= x+1; j++){
          if (!(i== y && j == x) && (i > 0 && j > 0 && i < nRows-1 && j < nCols-1) && (map[i][j].hardness == 0)){
            if (marked[i][j] && (dist[y][x] + 1 < dist[i][j])) dist[i][j] = dist[y][x] + 1;
            else if (marked[i][j] == 0){
              temps[i][j] = (Cell *)malloc(sizeof(Cell));
              // printf("Malloc'd temp[%d][%d]\n", i, j);
              Cell temp_cell = {j, i, map[i][j].value, map[i][j].hardness};
              *temps[i][j] = temp_cell;
              enqueue(q, temps[i][j]);
              dist[i][j] = dist[y][x] + 1;
              marked[i][j] = 1;
            }
            
          }
        }
      }
    }
    
    /* Freeing mask cells that were used*/
    for (i = 0; i < nRows; i++){
      for (j = 0; j< nCols; j++){
        if (temps[i][j]){
          // printf("Free'd temp[%d][%d]\n", i, j);
          free(temps[i][j]);
          temps[i][j] = NULL;
        }
      }
    }
  	empty_queue(q);//TODO do this here or outside the function?
  }
  
void Djikstra_impl(int t_dist[][nCols], Cell map[][nCols], Queue* q, Cell pc){
  /*Make sure queue is empty TODO: Or just use a new queue in here*/
  empty_queue(q);
  int i = 0, j = 0;
  uint8_t weight[nRows][nCols];
  uint8_t marked[nRows][nCols];
  
  /*Mask cells to be used with the queue*/
  Cell* temps[nRows][nCols];
  for (i = 0; i < nRows; i++)
    for (j = 0; j< nCols; j++)
      temps[i][j] = NULL;
  
  /*Initialize distance of every cell to infinte distances and unmarked*/
  for (i = 0; i < nRows; i++){
		for (j =0; j < nCols; j++){
		  if (map[i][j].hardness == 0) weight[i][j] = 1;
		  else if (map[i][j].hardness >= 1 && map[i][j].hardness <= 84) weight[i][j] = 1;
		  else if (map[i][j].hardness >= 85 && map[i][j].hardness <= 170) weight[i][j] = 2;
		  else if (map[i][j].hardness >= 171 && map[i][j].hardness <= 254) weight[i][j] = 3;
		  else weight[i][j] = UINT8_MAX;
			marked[i][j] = 0;
			t_dist[i][j] = INT_MAX;
		}
	}

	enqueue(q, &pc);//NOTE: In case of any warnings, pc is not type cell but is expected to evaluate type cell
	t_dist[pc.y][pc.x] = 0;
	
	/*shortest path finding using Djistra's algorithm*/
  while (q->size > 0){
    Cell* curr = (Cell *)peek(q, &curr);
    int x = curr->x, y = curr->y;
    dequeue(q);
    marked[y][x] = 1;
    for (i = y-1; i <= y+1; i++){
      for (j = x-1; j <= x+1; j++){
        if (!(i== y && j == x) && (i > 0 && j > 0 && i < nRows && j < nCols)){
          int alt_dist = t_dist[y][x] + weight[y][x];
          if (marked[i][j] == 0 && (alt_dist < t_dist[i][j])){
            t_dist[i][j] = alt_dist;
            if (temps[i][j]==NULL)
              temps[i][j] = (Cell *)malloc(sizeof(Cell));
            Cell temp_cell = {j, i, map[i][j].value, map[i][j].hardness};
            *temps[i][j] = temp_cell;
            add_with_priority(q, temps[i][j], alt_dist);
          }
        }
      }
    }
  }

  for (i = 0; i < nRows; i++)
      for (j = 0; j< nCols; j++)
          free(temps[i][j]);
}

void print_player(void* player){ //REname tp print player
  Player player_c = *(Player *)player;
  printf("id: %d => %s speed is %d\n",player_c.id, player_c.value, player_c.speed);
}

int getRoom(Dungeon d, int x, int y){
  int i = 0;
  for (i = 0; i < d.num_rooms; i++){
    Room room = d.rooms[i];
    if (x >= room.x && x < (room.x + room.width) && y >= room.y && y < (room.y + room.height))
      return i;
  }
  return -1;
}

Pair determine_position(Room room){
  Pair result = {
    rand_gen(room.x, room.x + room.width - 1),
    rand_gen(room.y, room.y + room.height - 1)
  };
  return result;
}


//test crap
// puts("Initial queue");
  // print_queue(&evt);
  // do{
  //   Player curr = *(Player *)peek(&evt, &curr);
  //   printf("Curr: ");
  //   print_player(&curr);
  // }while(0);
  // print_queue(&evt);
  // dequeue(&evt);
  // puts("Dequeueing");
  // print_queue(&evt);
  // puts("Emptying");
  // empty_queue(&evt);
  // print_queue(&evt);
  
  //weird 4: --nummon=1 --seed=1487510267
  //weird PC, a couple of 7s and a b: --nummon=3 -e 1487512423
  //game --nummon=20 1487542614
  //--nummon=1 1487549385 gets stuck but shouldn't
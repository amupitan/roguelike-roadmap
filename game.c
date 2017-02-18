#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <sys/stat.h>
#include <stdint.h>
#include <endian.h>
#include <limits.h>

#include "queue.h"
#include "game.h"

int main(int argc, char *argv[]){
  
  //regular
  Dungeon dungeon;
  dungeon.rooms = (Room *)malloc(sizeof(Room)*2500); //debug this issue
  dungeon.num_rooms = 0;
  
  //monster
  // char chars[nRows][nCols];
  char const *values[8] = {"\x1B[31m", "\x1B[33m", "\x1B[34m", "\x1B[35m", "\x1B[36m", "\x1B[37m", "\x1B[32m", "\x1B[31m"};
  
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
  
  //Monster stuff
  Player characters[nummon+1];
  // memcpy(&characters[0], &pc, sizeof(pc));
  // characters[0].x = pc.x;characters[0].y = pc.y;;characters[0].type = pc.type;
  characters[0] = pc;
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
  
  //initialize random pc if no valid command line argument was entered
  // if (pc.type == 0){
  //   int rand_room = rand_gen(0, dungeon.num_rooms - 1);
  //   characters[0].x = rand_gen(dungeon.rooms[rand_room].x, dungeon.rooms[rand_room].x + dungeon.rooms[rand_room].width - 1);
  //   pc.y = rand_gen(dungeon.rooms[rand_room].y, dungeon.rooms[rand_room].y + dungeon.rooms[rand_room].height - 1);
  // }
  
  // printf("PC:\nPC.x: %d\nPC.y %d\nPC.value %s\nPC.type: %x\n", pc.x, pc.y, pc.value, pc.type);
  // printf("PC:\nPC.x: %d\nPC.y %d\nPC.value %s\nPC.type: %x\n", characters[0].x, characters[0].y, characters[0].value, characters[0].type);
  /*Monster magic and initialize random pc if no valid command line argument was entered*/
  for(i = 0; i < nummon+1; i++){
    int rand_room = i ? rand_gen(1, dungeon.num_rooms - 1) : 0;
    if ( (i != 0) || (i == 0 && characters[i].type == 0)){
      characters[i].x = rand_gen(dungeon.rooms[rand_room].x, dungeon.rooms[rand_room].x + dungeon.rooms[rand_room].width - 1);
      characters[i].y = rand_gen(dungeon.rooms[rand_room].y, dungeon.rooms[rand_room].y + dungeon.rooms[rand_room].height - 1);
      if (i != 0 ) {
        characters[i].type = rand() & 0xF;//rand_gen(0x0,0xF);
        characters[i].value = (char *)malloc(sizeof(RESET)*2 + sizeof(char));
        sprintf(characters[i].value, "%s%x%s", values[characters[i].type/2], characters[i].type, RESET);
      }
    }
    // printf("monster %d: x = %s\n", i, characters[i].value);
  }
  
  /*Create and initialize queue for calculating monster distances*/
  Queue q;
  queue_init(&q, cell_equals, NULL);
  
  /*Breadth First Search for non-tunelling characters*/
  int dist[nRows][nCols];
  BFS_impl(dist, map, &q, characters[0]);
  
  /* Djikstra for tunelling characters */
  int t_dist[nRows][nCols];
  Djikstra_impl(t_dist, map, &q, characters[0]);
  
  /*render regular dungeon*/
  // render(map, pc.x, pc.y);
  
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

// void render_den(Cell map[][nCols], int x, int y){
//   int i = 0, j = 0;
//   for (i = 0; i < nRows; i++){
// 		for (j =0; j < nCols; j++){
// 		  if (i == y && j == x) printf(PC);
// 		  else putchar(map[i][j].value);
// 		}
// 		putchar('\n');
// 	}
// }

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

void BFS_impl(int dist[][nCols], Cell map[][nCols], Queue* q, Player pc){
    int i = 0,j = 0;
    uint8_t marked[nRows][nCols];
    
    //initialize distance of all cells to infinity & initialized all cells as unmarked
    for (i = 0; i < nRows; i++){
  		for (j =0; j < nCols; j++){
  		  dist[i][j] = INT_MAX;
  			marked[i][j] = 0;
  		}
  	}
  	
    //Mask cells to be used with the queue
    Cell* temps[nRows][nCols];
    for (i = 0; i < nRows; i++)
      for (j = 0; j< nCols; j++)
        temps[i][j] = NULL;
  
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
          if (!(i== y && j == x) && (i > 0 && j > 0 && i < nRows && j < nCols) && (map[i][j].hardness == 0)){
            if (marked[i][j] && (dist[y][x] + 1 < dist[i][j])) dist[i][j] = dist[y][x] + 1;
            else if (marked[i][j] == 0){
              temps[i][j] = (Cell *)malloc(sizeof(Cell));
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
    
    for (i = 0; i < nRows; i++){
      for (j = 0; j< nCols; j++){
        if (temps[i][j]){
          free(temps[i][j]);
          temps[i][j] = NULL;
        }
      }
    }
  	empty_queue(q);//TODO do this here or outside the function?
  }
  
void Djikstra_impl(int t_dist[][nCols], Cell map[][nCols], Queue* q, Player pc){
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
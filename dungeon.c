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

// void render_dungeon(Cell map[][nCols], int chars[][nCols], Player monsts[]){
//   int i = 0, j = 0;
//   for (i = 0; i < nRows; i++){
// 		for (j =0; j < nCols; j++){
// 		  if (i == 0 || j == 0 || i == (nRows - 1) || j == (nCols - 1)) putchar(' ');
// 		  else{
//   		  int temp = chars[i][j];
//   		  if (temp != -1) printf("%s",monsts[temp].value);
//   		  else putchar(map[i][j].value);
// 		  }
// 		}
// 		putchar('\n');
// 	}
// }

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
  if ((rlg->num_rooms%50 == 0) && (rlg->num_rooms != 0)){
    int ratio = rlg->num_rooms*2;
    if (!(rlg->rooms = (Room *)realloc(rlg->rooms, ratio*sizeof(Room)))) return -1;
  }
  rlg->rooms[rlg->num_rooms].x = room.x;
  rlg->rooms[rlg->num_rooms].y = room.y;
  rlg->rooms[rlg->num_rooms].width = room.width;
  rlg->rooms[rlg->num_rooms].height = room.height;
  rlg->num_rooms++;
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

void BFS_impl(int dist[][nCols], Cell map[][nCols], Queue* q, Cell pc){
    int i = 0,j = 0;
    uint8_t marked[nRows][nCols];
    /* Initialize distance of all cells to infinity & initialize all cells as unmarked*/
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
    enqueue(q, &pc);
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
          free(temps[i][j]);
          temps[i][j] = NULL;
        }
      }
    }
  	empty_queue(q);
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

  /*Free all memory malloc'd for masked cells*/
  for (i = 0; i < nRows; i++)
      for (j = 0; j< nCols; j++)
          free(temps[i][j]);
}

void nrender_dungeon(Cell map[][nCols], int chars[][nCols], Player monsts[]){
  int i = 0, j = 0;
  for (i = 0; i < nRows; i++){
		for (j =0; j < nCols; j++){
		  if (i == 0 || j == 0 || i == (nRows - 1) || j == (nCols - 1)) addch(' ');
		  else{
  		  int temp = chars[i][j];
  		  if (temp != -1) printmon(monsts[chars[i][j]]);
  		  else addch(map[i][j].value);
		  }
		}
		addch('\n');
	}
	refresh();
	move(0,0);
}

void printmon(Player player){
  attron(COLOR_PAIR((player.id % 6) + 1));
  // printw("%c",player.value);
  addch(player.value);
  attroff(COLOR_PAIR((player.id % 6) + 1));
}
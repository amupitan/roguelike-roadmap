#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define nRows 105
#define nCols 160

typedef struct Point{
	int x;
	int y;
	int immutable;
}Point;


int* create_room(char map[][nCols], int x, int y, int *room_ends);
int rand_gen(int min, int max);
int connect_rooms(char map[][nCols], Point p, Point q);

int main(int argc, char *argv[]){
	int seed, area = 0, tries = 0, count = 0;
	char map[nRows][nCols];
	int i = 0, j = 0;
	Point room_points[35];
	if (argc > 1)
		seed = atoi(argv[1]);
	else seed = (unsigned) time(NULL);
	srand(seed);
	//initialise all room points to whitespace
	for (i = 0; i < nRows; i++){
		for (j =0; j < nCols; j++){
			map[i][j] = 32;
		}
	}

	//add rooms to dungeon
	while ((area < .2*(nRows*nCols) || tries < 500 || count < 10) && (count < 20)){
		int y = rand()%(nRows-2) + 1;
		int x = rand()%(nCols-2) + 1;
		int room_coords[] = {0,0};
		create_room(map, x, y, room_coords);
		if (room_coords[0] == 0 && room_coords[1] == 0) continue;
		area += room_coords[0] * room_coords[1];
		
    //get random point in each room
		room_points[count].x = rand_gen(x, room_coords[0] + x -1);
		room_points[count].y = rand_gen(y, room_coords[1] + y -1);
		count++;
		tries++;
	}

  //connect random points
	for (i =0; i < count-1; i++){
		connect_rooms(map, room_points[i], room_points[i+1]);
	}
	//render world
	for (i = 0; i < nRows; i++){
		for (j =0; j < nCols; j++){
			printf("%c", map[i][j]);
		}
		printf("\n");
	}
	return 0;
}

int* create_room(char map[][nCols], int x, int y, int *room_ends){
	int i = 0, j= 0;
	//generate random length and width of room by genrating a random co-ord
	int max = nCols-2;
	int min = x + 6;
	int end_x = rand_gen(min,max);
	max = nRows - 2;
	min = y + 4;
	int end_y = rand_gen(min,max);
	if (end_x - x > 35 || end_y - y > 25 ) return room_ends;//should be 0
	if (end_x == -1 || end_y == -1) return room_ends; //should be 0

	//check if this part of dungeon already contains a room
	for (i = y-1; i <= end_y + 2; i += end_y - y + 2){ //TODO check logic for this block and the next one
		for (j = x; j <= end_x; j++){
			if (map[i][j] == 46) return room_ends;//should be 0
		}
	}
	for (i = x-1; i <= end_x + 2; i += end_x - x + 2){
		for (j = y; j <= end_y; j++){
			if (map[j][i] == 46) return room_ends;//should be 0
		}
	}

	//fill room with dots
	for (j = y; j <= end_y; j++){
		for (i = x; i <= end_x; i++){
			map[j][i] = 46;
		}
	}

	//return width and length of room respectively
	room_ends[0] = end_x - x + 1;
       	room_ends[1] = end_y - y + 1;
	return room_ends;
}

int rand_gen(int min, int max){
	return (max >= min) ? (rand() % (max-min+1)) + min : -1;
}

int connect_rooms(char map[][nCols], Point p, Point q){
	while (p.x != q.x){
		if (map[p.y][p.x] != 46){
			map[p.y][p.x] = 35;
		}
		p.x = (p.x > q.x) ? p.x-1 : p.x+1;
	}
	while (p.y != q.y){
		if (map[p.y][p.x] != 46){
			map[p.y][p.x] = 35;
		}
		p.y = (p.y > q.y) ? p.y-1 : p.y+1;
	}
	return 1;
}

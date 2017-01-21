#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define nRows 105
#define nCols 160

int* create_room(char map[][nCols], int x, int y, int *room_ends);
int rand_gen(int min, int max);
int draw_path(char map[][nCols], int x1, int y1, int x2, int y2);

int main(int argc, char *argv[]){
	int seed, area = 0, tries = 0, count = 0;
	char map[nRows][nCols];
	//static int num_rocks = 0;
	int i = 0, j = 0;
//	int[] rooms;
	if (argc > 1)
		seed = atoi(argv[1]);
	else seed = (unsigned) time(NULL);

	srand(seed);
	for (i = 0; i < nRows; i++){
		for (j =0; j < nCols; j++){
			map[i][j] = 32;
			//printf("%c", map[i][j]);
		}
//		printf("\n");
	}
//	int y = rand()%(nRows-2) + 1;
//	int x = rand()%(nCols-2) + 1;
//	int max = nCols-2;
//	int min = x + 6;
//	int width = rand_gen(min,max);//(rand() % (max-min+1)) + min;
//	max = nRows - 2;
//	min = y + 4;
//	int height = rand_gen(min,max);//(rand() % (max-min+1)) + min;
//	for (j = y; j <= height; j++){
//		for (i = x; i <= width; i++){
//			map[j][i] = 46;
//		}
//	}
//	int p = create_room(map, 0,0);
//	int p = rand_gen(3,10);
	while (area < .2*(nRows*nCols) || tries < 500 || count < 10){
		int y = rand()%(nRows-2) + 1;
		int x = rand()%(nCols-2) + 1;
		int room_coords[] = {0,0};
	       	create_room(map, x, y, room_coords); 
		area += room_coords[0] * room_coords[1]; 
		count +=  (area > 0) ? 1 : 0;
		tries++;
	}
	draw_path(map, 0, 0, 45,22);
	for (i = 0; i < nRows; i++){
		for (j =0; j < nCols; j++){
			printf("%c", map[i][j]);
		}
		printf("\n");
	}
	//printf("x:%d y:%d height:%d width:%d\n", x, y, height, width);



	return 0;	
}

int* create_room(char map[][nCols], int x, int y, int *room_ends){
	int i = 0, j= 0;
	//int room_ends[2];
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

	//return area of room
	room_ends[0] = end_x - x + 1;
       	room_ends[1] = end_y - y + 1;
	//return (end_x - x + 1) * (end_y - y + 1);
	return room_ends;
}	

int rand_gen(int min, int max){
	return (max >= min) ? (rand() % (max-min+1)) + min : -1;
}

int draw_path(char map[][nCols], int x1, int y1, int x2, int y2){
	while (x1 != x2 || y1 != y2){
		if (map[y1][x1] != 46){
			map[y1][x1] = '#';
		}
		if (x1 < x2){
			x1++;
		}
		if (y1 < y2){
			y1++;
		}
	}
	return 1;
}

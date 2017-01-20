#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define nRows 105
#define nCols 160

int create_room(char map[][nCols], int x, int y);
int rand_gen(int min, int max);

int main(int argc, char *argv[]){
	int seed, count = 0, tries = 0;
	char map[nRows][nCols];
	static int num_rocks = 0;
	int i = 0, j = 0;

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
	while (count < 10 || tries < 50){
		int y = rand()%(nRows-2) + 1;
		int x = rand()%(nCols-2) + 1;
		count += create_room(map, x, y);
		tries++;
	}
	for (i = 0; i < nRows; i++){
		for (j =0; j < nCols; j++){
			printf("%c", map[i][j]);
		}
		printf("\n");
	}
	//printf("x:%d y:%d height:%d width:%d\n", x, y, height, width);



	return 0;	
}

int create_room(char map[][nCols], int x, int y){
	int i = 0, j= 0;
	int max = nCols-2;
	int min = x + 6;
	int end_x = rand_gen(min,max);
	max = nRows - 2;
	min = y + 4;
	int end_y = rand_gen(min,max);
	if (end_x == -1 || end_y == -1) return 0; 
	for (i = y-1; i <= end_y + 2; i += end_y - y + 2){ //TODO check logic for this block and the next one
		for (j = x; j <= end_x; j++){
			if (map[i][j] == 46) return 0;
		}
	}
	for (i = x-1; i <= end_x + 2; i += end_x - x + 2){
		for (j = y; j <= end_y; j++){
			if (map[j][i] == 46) return 0;
		}
	}
	for (j = y; j <= end_y; j++){
		for (i = x; i <= end_x; i++){
			map[j][i] = 46;
		}
	}
	return 1;
}	

int rand_gen(int min, int max){
	return (max >= min) ? (rand() % (max-min+1)) + min : -1;
}

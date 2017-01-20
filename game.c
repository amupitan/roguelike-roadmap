#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define nRows 105
#define nCols 160

int main(int argc, char *argv[]){
	int seed;
	static char map[nRows][nCols];
	int i = 0, j = 0;
	if (argc > 1)
		seed = atoi(argv[1]);
	else seed = (unsigned) time(&t);

	srand(seed);

	for (i = 0; i < nRows; i++){
		for (j =0; j < nCols; j++){
			map[i][j] = 46;
			printf("%c", map[i][j]);
		}
		printf("\n");
	}

	return 0;	
}


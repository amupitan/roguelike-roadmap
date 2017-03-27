#ifndef CELL_H
#define CELL_H

# ifdef __cplusplus
extern "C" {
# endif

typedef struct Pair{
  int x;
  int y;
} Pair;

typedef struct Cell{
	int x;
	int y;
	char value;
	unsigned char hardness;
}Cell;

# ifdef __cplusplus
}
# endif

#endif

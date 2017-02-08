#ifndef QUEUE_H
#define QUEUE_H

# ifdef __cplusplus
extern "C" {
# endif

#include "game.h"

struct Node {
	Cell data;
	struct Node* next;
	int priority;
};

typedef struct Queue {
  struct Node* front;
  struct Node* rear;
  int size;
} Queue;

void queue_init(Queue* q);
void print_queue(Queue* q);
void peek(Queue* q, Cell* c);
int dequeue(Queue* q);
void enqueue(Queue* q, Cell x);
void empty_queue(Queue* q);
void add_with_priority(Queue* q, Cell c, int priority);
void add_with_priority_duplicates(Queue* q, Cell c, int priority);

# ifdef __cplusplus
}
# endif

#endif
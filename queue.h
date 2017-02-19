#ifndef QUEUE_H
#define QUEUE_H

# ifdef __cplusplus
extern "C" {
# endif

typedef struct Node {
	void* data;
	struct Node* next;
	int priority;
	
	
} Node;

typedef struct Queue {
  struct Node* front;
  struct Node* rear;
  int size;
  int (*equals)(void* data1, void* data2); //TODO have it use const (const void* data1, const void* data2);
  void (*printer)(void* node);
  void (*copier)(void* dest, void* src);
} Queue;

void queue_init(Queue* q, int (*node_equals)(void* data1, void* data2), void(*node_printer)(void* node)); //TODO const
void print_queue(Queue* q);
void* peek(Queue* q, void* data);
void dequeue(Queue* q);
void enqueue(Queue* q, void* data);
void empty_queue(Queue* q);
void add_with_priority(Queue* q, void* data, int priority);
void add_with_priority_duplicates(Queue* q, void* data, int priority);
void change_priority(Queue* q, void* data, int new_priority);

# ifdef __cplusplus
}
# endif

#endif
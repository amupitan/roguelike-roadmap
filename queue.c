/*Priority Queue - Linked List implementation*/
#include<stdio.h>
#include<stdlib.h>

#include "queue.h"

void queue_init(Queue* q){
  q->front = NULL;
  q->rear = NULL;
  q->size = 0;
}

/* Add to queue without priority (linear queue)*/
void enqueue(Queue* q, Cell x) {
	struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
	temp->data = x;
	temp->next = NULL;
	temp->priority = 0;
	q->size++;
	if(q->front == NULL && q->rear == NULL){
		q->front = q->rear = temp;
		return;
	}
	q->rear->next = temp;
	q->rear = temp;
	
}

/* Add to queue with priorty, duplicates are ignored*/
void add_with_priority(Queue* q, Cell c, int priority){
  struct Node* current = q->front;
  while (current != NULL){
    Cell cell = current->data;
    if (c.x == cell.x && c.y == cell.y) return;
    current = current->next;
  }
  add_with_priority_duplicates(q, c, priority);
}

/* Add to queue with priority, duplicates are allowed*/
void add_with_priority_duplicates(Queue* q, Cell c, int priority){
  struct Node* temp = (struct Node*)calloc(1, sizeof(struct Node));
	temp->data = c;
	temp->priority = priority;
	q->size++;
	if(q->front == NULL && q->rear == NULL){
	  q->front = q->rear = temp;
	  return;
  }
  struct Node* point = q->front;
  struct Node* prev = NULL;
  char added = 0;
  while (!added && point != NULL){
    if (temp->priority < point->priority){
      if (prev == NULL){
        temp->next = point;
        q->front = temp;
        added = 1;
      }else{
        temp->next = point;
        prev->next = temp;
        added = 1;
      }
    }
    prev = point;
    point = point->next;
  }
  if (!added) {
    q->rear->next = temp;
    q->rear = temp;
  }
}

/* Removes node with highest priority*/
void dequeue(Queue* q) {
	struct Node* temp = q->front;
	if(q->front == NULL) {
		return;
	}
	if(q->front == q->rear) {
		q->front = q->rear = NULL;
	}
	else {
		q->front = q->front->next;
	}
	free(temp);
	q->size--;
}

/* Modifies Node data pointer to data of the node with the highest priority*/
void peek(Queue* q, Cell* c) {
	if(q->front == NULL) {
		return;
	}
	*c = q->front->data;
}

/* Prints all the node data values and the size of the queue*/
void print_queue(Queue* q) {
	struct Node* temp = q->front;
	while(temp != NULL) {
		printf("x: %d, y: %d ",(temp->data).x, (temp->data).y);
		temp = temp->next;
	}
	printf("\nSize: %d\n", q->size);
}

void empty_queue(Queue* q){
  struct Node* temp = q->front;
  while (temp){
    q->front = q->front->next;
    free(temp);
    temp = q->front;
  }
  q->front = q->rear = NULL;
  q->size = 0;
}

/*int main(){
	// Drive code to test the implementation.
  Queue q;
  queue_init(&q);
  Cell c = {1, 2, 0, 0}, d = {3, 4, 0, 0};
  enqueue(&q, c);print_queue(&q);
  enqueue(&q, d);print_queue(&q);
  c.x = 5; c.y = 6;
  dequeue(&q);  print_queue(&q);
  enqueue(&q, c);print_queue(&q);
  c.x = 10; c.y = 11;
  add_with_priority(&q, c, -1);
  c.x = 50; c.y = 50;
  add_with_priority(&q, c, 1);
  add_with_priority(&q, c, 1);
  add_with_priority(&q, c, 1);
  print_queue(&q);
  c.x = 100; c.y =100;
  add_with_priority_duplicates(&q, c, 10);
  add_with_priority_duplicates(&q, c, 10);
  add_with_priority_duplicates(&q, c, 10);
  print_queue(&q);
  printf("empty queue");
  empty_queue(&q);print_queue(&q);
}*/



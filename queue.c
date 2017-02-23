/*Priority Queue - Linked List implementation*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "queue.h"

/*Initializes queue with function pointers, and other front and rear pointers*/
void queue_init(Queue* q, int (*node_equals)(void* data1, void* data2), void(*node_printer)(void* node)){
  q->front = NULL;
  q->rear = NULL;
  q->size = 0;
  q->printer = node_printer;
  q->equals = node_equals;
}

/* Add to queue without priority (linear queue)*/
void enqueue(Queue* q, void* data) {
	struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
	temp->data = data;
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
void add_with_priority(Queue* q, void* data, int priority){
  struct Node* current = q->front;
  while (current != NULL){
    void* curr_data = current->data;
    if (q->equals(curr_data, data)) return;
    current = current->next;
  }
  add_with_priority_duplicates(q, data, priority);
}

/* Add to queue with priority, duplicates are allowed*/
void add_with_priority_duplicates(Queue* q, void* data, int priority){
  struct Node* temp = (struct Node*)calloc(1, sizeof(struct Node));
	temp->data = data;
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

/*Change the priority of a node, works best on non-duplicates, selects all on duplicates(exact memory), select first on duplicates(different memeory)*/
void change_priority(Queue* q, void* data, int new_priority){
  int changed = 0;
  struct Node* current = q->front;
  struct Node* previous = NULL;
  while (current != NULL){
    void* curr_data = current->data;
    if (q->equals(curr_data, data)) {
      if (previous)
        previous->next = current->next;
      else{
          q->front = current->next;
      }
      if (q->rear == current) q->rear = previous;
      /*move to upper part*/
      if (current->priority > new_priority) {
        current->priority = new_priority;
        struct Node* curr = q->front;
        struct Node* prev = NULL;
        while ((previous != NULL) && curr != (previous->next)){
          if (new_priority < curr->priority){
            if (prev == NULL){
              current->next = curr;
              q->front = current;
            }else{
              current->next = curr;
              prev->next = current;
              
            }
            changed = 1;
            break;
          }
          prev = curr;
          curr = curr->next;
        }
        if (q->rear == NULL) q->rear = current;
      }
      else if (current->priority == new_priority) break;
      else if (current->priority < new_priority){/*move to lower part*/
        current->priority = new_priority;
        struct Node* curr = current->next;
        struct Node* prev = NULL;
        while (curr != NULL){
          if (new_priority < curr->priority){
            if (prev == NULL){
              current->next = curr;
              q->front = current;
            }else{
              prev->next = current;
              current->next = curr;
            }
            changed = 1;
            break;
          }
          prev = curr;
          curr = curr->next;
        }
      }
      break;
    }
    previous = current;
    current = current->next;
  }
  if (current == NULL) return;
  if (changed == 0){
    current->priority = new_priority;
    if (q->rear) (q->rear)->next = current;
    q->rear = current;
    (q->rear)->next = NULL;
    if(q->front == NULL) q->front = current; //should only apply when there's only one element
  }
}

/* Removes node with highest priority*/
void dequeue(Queue* q) {
	struct Node* temp = q->front;
	if(q->front == NULL)
		return;
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
void* peek(Queue* q, void* data) {
	if(q->front == NULL) return NULL;
	data = q->front->data;
	return data;
}

/* Prints all the node data values and the size of the queue*/
void print_queue(Queue* q) {
	struct Node* temp = q->front;
	while(temp != NULL) {
	  printf("Priority: %d ",temp->priority);
	  q->printer(temp->data);
		temp = temp->next;
	}
	printf("\nSize: %d\n", q->size);
}

/*Removes all elements from the queue*/
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
	// Drive code to test the implementation. The comments in the code don't actually apply. Passing the sae variable but changing it's values doesn't change anything
	
	//test 1 starts
	int i = 5;
	Queue q;
  queue_init(&q, equals, printer_n);
	while (i-->0){
	  printf("%d\n", i);
  Cell c = {1, 2, 0, 0};
  add_with_priority(&q, &c, 5); //test with add_with*
  print_queue(&q);
	}
	empty_queue(&q);
  print_queue(&q);
  //test 1 ends
  
  //test 2 starts
  // Queue q;
  // queue_init(&q, equals, printer_n);
  // Cell c = {1, 2, 0, 0};
  // enqueue(&q, &c);print_queue(&q);
  // dequeue(&q);  print_queue(&q);
  
  // Cell d = {3, 4, 0, 0};
  // enqueue(&q, &c);print_queue(&q);//1,2 : 1
  // enqueue(&q, &d);print_queue(&q);//1,2 3,4 :2
  // c = {5,6};
  // dequeue(&q);  print_queue(&q);//3,4 :1
  // enqueue(&q, &c);print_queue(&q);//3,4 5,6 :2
  // c= {10,11};
  
  // add_with_priority(&q, &c, -1);
  // print_queue(&q);//3,4 5,6 10,11 :3
  // c = {50,50};
  // add_with_priority(&q, &c, 1);//!50, 50 :4
  // add_with_priority(&q, &c, 1);
  // add_with_priority(&q, &c, 1);//! :4
  // print_queue(&q);//3,4 5,6 10,11 50,50 :4
  // c = {100,100};
  // add_with_priority_duplicates(&q, &c, 10);
  // add_with_priority_duplicates(&q, &c, 10);
  // add_with_priority_duplicates(&q, &c, 10);
  // print_queue(&q);
  // printf("empty queue");
  // empty_queue(&q);
  // print_queue(&q);
  // printf("c: %d %d d: %d %d\n", c.x, c.y, d.x, d.y);
  //test 2 ends
}*/



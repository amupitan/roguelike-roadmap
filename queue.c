/*Queue - Linked List implementation*/
#include<stdio.h>
#include<stdlib.h>

#include "game.h"

struct Node {
	Cell data;
	struct Node* next;
};
// Two glboal variables to store address of front and rear nodes.
struct Node* front = NULL;
struct Node* rear = NULL;

// To Enqueue a Cell
void Enqueue(Cell x) {
	struct Node* temp =
		(struct Node*)malloc(sizeof(struct Node));
	temp->data =x;
	temp->next = NULL;
	if(front == NULL && rear == NULL){
		front = rear = temp;
		return;
	}
	rear->next = temp;
	rear = temp;
}

// To Dequeue a cell
void Dequeue() {
	struct Node* temp = front;
	if(front == NULL) {
		// printf("Queue is Empty\n");
		return;
	}
	if(front == rear) {
		front = rear = NULL;
	}
	else {
		front = front->next;
	}
	free(temp);
}

Cell Front() {
	if(front == NULL) {
		printf("Queue is empty\n");
		return;
	}
	return front->data;
}

void Print() {
	struct Node* temp = front;
	while(temp != NULL) {
		printf("x: %d, y: %d ",(temp->data).x, (temp->data).y);
		temp = temp->next;
	}
	printf("\n");
}

int main(){
	/* Drive code to test the implementation. */
	// Printing elements in Queue after each Enqueue or Dequeue
// 	Enqueue(2); Print();
// 	Enqueue(4); Print();
// 	Enqueue(6); Print();
// 	Dequeue();  Print();
// 	Enqueue(8); Print();
  Cell c = {1, 2, 0, 0}, d = {3, 4, 0, 0};
  Enqueue(c);Print();
  Enqueue(d);Print();
  c.x = 5; c.y = 6;
  Dequeue();  Print();
  Enqueue(c);Print();
}



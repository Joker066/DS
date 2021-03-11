#include <stdlib.h>
#include <stdio.h>
/*
   node
 */
typedef struct node node;

node *new_node(int);

struct node {
	int v;
	node *prev;
	node *next;
};

node *new_node(int v) {
	node *n = (node*)malloc(sizeof(node));
	if (!n) return n;
	n->v = v;
	n->prev = NULL;
	n->next = NULL;
	return n;
}

/*
   queue_pop
 */
typedef struct queue queue;

queue *new_queue();
void queue_dispose(queue*);
int queue_is_empty(const queue*);
int queue_peek(const queue*);
int queue_push(queue*, int);
int queue_pop(queue*);

struct queue {
	node *head;
	node *tail;
	int size;
	void (*dispose)(queue *self);
	int (*is_empty)(const queue *self);
	int (*peek)(const queue *self);
	int (*push)(queue *self, int v);
	int (*pop)(queue *self);
};

queue *new_queue() {
	queue *q = (queue*)malloc(sizeof(queue));
	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
	q->dispose = queue_dispose;
	q->is_empty = queue_is_empty;
	q->peek = queue_peek;
	q->push = queue_push;
	q->pop = queue_pop;
	return q;
}

void queue_dispose(queue *self) {
	if (!self) return;
	node *cur = self->head;
	while (cur) {
		node *pre = cur;
		cur = cur->next;
		free(pre);
	}
	free(self);
}

int queue_is_empty(const queue *self) {
	return self->size == 0;
}

int queue_peek(const queue *self) {
	if (queue_is_empty(self)) {
		printf("it's empty\n");
		return -1;
	}
	return self->head->v;
}

int queue_push(queue *self, int v) {
	node *n = new_node(v);
	if (!n) return 0;
	self->size++;
	if (!(self->head)) {
		self->head = n;
		self->tail = n;
	}
	else {
		self->tail->next = n;
		n->prev = self->tail;
		self->tail = n;
	}
	return 1;
}

int queue_pop(queue *self) {
	if (queue_is_empty(self)) {
		printf("it's empty\n");
		return -1;
	}
	int pop = self->head->v;
	self->size--;
	if (self->head == self->tail) {
		free(self->head);
		self->head = NULL;
		self->tail = NULL;
	}
	else {
		node *cur = self->head;
		self->head = cur->next;
		free(cur);
	}
	return pop;
}

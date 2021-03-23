#include <stdio.h>
#include <stdlib.h>
typedef struct node node;
typedef struct xor_linkedlist x_linkedlist;

struct node {
	int data;
	node *link;
};

node *new_node(int v) {
	node *n = (node *)malloc(sizeof(node));
	n->data = v;
	n->link = NULL;
	return n;
}

node *_xor(node *a, node *b) {
	return (node *)((unsigned long)a ^ (unsigned long)b);
}

struct xor_linkedlist {
	node *head, *tail;
	int size;
};

x_linkedlist *new_x_linkedlist() {
	x_linkedlist *x = (x_linkedlist *)malloc(sizeof(x_linkedlist));
	x->head = NULL, x->tail = NULL;
	x->size = 0;
	return x;
}

int xl_is_empty(x_linkedlist *self) {
	return self->size == 0;
}

void xl_push_back(x_linkedlist *self, int v) {
	node *n = new_node(v);
	n->link = self->tail;
	if (xl_is_empty(self)) self->head = n;
	else self->tail->link = _xor(self->tail->link, n);
	self->tail = n;
	self->size++;
}

void xl_reverse(x_linkedlist *self) {
	node *temp = self->head;
	self->head = self->tail;
	self->tail = temp;
}

int xl_pop_back(x_linkedlist *self) {
	if (xl_is_empty(self)) return -1;
	int pop = self->tail->data;
	if (self->size == 1) {
		free(self->head);
		self->head = NULL, self->tail = NULL;
		self->size--;
		return pop;
	}
	xl_reverse(self);
	node *head = self->head;
	node *next = _xor(NULL, self->head->link);
	self->head = next;
	self->head->link = _xor(head, next->link);
	free(head);
	self->size--;
	xl_reverse(self);
	return pop;
}//reverse and delete head can make it O(1)! bravo!

void xl_concant(x_linkedlist *self, x_linkedlist *other) {
	if (xl_is_empty(other)) return;
	if (xl_is_empty(self)) {
		free(self);
		self = other;
		return;
	}
	self->tail->link = _xor(self->tail->link, other->head);
	other->head->link = _xor(other->head->link, self->tail);
	self->tail = other->tail;
	self->size += other->size;
}

void xl_print(x_linkedlist *self) {
	if (xl_is_empty(self)) {
		printf("empty.\n");
		return;
	}
	node *cur = self->head, *pre = NULL, *next;
	while (cur) {
		printf("%d ", cur->data);
		next = _xor(pre, cur->link);
		pre = cur;
		cur = next;
	}
	printf("\n");
}

//not done

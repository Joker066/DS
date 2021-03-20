#include <stdlib.h>
#include <stdio.h>

typedef struct node node;
typedef struct linkedlist linkedlist;
struct node {
	int v;
	node *next, *prev;
};

node *node_new(int v) {
	node *n = (node *)malloc(sizeof(node));
	n->v = v, n->next = NULL, n->prev = NULL;
	return n;
}

linkedlist *llist_new();
int llist_is_empty(const linkedlist *);
int llist_get_index(linkedlist *, int);
int llist_indexof(const linkedlist *, int);
void llist_push_back(linkedlist *, int);
int llist_pop_back(linkedlist *);
void llist_push_front(linkedlist *, int);
int llist_pop_front(linkedlist *);
void llist_insert(linkedlist *, int, int);
void llist_delete_index(linkedlist *, int);
void llist_delete_v(linkedlist *, int);
void llist_print(linkedlist *);

struct linkedlist {
	node *head, *tail;
	int size;
	int (*is_empty)(const linkedlist *self);
	int (*get_index)(linkedlist *self, int index);
	int (*indexof)(const linkedlist *self, int v);
	void (*push_back)(linkedlist *self, int v);
	int (*pop_back)(linkedlist *self);
	void (*push_front)(linkedlist *self, int v);
	int (*pop_front)(linkedlist *self);
	void (*insert)(linkedlist *self, int index, int v);
	void (*delete_index)(linkedlist *self, int index);
	void (*delete_v)(linkedlist *self, int v);
	void (*print)(linkedlist *self);
};

linkedlist *llist_new() {
	linkedlist *llist = (linkedlist *)malloc(sizeof(linkedlist));
	llist->size = 0;
	llist->head = NULL;
	llist->tail = NULL;
	llist->is_empty = llist_is_empty;
	llist->get_index = llist_get_index;
	llist->indexof = llist_indexof;
	llist->push_back = llist_push_back;
	llist->pop_back = llist_pop_back;
	llist->push_front = llist_push_front;
	llist->pop_front = llist_pop_front;
	llist->insert = llist_insert;
	llist->delete_index = llist_delete_index;
	llist->delete_v = llist_delete_v;
	llist->print = llist_print;
	return llist;
}

int llist_is_empty(const linkedlist *self) {
	return self->size == 0;
}

int llist_get_index(linkedlist *self, int index) {
	if ((index >= self->size) || (index < 0)) {
		printf("index error\n");
		return -1;
	}
	node *cur = self->head;
	for (int i = 0; i < index; i++) {
		cur = cur->next;
	}
	return cur->v;
}

int llist_indexof(const linkedlist *self, int v) {
	node *cur = self->head;
	int index = 0;
	while (cur) {
		if (cur->v == v) {
			return index;
		}
		cur = cur->next, index++;
	}
	printf("not in llist\n");
	return -1;
}

void llist_push_back(linkedlist *self, int v) {
	node *n = node_new(v);
	if (llist_is_empty(self)) {
		self->head = n;
		self->tail = n;
		self->size++;
		return;
	}
	self->size++;
	self->tail->next = n;
	n->prev = self->tail;
	self->tail = n;
}

int llist_pop_back(linkedlist *self) {
	if (llist_is_empty(self)) {
		printf("empty error\n");
		return -1;
	}
	int pop = self->tail->v;
	self->size--;
	if (self->head == self->tail) {
		free(self->tail);
		self->head = NULL;
		self->tail = NULL;
	}
	else {
		node *cur = self->tail;
		self->tail = cur->prev;
		free(cur);
		self->tail->next = NULL;
	}
	return pop;
}

void llist_push_front(linkedlist *self, int v) {
	node *n = node_new(v);
	if (llist_is_empty(self)) {
		self->head = n;
		self->tail = n;
		self->size++;
		return;
	}
	self->size++;
	self->head->prev = n;
	n->next = self->head;
	self->head = n;
}

int llist_pop_front(linkedlist *self) {
	if (llist_is_empty(self)) {
		printf("empty error\n");
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

void llist_insert(linkedlist *self, int index, int v) {
	if ((index > self->size) || (index < 0)) {
		printf("index error\n");
		return;
	}
	if (index == 0) {
		llist_push_front(self, v);
		return;
	}
	if (index == self->size) {
		llist_push_back(self, v);
		return;
	}
	node *n = node_new(v);
	node *cur = self->head;
	for (int i = 0; i < index - 1; i++) {
		cur = cur->next;
	}
	cur->next->prev = n;
	n->next = cur->next;
	n->prev = cur;
	cur->next = n;
	self->size++;
}

void llist_delete_index(linkedlist *self, int index) {
	if (index >= self->size) {
		printf("index error\n");
		return;
	}
	if (index == 0) {
		llist_pop_front(self);
		return;
	}
	if (index == self->size - 1) {
		llist_pop_back(self);
		return;
	}
	node *cur = self->head;
	node *n = cur->next;
	cur->next = cur->next->next;
	cur->next->prev = cur;
	free(n);
	self->size--;
}

void llist_delete_v(linkedlist *self, int v) {
	if (llist_is_empty(self)) {
		return;
	}
	self->size--;
	node *cur = self->head;
	while (cur->next) {
		if (cur->v == v) {
			cur->prev->next = cur->next;
			cur->next->prev = cur->prev;
			free(cur);
			return;
		}
		cur = cur->next;
	}
}

void llist_print(linkedlist *self) {
	if (llist_is_empty(self)) {
		return;
	}
	node *cur = self->head;
	printf("[");
	while (cur->next->next) {
		printf("%d, ", cur->v);
		cur = cur->next;
	}
	printf("%d, %d] len:%d\n", cur->v, cur->next->v, self->size);
}

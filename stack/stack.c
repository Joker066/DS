#include <stdio.h>
#include <stdlib.h>
typedef struct node node;
typedef struct stack stack;
int stack_peek(const stack *);
int stack_pop(stack *);
void stack_push(stack *, int);
int stack_isempty(const stack *);
struct node {
	int data;
	node *prev;
};

node *new_node(int v) {
	node *n = (node *)malloc(sizeof(node));
	n->data = v;
	n->prev = NULL;
	return n;
}

struct stack {
	node *top;
	int size;
	int (*peek)(const stack *self);
	int (*pop)(stack *self);
	void (*push)(stack *self, int data);
	int (*isempty)(const stack *self);
};

stack *new_stack() {
	stack *s = (stack *)malloc(sizeof(stack));
	s->top = (node *)malloc(sizeof(node));
	s->size = 0;
	s->peek = stack_peek;
	s->pop = stack_pop;
	s->push = stack_push;
	s->isempty = stack_isempty;
	return s;
}

void stack_dispose(stack *self) {
	node* temp;
	for (node *cur = self->top; cur;) {
		temp = cur;
		cur = cur->prev;
		free(temp);
	}
	free(self);
}

int stack_peek(const stack *self) {
	return self->top->data;
}

int stack_pop(stack *self) {
	self->size--;
	node *top = self->top;
	int pop = top->data;
	self->top = self->top->prev;
	free(top);
	return pop;
}

void stack_push(stack *self, int v) {
	self->size++;
	node *top = new_node(v);
	top->prev = self->top;
	self->top = top;
}

int stack_isempty(const stack *self) {
	return self->size == 0;
}

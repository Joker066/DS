#include <stdlib.h>
#include <stdio.h>

typedef struct vector vector;
//test
//API prototype
vector *new_vector();
void vector_dispose(vector*);
void vector_expand(vector*);
void vector_print(const vector*);
int vector_size(const vector*);
int vector_is_empty(const vector*);
int vector_peek_front(const vector*);
int vector_peek_back(const vector*);
int vector_at(const vector*, int index);
void vector_set(vector*, int index, int value);
void vector_push_front(vector*, int);
void vector_push_back(vector*, int);
int vector_pop_front(vector*);
int vector_pop_back(vector*);
void vector_insert(vector*, int, int);
void vector_remove(vector*, int);
//def
struct vector {
	int size, capacity;
	int head, tail;
	int *array;
	void (*dispose)(vector *self);
	void (*print)(const vector *self);
	int (*length)(const vector *self);
	int (*is_empty)(const vector *self);
	int (*peek_front)(const vector *self);
	int (*peek_back)(const vector *self);
	int (*at)(const vector *self, int dex);
	void (*set)(vector *self, int index, int value);
	void (*push_front)(vector *self, int value);
	void (*push_back)(vector *self, int value);
	int (*pop_front)(vector *self);
	int (*pop_back)(vector *self);
	void (*insert)(vector *self, int index, int value);
	void (*remove)(vector *self, int index);
};
//test
//API def
vector *new_vector(){
	vector *v = (vector*)malloc(sizeof(vector));
	v->size = 0, v->capacity = 2;
	v->head = 0, v->tail = 0;
	v->array = (int*)malloc(v->capacity * sizeof(int));
	v->dispose = vector_dispose;
	v->print = vector_print;
	v->length = vector_size;
	v->is_empty = vector_is_empty;
	v->peek_front = vector_peek_front;
	v->peek_back = vector_peek_back;
	v->at = vector_at;
	v->set = vector_set;
	v->push_front = vector_push_front;
	v->push_back = vector_push_back;
	v->pop_front = vector_pop_front;
	v->pop_back = vector_pop_back;
	v->insert = vector_insert;
	v->remove = vector_remove;
	return v;
}

void vector_dispose(vector *self){
	free(self->array);
	free(self);
}

void vector_expand(vector *self){
	if (self->size < self->capacity) return;
	self->capacity <<= 1;
	int *ori_arr = self->array;
	int *new_arr = (int*)malloc(self->capacity * sizeof(int));
	int i = self->head, j = 0;
	int s = 0;
	while (s < self->size) {
		new_arr[j] = ori_arr[i];
		i = (i + 1) % self->capacity;
		j = (j + 1) % self->capacity;
		s++;
	}
	self->array = new_arr;
	self->head = 0;
	self->tail = (self->head + self->size - 1) % self->capacity;
	free(ori_arr);
}

void vector_print(const vector *self){
	if (vector_is_empty(self)) {
		printf("empty.\n");
		return;
	}
	printf("[");
	if (self->head <= self->tail) {
		for (int i = self->head; i < self->tail; i++)
			printf("%d, ", self->array[i]);
	}
	else{
		for (int i = self->head; i < self->tail + self->capacity; i++) {
			i %= self->capacity;
			printf("%d, ", self->array[i]);
		}
	}
	printf("%d]\n", self->array[self->tail]);
}

int vector_size(const vector *self){
	return self->size;
}

int vector_is_empty(const vector *self){
	return self->size == 0;
}

int vector_peek_front(const vector *self){
	if (vector_is_empty(self)) {
		printf("vector is empty. error code: ");
		return -1;
	}
	return self->array[self->head];
}

int vector_peek_back(const vector *self){
	if (vector_is_empty(self)) {
		printf("vector is empty. error code: ");
		return -1;
	}
	return self->array[self->tail];
}

int vector_at(const vector *self, int index){
	if (vector_is_empty(self)) {
		printf("vector is empty. error code: ");
		return -1;
	}
	if (index >= self->size) {
		printf("index out of range. error code: ");
		return -2;
	}
	return self->array[(self->head + index) % self->capacity];
}

void vector_set(vector *self, int index, int value){
	if (vector_is_empty(self)) printf("vector is empty. error code: -1\n");
	if (index >= self->size) printf("index out of range. error code: -2\n");
	self->array[(self->head + index) % self->capacity] = value;
}

void vector_push_front(vector *self, int value){
	vector_expand(self);
	if (self->size > 0) self->head = !self->head ? self->capacity - 1 : self->head - 1;
	self->array[self->head] = value;
	self->size++;
}

void vector_push_back(vector *self, int value){
	vector_expand(self);
	if (self->size > 0) self->tail = (self->tail + 1) % self->capacity;
	self->array[self->tail] = value;
	self->size++;
}

int vector_pop_front(vector *self){
	if (vector_is_empty(self)) {
		printf("vector is empty. error code: ");
		return -1;
	}
	int pop = self->array[self->head];
	self->head = (self->head + 1) % self->capacity;
	self->size--;
	return pop;
}

int vector_pop_back(vector *self){
	if (vector_is_empty(self)) {
		printf("vector is empty. error code: ");
		return -1;
	}
	int pop = self->array[self->tail];
	self->tail = !self->size ? self->capacity - 1 : self->tail - 1;
	self->size--;
	return pop;
}

void vector_insert(vector *self, int index, int value){
	if (index < 0 || index > self->size) printf("index out of range. error code: -2\n");
	if (index == self->size) {
		vector_push_back(self, value);
		return;
	}
	int new_index = (self->head + index) % self->capacity;
	if (vector_is_empty(self)) {
		self->array[new_index] = value;
		self->size++;
		return;
	}
	if (self->size < self->capacity) {
		int i = self->tail;
		int s = 0;
		while (s < self->size) {
			if (self->head <= self->tail || new_index <= self->tail) {
				if (new_index <= i) self->array[(i + 1) % self->capacity] = self->array[i];
				if (new_index == i) {
					self->array[i] = value;
					break;
				}
			}
			else if (self->head <= new_index) {
				if (i <= self->tail || i >= new_index) {
					self->array[(i + 1) % self->capacity] = self->array[i];
				}
				if (i == new_index) {
					self->array[i] = value;
					break;
				}
			}
			i = !i ? self->capacity - 1 : i - 1;
			s++;
		}
		self->tail = (self->tail + 1) % self->capacity;
	}
	else{
		self->capacity <<= 1;
		int *ori_arr = self->array;
		int *new_arr = (int*)malloc(self->capacity * sizeof(int));
		int i = self->tail, j = self->size - 1;
		int s = 0;
		while (s < self->size) {
			if (self->head <= self->tail || new_index <= self->tail) {
				if (new_index <= i) {
					new_arr[(j + 1) % self->capacity] = ori_arr[i];
					if (i == new_index) new_arr[j] = value;
				}
				else new_arr[j] = ori_arr[i];
			}
			else if (self->head <= new_index) {
				if (i <= self->tail || new_index <= i) {
					new_arr[(j + 1) % self->capacity] = ori_arr[i];
					if (i == new_index) new_arr[j] = value;
				}
				else new_arr[j] = ori_arr[i];
			}
			i = !i ? self->size - 1 : i -1;
			j = !j ? self->capacity - 1 : j - 1;
			s++;
		}
		self->array = new_arr;
		free(ori_arr);
		self->head = 0;
		self->tail = self->size;
	}
	self->size++;
}

void vector_remove(vector *self, int index){
	if (vector_is_empty(self)) printf("vector is empty. error code: -1\n");
	if (index >= self->size) printf("index out of range. error code: -2\n");
	int new_index = (self->head + index) % self->capacity;
	if (self->size == 1) {
		self->tail = !self->tail ? self->size - 1 : self->tail - 1;
		self->size--;
		return;
	}
	int i = self->head;
	int s = 0;
	while (s < self->size) {
		if (self->head <= self->tail) {
			if (new_index <= i) self->array[i] = self->array[(i + 1) % self->capacity];
		}
		else {
			if (new_index <= self->tail) {
				if (new_index <= i && i <= self->tail) self->array[i] = self->array[(i + 1) % self->capacity];
			}
			else {
				if (i <= self->tail || new_index <= i) self->array[i] = self->array[(i + 1) % self->capacity];
			}
		}
		i = (i + 1) % self->capacity;
		s++;
	}
	self->tail = !self->tail ? self->capacity - 1 : self->tail - 1;
	self->size--;
}

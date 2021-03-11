#include <stdio.h>
#include <stdlib.h>
typedef struct ordered_array ordered_array;
void ordered_array_insert(ordered_array *self, int data);
void ordered_array_update(ordered_array *self, int index, int data);
void ordered_array_del(ordered_array *self, int index);
int ordered_array_search(ordered_array *self, int data);
void ordered_array_print(ordered_array *self);
static void swap(int *x, int *y){
	int temp = *x;
	*x = *y;
	*y = temp;
}

struct ordered_array {
	int A[1000];
	int len;
	void (*insert)(ordered_array *self, int data);
	void (*update)(ordered_array *self, int index, int data);
	void (*del)(ordered_array *self, int index);
	int (*search)(ordered_array *self, int data);
	void (*print)(ordered_array *);
};

void insertion_sort(ordered_array *self, int len){
	for (int i = 1; i < len; i++) {
		int j = i;
		while (self->A[j] < self->A[j - 1] && j > 0) {
			swap(&self->A[j], &self->A[j - 1]);
			j--;
		}
	}
}

ordered_array* new_ordered_array(int A[], int len){
	ordered_array *o = (ordered_array*)malloc(sizeof(ordered_array));
	o->len = len;
	for(int i = 0; i < len; i++) {
		o->A[i] = A[i];
	}
	o->insert = ordered_array_insert;
	o->update = ordered_array_update;
	o->del = ordered_array_del;
	o->search = ordered_array_search;
	o->print = ordered_array_print;
	return o;
}

void ordered_array_insert(ordered_array *self, int data){
	self->A[self->len++] = data;
	int i = self->len - 1;
	while (self->A[i] < self->A[i - 1] && i > 0) {
		swap(&self->A[i], &self->A[i - 1]);
		i--;
	}
}

void ordered_array_update(ordered_array *self, int index, int data){
	int ori = self->A[index];
	self->A[index] = data;
	int j = index;
	if (data > ori) {
		while (self->A[j] > self->A[j + 1] && j + 1 < self->len) {
			swap(&self->A[j], &self->A[j + 1]);
			j++;
		}
	}
	else if (data < ori) {
		while (self->A[j] < self->A[j - 1] && j > 0) {
			swap(&self->A[j], &self->A[j - 1]);
			j--;
		}
	}
}

void ordered_array_del(ordered_array *self, int index){
	for (int i = index; i < self->len; i++) {
		self->A[i] = self->A[i + 1];
	}
	self->len--;
}

int ordered_array_search(ordered_array *self, int data){
	// for (int i = 0; i < self->len; i++) {
	// 	if (self->A[i] == data) return i;
	// 	if (self->A[i] > data) break;
	// }
	int left = 0, right = self->len - 1, mid;
	while (left <= right){
		mid = (left + right) / 2;
		if (data < self->A[mid]) right = mid - 1;
		else if (data > self->A[mid]) left = mid + 1;
		else return mid;
	}
	return -1;
}//use binary search can make it faster

void ordered_array_print(ordered_array *self){
	for (int i = 0; i < self->len; i++) {
		printf("%d ", self->A[i]);
	}
	printf("\n");
}

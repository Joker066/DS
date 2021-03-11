#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
   this code implement set with C.
   use -lm when compiling on gcc.
 */

/*typedef*/
typedef struct item item;
typedef struct set set;

/*prime*/

//constant
const int prime_1 = 151, prime_2 = 163;

//API prototype
int is_prime(const int x);
int next_prime(int x);

//API def
int is_prime(const int x){
	if (x < 2) {
		return -1;
	}//undefined
	if ((x == 2) || (x == 3)) {
		return 1;
	}//prime
	if (x % 2 == 0) {
		return 0;
	}//not prime
	for (int i = 3; i <= floor(sqrt((double)x)); i++) {
		if (x % i == 0) {
			return 0;
		}
	}
	return 1;
}

int next_prime(int x){
	while (is_prime(x) != 1) {
		x++;
	}
	return x;
}

/*hash*/

//API prototype
int hash();
int get_hash();

//API def
int hash(const char *s, const int a, const int m){
	long hash = 0;
	int len_s = strlen(s);
	for (int i = 0; i < len_s; i++) {
		hash += (long)pow(a, len_s - i - 1) * s[i];
		hash %= m;
	}
	return (int)hash;
}

int get_hash(const char*s, const int num_buckets, int attempt){
	int hash_a = hash(s, prime_1, num_buckets);
	int hash_b = hash(s, prime_2, num_buckets);
	return (hash_a + attempt * (hash_b + 1)) % num_buckets;
}

/*item for set*/

//def
struct item {
	char *key;
};

//constant
item set_deleted_item = {NULL};

//API prototype
item *item_new();
void item_dispose();

//API def
item *item_new(const char* key){
	item* i = (item *)malloc(sizeof(item));
	i->key = strdup(key); // str copy
	return i;
}

void item_dispose(item *i){
	free(i->key);
	free(i);
}

/*set*/

//constant
const int set_init_base_size = 50;

//API prototype
set *new_set();
set *set_new_sized(const int);
void set_dispose(set*);
void set_resize(set*, const int);
void set_resize_up(set*);
void set_resize_down(set*);
void set_insert(set*, const char*);
void set_delete(set*, const char*);
char *set_search(set*, const char*);

//def
struct set {
	int base_size;
	int size;
	int count;
	item **items;
	void (*dispose)(set *self);
	void (*insert)(set *self, const char *key);
	void (*del)(set *self, const char *key);
	char* (*search)(set *self, const char *key);
};

//API def
set *set_new_sized(const int base_size){
	set* d = (set*) malloc(sizeof(set));
	d->base_size = base_size;
	d->size      = next_prime(base_size);
	d->count = 0;
	d->items = (item**)calloc((size_t)d->size, sizeof(item*));//initialize value
	return d;
}

set *new_set(){
	set *d = set_new_sized(set_init_base_size);
	d->dispose = set_dispose;
	d->insert = set_insert;
	d->del = set_delete;
	d->search = set_search;
	return d;
}

void set_dispose(set *d){
	for (int i = 0; i < d->size; i++) {
		item *it = d->items[i];
		if (it != NULL) {
			item_dispose(it);
		}
	}
	free(d->items);
	free(d);
}

void set_resize(set *self, const int base_size){
	if (base_size < set_init_base_size) {
		return;
	}
	set *new_d = set_new_sized(base_size);
	for (int i = 0; i < self->size; i++) {
		item *it = self->items[i];
		if (it && (it != &set_deleted_item)) {
			set_insert(new_d, it->key);
		}
	}
	self->base_size = new_d->base_size;
	self->count = new_d->count;
	int temp_size = self->size;
	self->size  = new_d->size;
	new_d->size = temp_size;
	item **temp_items = self->items;
	self->items  = new_d->items;
	new_d->items = temp_items;
	set_dispose(new_d);
}

void set_resize_up(set *self){
	int new_base_size = self->base_size * 2;
	set_resize(self, new_base_size);
}

void set_resize_down(set *self){
	int new_base_size = self->base_size / 2;
	set_resize(self, new_base_size);
}

void set_insert(set *self, const char *key){
	int load = self->count * 100 / self->size;
	if (load > 70) {
		set_resize_up(self);
	}
	item *i = item_new(key);
	int index = get_hash(i->key, self->size, 0);
	item *cur = self->items[index];
	int j = 1;
	while (cur && cur != &set_deleted_item) {
		if (cur != &set_deleted_item) {
			if (strcmp(cur->key, key) == 0) {
				item_dispose(cur);
				self->items[index] = i;
				return;
			}
		}
		index = get_hash(i->key, self->size, j);
		cur = self->items[index];
		j++;
	}
	self->items[index] = i;
	self->count++;
}

void set_delete(set *self, const char *key){
	int load = self->count * 100 / self->size;
	if (load < 10) {
		set_resize_down(self);
	}
	int index = get_hash(key, self->size, 0);
	item *i = self->items[index];
	int j = 1;
	while (i) {
		if (i != &set_deleted_item) {
			if (strcmp(i->key, key) == 0) {
				item_dispose(i);
				self->items[index] = &set_deleted_item;
			}
		}
		index = get_hash(key, self->size, j);
		i = self->items[index];
		j++;
	}
	self->count--;
}

char* set_search(set *self, const char *key){
	int index = get_hash(key, self->size, 0);
	item *i = self->items[index];
	int j = 1;
	while (i && i != &set_deleted_item) {
		if (strcmp(i->key, key) == 0) {
			return (char*)"true";
		}
		index = get_hash(key, self->size, j);
		i = self->items[index];
		j++;
	}
	return (char*)"false";
}

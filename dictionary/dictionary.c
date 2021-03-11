#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
   this code implement dictionary with C.
   use -lm when compiling on gcc.
 */

/*typedef*/
typedef struct item		  item;
typedef struct dictionary dict;

/*prime*/

//constant
const int prime_1 = 151, prime_2 = 163;

//API prototype
int is_prime(const int x);
int next_prime(int x);

//API def
int is_prime(const int x){
    if (x < 2){
        return -1;
    }//undefined
    if ((x == 2) || (x == 3)){
        return 1;
    }//prime
    if (x % 2 == 0){
        return 0;
    }//not prime
    for (int i = 3; i <= floor(sqrt((double)x)); i++){
        if (x % i == 0){
            return 0;
        }
    }
    return 1;
}

int next_prime(int x){
    while (is_prime(x) != 1){
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
    for (int i = 0; i < len_s; i++){
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

/*item for dictionary*/

//def
struct item {
    char *key, *value;
};

//constant
item dict_deleted_item = {NULL, NULL};

//API prototype
item *item_new();
void item_dispose();

//API def
item *item_new(const char* k, const char* v){
    item* i = (item *)malloc(sizeof(item));
    i->key = strdup(k); // str copy
    i->value = strdup(v);
    return i;
}

void item_dispose(item *i){
    free(i->key);
    free(i->value);
    free(i);
}

/*dictionary*/

//def
struct dictionary {
    int	   base_size;
    int	   size;
    int	   count;
    item **items;
    void (*dispose)(dict *self);
    void (*insert)(dict *self, const char *key, const char *value);
    void (*del)(dict *self, const char *key);
    char *(*search)(dict *self, const char *key);
};

//constant
const int dict_init_base_size = 50;

//API prototype
dict *new_dict();
dict *dict_new_sized(const int);
void dict_dispose(dict*);
void dict_resize(dict*, const int);
void dict_resize_up(dict*);
void dict_resize_down(dict*);
void dict_insert(dict*, const char*, const char*);
void dict_del(dict*, const char*);
char *dict_search(dict*, const char*);

//API def
dict *dict_new_sized(const int base_size){
    dict* d = (dict*) malloc(sizeof(dict));
    d->base_size = base_size;
    d->size	 = next_prime(base_size);
    d->count = 0;
    d->items = (item**)calloc((size_t)d->size, sizeof(item*));//initialize value
    return d;
}

dict *new_dict(){
    dict *d = dict_new_sized(dict_init_base_size);
    d->dispose = dict_dispose;
    d->insert = dict_insert;
    d->del = dict_del;
    d->search = dict_search;
    return d;
}

void dict_dispose(dict *d){
    for (int i = 0; i < d->size; i++){
        item *it = d->items[i];
        if (it != NULL){
            item_dispose(it);
        }
    }
    free(d->items);
    free(d);
}

void dict_resize(dict *self, const int base_size){
    if (base_size < dict_init_base_size){
        return;
    }
    dict *new_d = dict_new_sized(base_size);
    for (int i = 0; i < self->size; i++){
        item *it = self->items[i];
        if (it && (it != &dict_deleted_item)){
            dict_insert(new_d, it->key, it->value);
        }
    }
    self->base_size = new_d->base_size;
    self->count = new_d->count;
    int temp_size = self->size;
    self->size	= new_d->size;
    new_d->size = temp_size;
    item **temp_items = self->items;
    self->items	 = new_d->items;
    new_d->items = temp_items;
    dict_dispose(new_d);
}

void dict_resize_up(dict *self){
    int new_base_size = self->base_size * 2;
    dict_resize(self, new_base_size);
}

void dict_resize_down(dict *self){
    int new_base_size = self->base_size / 2;
    dict_resize(self, new_base_size);
}

void dict_insert(dict *self, const char *key, const char *value){
    int load = self->count * 100 / self->size;
    if (load > 70){
        dict_resize_up(self);
    }
    item *i = item_new(key, value);
    int index = get_hash(i->key, self->size, 0);
    item *cur = self->items[index];
    int j = 1;
    while (cur && cur != &dict_deleted_item){
        if (cur != &dict_deleted_item){
            if (strcmp(cur->key, key) == 0){
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

void dict_del(dict *self, const char *key){
    int load = self->count * 100 / self->size;
    if (load < 10){
        dict_resize_down(self);
    }
    int index = get_hash(key, self->size, 0);
    item *i = self->items[index];
    int j = 1;
    while (i){
        if (i != &dict_deleted_item){
            if (strcmp(i->key, key) == 0){
                item_dispose(i);
                self->items[index] = &dict_deleted_item;
            }
        }
        index = get_hash(key, self->size, j);
        i = self->items[index];
        j++;
    }
    self->count--;
}

char *dict_search(dict *self, const char *key){
    int index = get_hash(key, self->size, 0);
    item *i = self->items[index];
    int j = 1;
    while (i && i != &dict_deleted_item){
        if (strcmp(i->key, key) == 0){
            return i->value;
        }
        index = get_hash(key, self->size, j);
        i = self->items[index];
        j++;
    }
    printf("%s is not in the dictionary.", key);
    return (char*)"";
}

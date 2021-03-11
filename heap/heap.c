#include <stdio.h>
#include <stdlib.h>

/*
   this code implement min_heap with C
 */

/*typedef*/
typedef struct item item;
typedef struct heap heap;

/*item for heap*/
struct item {
    int value;
    int weight;
};

//API prototype
item *item_new(const int value, const int weight);
void item_dispose(item *x);

//API def
item *item_new(const int value, const int weight){
    item *i = (item *)malloc(sizeof(item));
    i->value  = value;
    i->weight = weight;
    return i;
}

void item_dispose(item* x){
    free(x);
}

/*heap*/
struct heap {
    item **h;
    int	   size;
};

//API prototype
heap *heap_new();
void heap_push(heap *self, item *x);
int heap_pop(heap *self);

//API def
heap *heap_new(){
    heap *h = (heap *)malloc(sizeof(heap));
    h->h = (item **)calloc((size_t)1000, sizeof(item*));
    h->size = 0;
    return h;
}

void heap_push(heap *self, item *x){
    int index = self->size++;
    while (index > 0){
        int parent = (index - 1) / 2;
        if (self->h[parent]->weight <= x->weight){
            break;
        }
        self->h[index] = self->h[parent];
        index = parent;
    }
    self->h[index] = x;
}

int heap_pop(heap *self){
    int top = self->h[0]->value;
    item *x = self->h[--self->size];
    int index = 0;
    while (index * 2 + 1 < self->size){
        item *left = self->h[index * 2 + 1], *right = self->h[index * 2 + 2];
        if ((index * 2 + 2 < self->size) && (right->weight < left->weight)){
            left = right;
        }
        if (left->weight >= x->weight){
            break;
        }
        self->h[index] = left;
        index = index * 2 + 1;
    }
    self->h[index] = x;
    return top;
}

int main(){
    return 0;
}

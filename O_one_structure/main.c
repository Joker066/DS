#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define m 10000
#define l 10000
typedef struct set set;
struct set {
    int *set;
    int *index;
    int k;
};

set *set_new() {
    set *s = malloc(sizeof(set));
    s->set = malloc((m + 1) * sizeof(int));
    s->index = malloc(l * sizeof(int));
    s->k = 0;
    return s;
}

void set_insert(set *self, int v) {
    if (self->set[v]) return;
    self->k++;
    self->set[v] = self->k;
    self->index[self->k] = v;
}

bool set_search(set *self, int v) {
    if (1 <= self->set[v] && self->set[v] <= self->k &&
        self->index[self->set[v]] == v)
        return true;
    return false;
}

void set_delete(set *self, int v) {
    self->set[self->index[self->k]] = self->set[v];
    self->index[self->set[v]] = self->index[self->k];
    self->k--;
}

int main() {
    srand(time(NULL));
    set *s = set_new();
    for (int i = 1; i <= l; i++)
        set_insert(s, rand() % m + 1);
    for (int i = 1; i <= 10; i++)
        printf("%d:%d\n", i, s->set[i]);
    printf("%d\n", set_search(s, 1));
    set_delete(s, 1);
    printf("%d\n", set_search(s, 1));
    return 0;
}

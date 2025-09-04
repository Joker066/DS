#include "sparse_set.h"

#include <stdlib.h>
#include <stdio.h>   /* only used by sset_debug_print */

/* Representation (sparse/dense):
   - pos[key] = index in dense[] if present, or SENTINEL if absent.
   - dense[i] = key stored at dense index i, for i in [0, size).
   Insertion places key at dense[size] and records pos[key] = size.
   Erase swaps last element into the removed slot and updates its pos[]. */

struct sparse_set {
    size_t *pos;       /* length = universe, maps key -> dense index or SENTINEL */
    size_t *dense;     /* length = capacity, stores keys */
    size_t   universe; /* number of distinct keys representable: [0, universe) */
    size_t   capacity; /* maximum number of stored keys */
    size_t   size;     /* current number of stored keys */
};

static const size_t POS_SENTINEL = (size_t)(-1);

/* ---- Lifecycle ---------------------------------------------------------- */

sset *sset_new(size_t universe, size_t capacity) {
    if (universe == 0 || capacity == 0) {
        return NULL;
    }
    else if (capacity > universe) {
        /* Capacity cannot exceed universe size for unique keys. */
        return NULL;
    }
    else {
        sset *s = (sset*)calloc(1, sizeof(*s));
        if (!s) {
            return NULL;
        }
        else {
            s->pos = (size_t*)malloc(universe * sizeof(size_t));
            s->dense = (size_t*)malloc(capacity * sizeof(size_t));
            if (!s->pos || !s->dense) {
                free(s->pos);
                free(s->dense);
                free(s);
                return NULL;
            }
            else {
                for (size_t i = 0; i < universe; ++i) {
                    s->pos[i] = POS_SENTINEL;
                }
                s->universe = universe;
                s->capacity = capacity;
                s->size = 0;
                return s;
            }
        }
    }
}

void sset_free(sset *s) {
    if (!s) {
        return;
    }
    else {
        free(s->pos);
        free(s->dense);
        free(s);
        return;
    }
}

void sset_clear(sset *s) {
    if (!s) {
        return;
    }
    else {
        for (size_t i = 0; i < s->size; ++i) {
            size_t key = s->dense[i];
            s->pos[key] = POS_SENTINEL;
        }
        s->size = 0;
        return;
    }
}

/* ---- Queries ------------------------------------------------------------ */

size_t sset_universe(const sset *s) {
    if (!s) {
        return 0;
    }
    else {
        return s->universe;
    }
}

size_t sset_capacity(const sset *s) {
    if (!s) {
        return 0;
    }
    else {
        return s->capacity;
    }
}

size_t sset_size(const sset *s) {
    if (!s) {
        return 0;
    }
    else {
        return s->size;
    }
}

bool sset_contains(const sset *s, size_t key) {
    if (!s || key >= s->universe) {
        return false;
    }
    else {
        return s->pos[key] != POS_SENTINEL;
    }
}

/* ---- Mutations ---------------------------------------------------------- */

int sset_insert(sset *s, size_t key) {
    if (!s || key >= s->universe) {
        return SSET_ERR_RANGE;
    }
    else if (s->pos[key] != POS_SENTINEL) {
        return SSET_ERR_DUPLICATE;
    }
    else if (s->size >= s->capacity) {
        return SSET_ERR_FULL;
    }
    else {
        size_t i = s->size;
        s->dense[i] = key;
        s->pos[key] = i;
        s->size = i + 1;
        return SSET_OK;
    }
}

int sset_erase(sset *s, size_t key) {
    if (!s || key >= s->universe) {
        return SSET_ERR_RANGE;
    }
    else {
        size_t i = s->pos[key];
        if (i == POS_SENTINEL) {
            return SSET_ERR_NOTFOUND;
        }
        else {
            size_t last_index = s->size - 1;
            size_t last_key = s->dense[last_index];

            /* Move last element into the removed slot (unless already last). */
            s->dense[i] = last_key;
            s->pos[last_key] = i;

            /* Mark removed key as absent and shrink size. */
            s->pos[key] = POS_SENTINEL;
            s->size = last_index;
            return SSET_OK;
        }
    }
}

/* ---- Access ------------------------------------------------------------- */

int sset_get_at(const sset *s, size_t i, size_t *out_key) {
    if (!s || i >= s->size) {
        return SSET_ERR_RANGE;
    }
    else {
        if (out_key) {
            *out_key = s->dense[i];
        }
        return SSET_OK;
    }
}

/* ---- Debug -------------------------------------------------------------- */

void sset_debug_print(const sset *s) {
    if (!s) {
        printf("sset(NULL)\n");
        return;
    }
    else {
        printf("sset(size=%zu, cap=%zu, uni=%zu): {", s->size, s->capacity, s->universe);
        for (size_t i = 0; i < s->size; ++i) {
            size_t key = s->dense[i];
            printf("%s%zu", (i ? ", " : ""), key);
        }
        printf("}\n");
        return;
    }
}

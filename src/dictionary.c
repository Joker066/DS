#include "dictionary.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>   

/* Configuration:
   - Open addressing with double hashing.
   - Base = next prime >= requested size.
   - Resizes on load-factor thresholds (grow >70%, shrink <10%). */

typedef struct item {
    char *key;
    char *value;
} item;

struct dictionary {
    size_t base_size;   /* logical sizing knob (pre-prime) */
    size_t size;        /* number of buckets (prime) */
    size_t count;       /* number of stored entries (excludes deleted tombstones) */
    item **items;       /* buckets: NULL / &deleted_sentinel / heap-allocated item */
};

/* ---- internals: constants, helpers ------------------------------------- */

static const unsigned PRIME_1 = 151u;
static const unsigned PRIME_2 = 163u;
static const size_t   DICT_INIT_BASE_SIZE = 50u;

/* Tombstone sentinel for deleted buckets (address used, contents ignored). */
static item DELETED_SENTINEL = { NULL, NULL };

/* strdup is not ISO C; provide a small replacement for portability. */
static char *dupstr(const char *s) {
    if (!s) {
        return NULL;
    }
    else {
        size_t n = strlen(s) + 1;
        char *p = (char*)malloc(n);
        if (!p) {
            return NULL;
        }
        else {
            memcpy(p, s, n);
            return p;
        }
    }
}

/* Prime utilities (simple trial division; sufficient for these sizes). */
static int is_prime(int x) {
    if (x < 2) {
        return 0;
    }
    else if (x == 2 || x == 3) {
        return 1;
    }
    else if ((x % 2) == 0) {
        return 0;
    }
    else {
        for (int i = 3; (long long)i * i <= x; i += 2) {
            if ((x % i) == 0) {
                return 0;
            }
            else {
                /* keep testing */
            }
        }
        return 1;
    }
}

static int next_prime(int x) {
    if (x <= 2) {
        return 2;
    }
    else {
        if ((x % 2) == 0) {
            x += 1;
        }
        while (!is_prime(x)) {
            x += 2;
        }
        return x;
    }
}

/* Polynomial rolling hash without pow(); computed left-to-right.
   Uses modular arithmetic to keep values in-range. */
static unsigned hash_poly(const char *s, unsigned a, unsigned m) {
    unsigned long long h = 0ull;
    for (const unsigned char *p = (const unsigned char*)s; *p; ++p) {
        h = (h * a + (unsigned long long)(*p)) % m;
    }
    return (unsigned)h;
}

/* Double hash: h(k, i) = (h1(k) + i*(h2(k)+1)) mod m */
static unsigned bucket_index(const char *s, unsigned m, unsigned attempt) {
    unsigned h1 = hash_poly(s, PRIME_1, m);
    unsigned h2 = hash_poly(s, PRIME_2, m);
    return (h1 + attempt * (h2 + 1u)) % m;
}

/* Bucket item helpers */
static item *item_new(const char *k, const char *v) {
    item *it = (item*)malloc(sizeof(*it));
    if (!it) {
        return NULL;
    }
    else {
        it->key = dupstr(k);
        it->value = dupstr(v ? v : "");
        if (!it->key || !it->value) {
            free(it->key);
            free(it->value);
            free(it);
            return NULL;
        }
        else {
            return it;
        }
    }
}

static void item_free(item *it) {
    if (!it || it == &DELETED_SENTINEL) {
        return;
    }
    else {
        free(it->key);
        free(it->value);
        free(it);
        return;
    }
}

/* ---- core dictionary routines ------------------------------------------ */

static dict *dict_new_sized(size_t base_size) {
    dict *d = (dict*)malloc(sizeof(*d));
    if (!d) {
        return NULL;
    }
    else {
        d->base_size = base_size;
        d->size  = (size_t)next_prime((int)base_size);
        d->count = 0;
        d->items = (item**)calloc(d->size, sizeof(item*));
        if (!d->items) {
            free(d);
            return NULL;
        }
        else {
            return d;
        }
    }
}

/* Inserts into a specific table without triggering resize; used by rehash. */
static int dict_insert_raw(dict *self, item *it) {
    unsigned attempt = 0;
    unsigned m = (unsigned)self->size;
    unsigned idx = bucket_index(it->key, m, attempt);

    while (self->items[idx] && self->items[idx] != &DELETED_SENTINEL) {
        if (strcmp(self->items[idx]->key, it->key) == 0) {
            /* Replace in-place (free old, keep slot) */
            item_free(self->items[idx]);
            self->items[idx] = it;
            return DICT_OK;
        }
        attempt++;
        idx = bucket_index(it->key, m, attempt);
    }

    self->items[idx] = it;
    self->count++;
    return DICT_OK;
}

/* Rehashes into a new table size; preserves entries. */
static int dict_resize(dict *self, size_t new_base_size) {
    if (new_base_size < DICT_INIT_BASE_SIZE) {
        return DICT_OK; /* ignore */
    }
    else {
        dict *nd = dict_new_sized(new_base_size);
        if (!nd) {
            return DICT_ERR_OOM;
        }
        else {
            for (size_t i = 0; i < self->size; ++i) {
                item *it = self->items[i];
                if (it && it != &DELETED_SENTINEL) {
                    /* transfer ownership of `it` into new table */
                    (void)dict_insert_raw(nd, it);
                }
                else {
                    /* skip empty or deleted */
                }
            }

            /* swap into place */
            size_t tmp_sz = self->size;
            self->size = nd->size;
            nd->size = tmp_sz;

            size_t tmp_base = self->base_size;
            self->base_size = nd->base_size;
            nd->base_size = tmp_base;

            item **tmp_items = self->items;
            self->items = nd->items;
            nd->items = tmp_items;

            /* nd now holds old arrays; dispose it (frees old buckets only) */
            nd->count = 0; /* avoid double-free of moved items */
            free(nd->items);
            free(nd);
            return DICT_OK;
        }
    }
}

/* Grow/shrink helpers based on load factor (%). */
static int dict_resize_up(dict *self) {
    return dict_resize(self, self->base_size * 2u);
}

static int dict_resize_down(dict *self) {
    return dict_resize(self, (self->base_size > DICT_INIT_BASE_SIZE)
                              ? (self->base_size / 2u)
                              : self->base_size);
}

/* ---- public API --------------------------------------------------------- */

dict *new_dict(void) {
    return dict_new_sized(DICT_INIT_BASE_SIZE);
}

void dict_dispose(dict *self) {
    if (!self) {
        return;
    }
    else {
        for (size_t i = 0; i < self->size; ++i) {
            item_free(self->items[i]);
        }
        free(self->items);
        free(self);
        return;
    }
}

size_t dict_size(const dict *self) {
    if (!self) {
        return 0;
    }
    else {
        return self->count;
    }
}

bool dict_contains(const dict *self, const char *key) {
    const char *dummy = NULL;
    return dict_get(self, key, &dummy) == DICT_OK;
}

int dict_get(const dict *self, const char *key, const char **out_value) {
    if (!self || !key) {
        return DICT_ERR_NOTFOUND;
    }
    else {
        unsigned attempt = 0;
        unsigned m = (unsigned)self->size;
        unsigned idx = bucket_index(key, m, attempt);

        item *it = self->items[idx];
        while (it) {
            if (it != &DELETED_SENTINEL) {
                if (strcmp(it->key, key) == 0) {
                    if (out_value) {
                        *out_value = it->value; /* owned by dict */
                    }
                    return DICT_OK;
                }
                else {
                    /* continue probing */
                }
            }
            attempt++;
            idx = bucket_index(key, m, attempt);
            it = self->items[idx];
        }
        return DICT_ERR_NOTFOUND;
    }
}

int dict_insert(dict *self, const char *key, const char *value) {
    if (!self || !key || !value) {
        return DICT_ERR_OOM; /* treat as failure */
    }
    else {
        /* grow if load factor exceeds 70% */
        if (self->size == 0) {
            return DICT_ERR_OOM;
        }
        else {
            size_t load = self->count * 100u / self->size;
            if (load > 70u) {
                int rc = dict_resize_up(self);
                if (rc != DICT_OK) {
                    return rc;
                }
            }
        }

        item *it = item_new(key, value);
        if (!it) {
            return DICT_ERR_OOM;
        }
        else {
            int rc = dict_insert_raw(self, it);
            return rc; /* DICT_OK or OOM (unlikely here since table already allocated) */
        }
    }
}

int dict_del(dict *self, const char *key) {
    if (!self || !key || self->size == 0) {
        return DICT_ERR_NOTFOUND;
    }
    else {
        unsigned attempt = 0;
        unsigned m = (unsigned)self->size;
        unsigned idx = bucket_index(key, m, attempt);

        item *it = self->items[idx];
        while (it) {
            if (it != &DELETED_SENTINEL) {
                if (strcmp(it->key, key) == 0) {
                    item_free(it);                      /* free key/value + item */
                    self->items[idx] = &DELETED_SENTINEL; /* leave tombstone */
                    if (self->count > 0) {
                        self->count -= 1;
                    }
                    /* shrink if load factor falls below 10% */
                    size_t load = self->count * 100u / self->size;
                    if (load < 10u) {
                        (void)dict_resize_down(self);   /* ignore OOM on shrink */
                    }
                    return DICT_OK;
                }
                else {
                    /* continue probing */
                }
            }
            attempt++;
            idx = bucket_index(key, m, attempt);
            it = self->items[idx];
        }
        return DICT_ERR_NOTFOUND;
    }
}

/* ---- Debug -------------------------------------------------------------- */

void dict_debug_print(const dict *self) {
    if (!self) {
        printf("dict(NULL)\n");
        return;
    }
    else {
        printf("dict(size=%zu, buckets=%zu, load=%zu%%)\n",
               self->count,
               self->size,
               (self->size ? (self->count * 100u / self->size) : 0u));
        
        for (size_t i = 0; i < self->size; ++i) {
            item *it = self->items[i];
            if (it && it != &DELETED_SENTINEL) {
                printf("  [%zu] %s -> %s\n", i, it->key, it->value);
            }
        }
        return;
    }
}

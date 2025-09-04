#include "set.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ---- internal types ----------------------------------------------------- */

typedef struct item {
    char *key;
} item;

struct set {
    size_t  base_size;
    size_t  size;     /* number of buckets (prime) */
    size_t  count;    /* stored entries */
    item  **items;    /* buckets: NULL / &DELETED_SENTINEL / heap item */
};

/* ---- constants ---------------------------------------------------------- */

static const unsigned PRIME_1 = 151u;
static const unsigned PRIME_2 = 163u;
static const size_t   SET_INIT_BASE_SIZE = 50u;

static item DELETED_SENTINEL = { NULL };

/* ---- small helpers ------------------------------------------------------ */

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

static unsigned hash_poly(const char *s, unsigned a, unsigned m) {
    unsigned long long h = 0ull;
    const unsigned char *p = (const unsigned char*)s;
    while (*p) {
        h = (h * a + (unsigned long long)(*p)) % m;
        ++p;
    }
    return (unsigned)h;
}

static unsigned bucket_index(const char *s, unsigned m, unsigned attempt) {
    unsigned h1 = hash_poly(s, PRIME_1, m);
    unsigned h2 = hash_poly(s, PRIME_2, m);
    return (h1 + attempt * (h2 + 1u)) % m;
}

static item *item_new(const char *k) {
    item *it = (item*)malloc(sizeof(*it));
    if (!it) {
        return NULL;
    }
    else {
        it->key = dupstr(k);
        if (!it->key) {
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
        free(it);
        return;
    }
}

/* ---- table management --------------------------------------------------- */

static set *set_new_sized(size_t base_size) {
    set *s = (set*)malloc(sizeof(*s));
    if (!s) {
        return NULL;
    }
    else {
        s->base_size = base_size;
        s->size  = (size_t)next_prime((int)base_size);
        s->count = 0;
        s->items = (item**)calloc(s->size, sizeof(item*));
        if (!s->items) {
            free(s);
            return NULL;
        }
        else {
            return s;
        }
    }
}

static int set_insert_raw(set *self, item *it) {
    unsigned attempt = 0;
    unsigned m = (unsigned)self->size;
    unsigned idx = bucket_index(it->key, m, attempt);

    while (self->items[idx] && self->items[idx] != &DELETED_SENTINEL) {
        if (strcmp(self->items[idx]->key, it->key) == 0) {
            item_free(self->items[idx]);
            self->items[idx] = it;
            return SET_OK;
        }
        attempt++;
        idx = bucket_index(it->key, m, attempt);
    }
    self->items[idx] = it;
    self->count++;
    return SET_OK;
}

static int set_resize(set *self, size_t new_base_size) {
    if (new_base_size < SET_INIT_BASE_SIZE) {
        return SET_OK;
    }
    else {
        set *ns = set_new_sized(new_base_size);
        if (!ns) {
            return SET_ERR_OOM;
        }
        else {
            for (size_t i = 0; i < self->size; ++i) {
                item *it = self->items[i];
                if (it && it != &DELETED_SENTINEL) {
                    (void)set_insert_raw(ns, it);
                }
            }

            size_t tmp_sz = self->size;
            self->size = ns->size;
            ns->size = tmp_sz;

            size_t tmp_bs = self->base_size;
            self->base_size = ns->base_size;
            ns->base_size = tmp_bs;

            item **tmp_items = self->items;
            self->items = ns->items;
            ns->items = tmp_items;

            ns->count = 0;
            free(ns->items);
            free(ns);
            return SET_OK;
        }
    }
}

static int set_resize_up(set *self) {
    return set_resize(self, self->base_size * 2u);
}

static int set_resize_down(set *self) {
    size_t nb = (self->base_size > SET_INIT_BASE_SIZE) ? (self->base_size / 2u)
                                                       : self->base_size;
    return set_resize(self, nb);
}

/* ---- public API --------------------------------------------------------- */

set *set_new(void) {
    return set_new_sized(SET_INIT_BASE_SIZE);
}

void set_dispose(set *self) {
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

size_t set_size(const set *self) {
    if (!self) {
        return 0;
    }
    else {
        return self->count;
    }
}

bool set_contains(const set *self, const char *key) {
    if (!self || !key) {
        return false;
    }
    else {
        unsigned attempt = 0;
        unsigned m = (unsigned)self->size;
        unsigned idx = bucket_index(key, m, attempt);

        item *it = self->items[idx];
        while (it) {
            if (it != &DELETED_SENTINEL) {
                if (strcmp(it->key, key) == 0) {
                    return true;
                }
            }
            attempt++;
            idx = bucket_index(key, m, attempt);
            it = self->items[idx];
        }
        return false;
    }
}

int set_insert(set *self, const char *key) {
    if (!self || !key) {
        return SET_ERR_OOM;
    }
    else {
        if (self->size == 0) {
            return SET_ERR_OOM;
        }
        else {
            size_t load = self->count * 100u / self->size;
            if (load > 70u) {
                int rc = set_resize_up(self);
                if (rc != SET_OK) {
                    return rc;
                }
            }
        }

        if (set_contains(self, key)) {
            return SET_ERR_DUPLICATE;
        }
        else {
            item *it = item_new(key);
            if (!it) {
                return SET_ERR_OOM;
            }
            else {
                return set_insert_raw(self, it);
            }
        }
    }
}

int set_remove(set *self, const char *key) {
    if (!self || !key || self->size == 0) {
        return SET_ERR_NOTFOUND;
    }
    else {
        unsigned attempt = 0;
        unsigned m = (unsigned)self->size;
        unsigned idx = bucket_index(key, m, attempt);

        item *it = self->items[idx];
        while (it) {
            if (it != &DELETED_SENTINEL) {
                if (strcmp(it->key, key) == 0) {
                    item_free(it);
                    self->items[idx] = &DELETED_SENTINEL;
                    if (self->count > 0) {
                        self->count -= 1;
                    }
                    size_t load = self->count * 100u / self->size;
                    if (load < 10u) {
                        (void)set_resize_down(self);
                    }
                    return SET_OK;
                }
            }
            attempt++;
            idx = bucket_index(key, m, attempt);
            it = self->items[idx];
        }
        return SET_ERR_NOTFOUND;
    }
}

/* ---- debug -------------------------------------------------------------- */

void set_debug_print(const set *self) {
    if (!self) {
        printf("set(NULL)\n");
        return;
    }
    else {
        printf("set(size=%zu, buckets=%zu, load=%zu%%)\n",
               self->count,
               self->size,
               (self->size ? (self->count * 100u / self->size) : 0u));
        return;
    }
}

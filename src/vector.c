#include "vector.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/* Internal layout:
   - Backed by a ring buffer (head..tail) for O(1) front/back operations.
   - Growth linearizes elements into a larger contiguous buffer.
   - Middle insert/remove first linearizes to enable memmove in logical order. */
struct vector {
    size_t size;
    size_t capacity;
    size_t head;   /* index of first element when size > 0 */
    size_t tail;   /* index of last  element when size > 0 */
    int   *data;
};

/* ---- internal helpers --------------------------------------------------- */

/* Ensures capacity >= want; keeps logical order and resets head to 0.
   Uses the old capacity for modulo arithmetic during copying. */
static int vector_ensure_capacity(vector *v, size_t want) {
    if (v->capacity >= want) {
        return VECTOR_OK;
    }
    else {
        size_t old_cap = (v->capacity ? v->capacity : 2);
        size_t new_cap = old_cap;
        while (new_cap < want) {
            if (new_cap > (SIZE_MAX / 2)) {
                return VECTOR_ERR_OOM; /* overflow protection */
            }
            else {
                new_cap <<= 1;
            }
        }

        int *new_data = (int*)malloc(new_cap * sizeof(int));
        if (!new_data) {
            return VECTOR_ERR_OOM;
        }
        else {
            if (v->capacity == 0) {
                /* nothing to copy when size==0 as well */
            }
            else {
                for (size_t i = 0; i < v->size; ++i) {
                    size_t from = (v->head + i) % v->capacity;
                    new_data[i] = v->data[from];
                }
            }

            free(v->data);
            v->data = new_data;
            v->capacity = new_cap;
            v->head = 0;
            v->tail = (v->size ? (v->size - 1) : 0);
            return VECTOR_OK;
        }
    }
}

/* Reorders the ring buffer so that logical index 0 is at physical index 0.
   Capacity is preserved. */
static int vector_linearize(vector *v) {
    if (v->size == 0 || v->head == 0) {
        return VECTOR_OK;
    }
    else {
        int *buf = (int*)malloc(v->capacity * sizeof(int));
        if (!buf) {
            return VECTOR_ERR_OOM;
        }
        else {
            for (size_t i = 0; i < v->size; ++i) {
                size_t from = (v->head + i) % v->capacity;
                buf[i] = v->data[from];
            }
            free(v->data);
            v->data = buf;
            v->head = 0;
            v->tail = (v->size ? (v->size - 1) : 0);
            return VECTOR_OK;
        }
    }
}

/* ---- public API --------------------------------------------------------- */

vector *vector_new(void) {
    vector *v = (vector*)calloc(1, sizeof(*v));
    if (!v) {
        return NULL;
    }
    else {
        v->capacity = 2;
        v->data = (int*)malloc(v->capacity * sizeof(int));
        if (!v->data) {
            free(v);
            return NULL;
        }
        else {
            /* size=0, head=0, tail=0 come from calloc */
            return v;
        }
    }
}

void vector_free(vector *v) {
    if (!v) {
        return;
    }
    else {
        free(v->data);
        free(v);
        return;
    }
}

int vector_reserve(vector *v, size_t min_capacity) {
    if (!v) {
        return VECTOR_ERR_RANGE;
    }
    else {
        return vector_ensure_capacity(v, min_capacity);
    }
}

size_t vector_capacity(const vector *v) {
    if (!v) {
        return 0;
    }
    else {
        return v->capacity;
    }
}

size_t vector_size(const vector *v) {
    if (!v) {
        return 0;
    }
    else {
        return v->size;
    }
}

bool vector_is_empty(const vector *v) {
    if (!v) {
        return true;
    }
    else {
        return v->size == 0;
    }
}

int vector_peek_front(const vector *v, int *out) {
    if (!v || v->size == 0) {
        return VECTOR_ERR_EMPTY;
    }
    else {
        if (out) {
            *out = v->data[v->head];
        }
        return VECTOR_OK;
    }
}

int vector_peek_back(const vector *v, int *out) {
    if (!v || v->size == 0) {
        return VECTOR_ERR_EMPTY;
    }
    else {
        if (out) {
            *out = v->data[v->tail];
        }
        return VECTOR_OK;
    }
}

int vector_get(const vector *v, size_t index, int *out) {
    if (!v) {
        return VECTOR_ERR_RANGE;
    }
    else if (index >= v->size) {
        return VECTOR_ERR_RANGE;
    }
    else {
        size_t pos = (v->head + index) % v->capacity;
        if (out) {
            *out = v->data[pos];
        }
        return VECTOR_OK;
    }
}

int vector_set(vector *v, size_t index, int value) {
    if (!v) {
        return VECTOR_ERR_RANGE;
    }
    else if (index >= v->size) {
        return VECTOR_ERR_RANGE;
    }
    else {
        size_t pos = (v->head + index) % v->capacity;
        v->data[pos] = value;
        return VECTOR_OK;
    }
}

int vector_push_back(vector *v, int value) {
    if (!v) {
        return VECTOR_ERR_RANGE;
    }
    else {
        if (v->size == v->capacity) {
            int rc = vector_ensure_capacity(v, v->size + 1);
            if (rc != VECTOR_OK) {
                return rc;
            }
        }

        if (v->size == 0) {
            v->head = 0;
            v->tail = 0;
        }
        else {
            v->tail = (v->tail + 1) % v->capacity;
        }

        v->data[v->tail] = value;
        v->size++;
        return VECTOR_OK;
    }
}

int vector_push_front(vector *v, int value) {
    if (!v) {
        return VECTOR_ERR_RANGE;
    }
    else {
        if (v->size == v->capacity) {
            int rc = vector_ensure_capacity(v, v->size + 1);
            if (rc != VECTOR_OK) {
                return rc;
            }
        }

        if (v->size == 0) {
            v->head = 0;
            v->tail = 0;
        }
        else {
            v->head = (v->head == 0) ? (v->capacity - 1) : (v->head - 1);
        }

        v->data[v->head] = value;
        v->size++;
        return VECTOR_OK;
    }
}

int vector_pop_front(vector *v, int *out) {
    if (!v || v->size == 0) {
        return VECTOR_ERR_EMPTY;
    }
    else {
        if (out) {
            *out = v->data[v->head];
        }
        v->head = (v->head + 1) % v->capacity;
        v->size--;
        if (v->size == 0) {
            v->head = 0;
            v->tail = 0;
        }
        else {
            /* tail remains valid */
        }
        return VECTOR_OK;
    }
}

int vector_pop_back(vector *v, int *out) {
    if (!v || v->size == 0) {
        return VECTOR_ERR_EMPTY;
    }
    else {
        if (out) {
            *out = v->data[v->tail];
        }
        v->tail = (v->tail == 0) ? (v->capacity - 1) : (v->tail - 1);
        v->size--;
        if (v->size == 0) {
            v->head = 0;
            v->tail = 0;
        }
        else {
            /* head remains valid */
        }
        return VECTOR_OK;
    }
}

int vector_insert(vector *v, size_t index, int value) {
    if (!v) {
        return VECTOR_ERR_RANGE;
    }
    else if (index > v->size) {
        return VECTOR_ERR_RANGE;
    }
    else if (index == v->size) {
        return vector_push_back(v, value);
    }
    else if (index == 0) {
        return vector_push_front(v, value);
    }
    else {
        int rc;
        if (v->size == v->capacity) {
            rc = vector_ensure_capacity(v, v->size + 1);
            if (rc != VECTOR_OK) {
                return rc;
            }
        }
        rc = vector_linearize(v);
        if (rc != VECTOR_OK) {
            return rc;
        }

        /* head==0, tail==size-1, data[0..size-1] valid and contiguous */
        size_t pos = index;
        memmove(&v->data[pos + 1], &v->data[pos], (v->size - pos) * sizeof(int));
        v->data[pos] = value;
        v->size++;
        v->tail = v->size - 1;
        return VECTOR_OK;
    }
}

int vector_remove(vector *v, size_t index, int *out) {
    if (!v) {
        return VECTOR_ERR_RANGE;
    }
    else if (v->size == 0) {
        return VECTOR_ERR_EMPTY;
    }
    else if (index >= v->size) {
        return VECTOR_ERR_RANGE;
    }
    else if (index == 0) {
        return vector_pop_front(v, out);
    }
    else if (index + 1 == v->size) {
        return vector_pop_back(v, out);
    }
    else {
        int rc = vector_linearize(v);
        if (rc != VECTOR_OK) {
            return rc;
        }

        if (out) {
            *out = v->data[index];
        }
        memmove(&v->data[index], &v->data[index + 1], (v->size - index - 1) * sizeof(int));
        v->size--;
        v->tail = v->size - 1;
        return VECTOR_OK;
    }
}

int vector_search(const vector *v, int value, size_t *out_index) {
    if (!v || v->size == 0) {
        return VECTOR_ERR_NOTFOUND;
    }
    else if (v->head <= v->tail) {
        for (size_t i = v->head; i <= v->tail; ++i) {
            if (v->data[i] == value) {
                if (out_index) {
                    *out_index = i - v->head;
                }
                return VECTOR_OK;
            }
            else {
                /* continue scanning */
            }
        }
        return VECTOR_ERR_NOTFOUND;
    }
    else {
        for (size_t i = 0; i < v->size; ++i) {
            size_t pos = (v->head + i) % v->capacity;
            if (v->data[pos] == value) {
                if (out_index) {
                    *out_index = i;
                }
                return VECTOR_OK;
            }
            else {
                /* continue scanning */
            }
        }
        return VECTOR_ERR_NOTFOUND;
    }
}

/* ---- Debug -------------------------------------------------------------- */

void vector_print(const vector *v) {
    if (!v || v->size == 0) {
        printf("vector([])\n");
        return;
    }
    else {
        printf("vector(size=%zu, cap=%zu, head=%zu, tail=%zu) [",
               v->size, v->capacity, v->head, v->tail);
        for (size_t i = 0; i < v->size; ++i) {
            size_t pos = (v->head + i) % v->capacity;
            printf("%d%s", v->data[pos], (i + 1 == v->size) ? "" : ", ");
        }
        printf("]\n");
        return;
    }
}

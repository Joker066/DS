#include "heap.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>   

/* Internal element stored in the heap array. */
typedef struct {
    int value;
    int weight;  /* priority; smaller is “better” */
} HeapItem;

/* Dynamic-array backed binary heap (0-based indexing). */
struct heap {
    HeapItem *arr;
    size_t    size;      /* number of elements in use */
    size_t    capacity;  /* allocated length of arr */
};

/* ---- internal helpers --------------------------------------------------- */

static void heap_swap(HeapItem *a, HeapItem *b) {
    HeapItem tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Ensure capacity for at least `need` elements. */
static int heap_ensure_capacity(heap *h, size_t need) {
    if (h->capacity >= need) {
        return HEAP_OK;
    }
    else {
        size_t new_cap = (h->capacity ? h->capacity : 16u);
        while (new_cap < need) {
            if (new_cap > SIZE_MAX / 2u) {
                return HEAP_ERR_OOM; /* would overflow size_t */
            }
            else {
                new_cap <<= 1;
            }
        }
        if (new_cap > SIZE_MAX / sizeof(HeapItem)) {
            return HEAP_ERR_OOM;      /* bytes would overflow size_t */
        }
        else {
            HeapItem *p = (HeapItem*)realloc(h->arr, new_cap * sizeof(HeapItem));
            if (!p) {
                return HEAP_ERR_OOM;
            }
            else {
                h->arr = p;
                h->capacity = new_cap;
                return HEAP_OK;
            }
        }
    }
}

/* Sift the element at index `i` up until heap property holds. */
static void sift_up(HeapItem *a, size_t i) {
    while (i > 0) {
        size_t p = (i - 1u) / 2u;
        if (a[p].weight <= a[i].weight) {
            break;
        }
        else {
            heap_swap(&a[p], &a[i]);
            i = p;
        }
    }
}

/* Sift the element at index `i` down until heap property holds. */
static void sift_down(HeapItem *a, size_t n, size_t i) {
    for (;;) {
        size_t l = 2u * i + 1u;
        if (l >= n) {
            break;
        }
        else {
            size_t r = l + 1u;
            size_t m = l; /* index of smaller child */
            if (r < n && a[r].weight < a[l].weight) {
                m = r;
            }
            if (a[i].weight <= a[m].weight) {
                break;
            }
            else {
                heap_swap(&a[i], &a[m]);
                i = m;
            }
        }
    }
}

/* ---- public API --------------------------------------------------------- */

heap *heap_new(void) {
    heap *h = (heap*)calloc(1, sizeof(*h));
    if (!h) {
        return NULL;
    }
    else {
        /* lazily allocate on first push */
        return h;
    }
}

void heap_free(heap *h) {
    if (!h) {
        return;
    }
    else {
        free(h->arr);
        free(h);
        return;
    }
}

void heap_clear(heap *h) {
    if (!h) {
        return;
    }
    else {
        h->size = 0;
        /* keep capacity and buffer for reuse */
        return;
    }
}

size_t heap_size(const heap *h) {
    if (!h) {
        return 0;
    }
    else {
        return h->size;
    }
}

bool heap_is_empty(const heap *h) {
    if (!h) {
        return true;
    }
    else {
        return h->size == 0;
    }
}

int heap_push(heap *h, int value, int weight) {
    if (!h) {
        return HEAP_ERR_RANGE;
    }
    else {
        int rc = heap_ensure_capacity(h, h->size + 1u);
        if (rc != HEAP_OK) {
            return rc;
        }
        else {
            size_t i = h->size++;
            h->arr[i].value  = value;
            h->arr[i].weight = weight;
            sift_up(h->arr, i);
            return HEAP_OK;
        }
    }
}

int heap_peek_min(const heap *h, int *out_value, int *out_weight) {
    if (!h || h->size == 0) {
        return HEAP_ERR_EMPTY;
    }
    else {
        if (out_value) {
            *out_value = h->arr[0].value;
        }
        if (out_weight) {
            *out_weight = h->arr[0].weight;
        }
        return HEAP_OK;
    }
}

int heap_pop_min(heap *h, int *out_value, int *out_weight) {
    if (!h || h->size == 0) {
        return HEAP_ERR_EMPTY;
    }
    else {
        if (out_value) {
            *out_value = h->arr[0].value;
        }
        if (out_weight) {
            *out_weight = h->arr[0].weight;
        }

        /* Move last to root, shrink, then sift down. */
        h->arr[0] = h->arr[h->size - 1u];
        h->size--;

        if (h->size > 0) {
            sift_down(h->arr, h->size, 0u);
        }
        else {
            /* heap became empty */
        }
        return HEAP_OK;
    }
}

/* ---- Debug -------------------------------------------------------------- */

void heap_debug_print(const heap *h) {
    if (!h) {
        printf("heap(NULL)\n");
        return;
    }
    else {
        printf("heap(size=%zu, cap=%zu)\n", h->size, h->capacity);
        for (size_t i = 0; i < h->size; ++i) {
            printf("  [%zu] v=%d w=%d\n", i, h->arr[i].value, h->arr[i].weight);
        }
        return;
    }
}

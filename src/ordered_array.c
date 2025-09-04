#include "ordered_array.h"

#include <stdlib.h>
#include <stdio.h>   
#include <string.h>  

/* Internal representation: dynamic buffer kept in ascending order. */
struct ordered_array {
    int    *data;
    size_t  size;
    size_t  capacity;
};

/* ---- internal helpers --------------------------------------------------- */

static size_t lower_bound(const int *a, size_t n, int x) {
    size_t lo = 0, hi = n;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (a[mid] < x) {
            lo = mid + 1;
        }
        else {
            hi = mid;
        }
    }
    return lo;
}

static size_t binary_search_eq(const int *a, size_t n, int x) {
    size_t lo = 0, hi = n;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (a[mid] < x) {
            lo = mid + 1;
        }
        else if (a[mid] > x) {
            hi = mid;
        }
        else {
            return mid;
        }
    }
    return (size_t)-1;
}

static int ensure_capacity(oarr *oa, size_t need) {
    if (oa->capacity >= need) {
        return OARR_OK;
    }
    else {
        size_t new_cap = (oa->capacity ? oa->capacity : 16u);
        while (new_cap < need) {
            if (new_cap > ((size_t)-1) / 2u) {
                return OARR_ERR_OOM;  /* would overflow */
            }
            else {
                new_cap <<= 1;
            }
        }

        int *p = (int*)realloc(oa->data, new_cap * sizeof(int));
        if (!p) {
            return OARR_ERR_OOM;
        }
        else {
            oa->data = p;
            oa->capacity = new_cap;
            return OARR_OK;
        }
    }
}

/* ---- public API --------------------------------------------------------- */

oarr *oarr_new(void) {
    return oarr_new_with_capacity(16u);
}

oarr *oarr_new_with_capacity(size_t capacity) {
    oarr *oa = (oarr*)calloc(1, sizeof(*oa));
    if (!oa) {
        return NULL;
    }
    else {
        if (capacity > 0) {
            oa->data = (int*)malloc(capacity * sizeof(int));
            if (!oa->data) {
                free(oa);
                return NULL;
            }
            else {
                oa->capacity = capacity;
            }
        }
        /* else: zero-capacity; will grow on first insert */
        oa->size = 0;
        return oa;
    }
}

oarr *oarr_new_from(const int *arr, size_t len) {
    oarr *oa = oarr_new_with_capacity(len > 0 ? len : 16u);
    if (!oa) {
        return NULL;
    }
    else {
        for (size_t i = 0; i < len; ++i) {
            if (oarr_insert(oa, arr[i]) != OARR_OK) {
                oarr_free(oa);
                return NULL;
            }
        }
        return oa;
    }
}

void oarr_free(oarr *oa) {
    if (!oa) {
        return;
    }
    else {
        free(oa->data);
        free(oa);
        return;
    }
}

int oarr_reserve(oarr *oa, size_t min_capacity) {
    if (!oa) {
        return OARR_ERR_RANGE;
    }
    else {
        return ensure_capacity(oa, min_capacity);
    }
}

size_t oarr_capacity(const oarr *oa) {
    if (!oa) {
        return 0;
    }
    else {
        return oa->capacity;
    }
}

size_t oarr_size(const oarr *oa) {
    if (!oa) {
        return 0;
    }
    else {
        return oa->size;
    }
}

bool oarr_is_empty(const oarr *oa) {
    if (!oa) {
        return true;
    }
    else {
        return oa->size == 0;
    }
}

int oarr_get(const oarr *oa, size_t index, int *out) {
    if (!oa || index >= oa->size) {
        return OARR_ERR_RANGE;
    }
    else {
        if (out) {
            *out = oa->data[index];
        }
        return OARR_OK;
    }
}

int oarr_search(const oarr *oa, int value, size_t *out_index) {
    if (!oa) {
        return OARR_ERR_RANGE;
    }
    else {
        size_t idx = binary_search_eq(oa->data, oa->size, value);
        if (idx == (size_t)-1) {
            return OARR_ERR_NOTFOUND;
        }
        else {
            if (out_index) {
                *out_index = idx;
            }
            return OARR_OK;
        }
    }
}

int oarr_insert(oarr *oa, int value) {
    if (!oa) {
        return OARR_ERR_RANGE;
    }
    else {
        int rc = ensure_capacity(oa, oa->size + 1u);
        if (rc != OARR_OK) {
            return rc;
        }
        else {
            size_t pos = lower_bound(oa->data, oa->size, value);
            size_t tail = oa->size - pos;
            if (tail > 0) {
                memmove(&oa->data[pos + 1], &oa->data[pos], tail * sizeof(int));
            }
            oa->data[pos] = value;
            oa->size += 1;
            return OARR_OK;
        }
    }
}

int oarr_update(oarr *oa, size_t index, int value) {
    if (!oa || index >= oa->size) {
        return OARR_ERR_RANGE;
    }
    else {
        /* Remove old element, then insert new value to keep order. */
        int old = oa->data[index];
        (void)old; /* kept for clarity; not used after remove */
        size_t tail = oa->size - index - 1u;
        if (tail > 0) {
            memmove(&oa->data[index], &oa->data[index + 1], tail * sizeof(int));
        }
        oa->size -= 1;
        return oarr_insert(oa, value);
    }
}

int oarr_remove(oarr *oa, size_t index, int *out_value) {
    if (!oa || index >= oa->size) {
        return OARR_ERR_RANGE;
    }
    else {
        if (out_value) {
            *out_value = oa->data[index];
        }
        size_t tail = oa->size - index - 1u;
        if (tail > 0) {
            memmove(&oa->data[index], &oa->data[index + 1], tail * sizeof(int));
        }
        oa->size -= 1;
        return OARR_OK;
    }
}

/* ---- Debug -------------------------------------------------------------- */

void oarr_print(const oarr *oa) {
    if (!oa) {
        printf("\n");
        return;
    }
    else {
        for (size_t i = 0; i < oa->size; ++i) {
            printf("%d%s", oa->data[i], (i + 1 == oa->size) ? "" : " ");
        }
        printf("\n");
        return;
    }
}

#include "xor_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/* ---- internal types ----------------------------------------------------- */

typedef struct xl_node {
    int               v;
    struct xl_node   *link;   /* XOR(prev, next) */
} xl_node;

struct xor_list {
    xl_node *head;
    xl_node *tail;
    size_t   size;
};

/* ---- small helpers ------------------------------------------------------ */

static inline xl_node *pxor(xl_node *a, xl_node *b) {
    return (xl_node*)((uintptr_t)a ^ (uintptr_t)b);
}

static xl_node *node_new(int v) {
    xl_node *n = (xl_node*)malloc(sizeof(*n));
    if (!n) {
        return NULL;
    }
    else {
        n->v = v;
        n->link = NULL;
        return n;
    }
}

/* ---- public API --------------------------------------------------------- */

xor_list *xor_list_new(void) {
    xor_list *xl = (xor_list*)calloc(1, sizeof(*xl));
    if (!xl) {
        return NULL;
    }
    else {
        return xl;
    }
}

void xor_list_free(xor_list *xl) {
    if (!xl) {
        return;
    }
    else {
        xor_list_clear(xl);
        free(xl);
        return;
    }
}

void xor_list_clear(xor_list *xl) {
    if (!xl) {
        return;
    }
    else {
        xl_node *prev = NULL;
        xl_node *cur  = xl->head;
        while (cur) {
            xl_node *next = pxor(prev, cur->link);
            free(cur);
            prev = NULL;  /* keep chain correct while walking: next uses prev=cur */
            prev = cur;
            cur  = next;
        }
        xl->head = NULL;
        xl->tail = NULL;
        xl->size = 0;
        return;
    }
}

size_t xor_list_size(const xor_list *xl) {
    if (!xl) {
        return 0;
    }
    else {
        return xl->size;
    }
}

bool xor_list_is_empty(const xor_list *xl) {
    if (!xl) {
        return true;
    }
    else {
        return xl->size == 0;
    }
}

/* ---- push/pop front ----------------------------------------------------- */

int xor_list_push_front(xor_list *xl, int v) {
    if (!xl) {
        return XL_ERR_RANGE;
    }
    else {
        xl_node *n = node_new(v);
        if (!n) {
            return XL_ERR_OOM;
        }
        else {
            n->link = pxor(NULL, xl->head);
            if (!xl->head) {
                xl->tail = n;
            }
            else {
                xl_node *head_next = pxor(NULL, xl->head->link);
                xl->head->link = pxor(n, head_next);
            }
            xl->head = n;
            xl->size += 1;
            return XL_OK;
        }
    }
}

int xor_list_push_back(xor_list *xl, int v) {
    if (!xl) {
        return XL_ERR_RANGE;
    }
    else if (!xl->tail) {
        return xor_list_push_front(xl, v);
    }
    else {
        xl_node *n = node_new(v);
        if (!n) {
            return XL_ERR_OOM;
        }
        else {
            xl_node *tail_prev = pxor(NULL, xl->tail->link);
            xl->tail->link = pxor(tail_prev, n);
            n->link = pxor(xl->tail, NULL);
            xl->tail = n;
            xl->size += 1;
            return XL_OK;
        }
    }
}

int xor_list_pop_front(xor_list *xl, int *out) {
    if (!xl || xl->size == 0) {
        return XL_ERR_EMPTY;
    }
    else if (xl->size == 1) {
        if (out) {
            *out = xl->head->v;
        }
        free(xl->head);
        xl->head = NULL;
        xl->tail = NULL;
        xl->size = 0;
        return XL_OK;
    }
    else {
        xl_node *old = xl->head;
        xl_node *next = pxor(NULL, old->link);
        xl_node *next_next = pxor(old, next->link);

        if (out) {
            *out = old->v;
        }

        next->link = pxor(NULL, next_next);
        xl->head = next;
        free(old);
        xl->size -= 1;
        return XL_OK;
    }
}

int xor_list_pop_back(xor_list *xl, int *out) {
    if (!xl || xl->size == 0) {
        return XL_ERR_EMPTY;
    }
    else if (xl->size == 1) {
        return xor_list_pop_front(xl, out);
    }
    else {
        xl_node *old = xl->tail;
        xl_node *prev = pxor(NULL, old->link);
        xl_node *prev_prev = pxor(old, prev->link);

        if (out) {
            *out = old->v;
        }

        prev->link = pxor(prev_prev, NULL);
        xl->tail = prev;
        free(old);
        xl->size -= 1;
        return XL_OK;
    }
}

/* ---- reverse / splice --------------------------------------------------- */

void xor_list_reverse(xor_list *xl) {
    if (!xl) {
        return;
    }
    else {
        xl_node *tmp = xl->head;
        xl->head = xl->tail;
        xl->tail = tmp;
        return;
    }
}

int xor_list_splice_back(xor_list *dst, xor_list *src) {
    if (!dst || !src) {
        return XL_ERR_RANGE;
    }
    else if (src->size == 0) {
        return XL_OK;
    }
    else if (dst->size == 0) {
        dst->head = src->head;
        dst->tail = src->tail;
        dst->size = src->size;
        src->head = NULL;
        src->tail = NULL;
        src->size = 0;
        return XL_OK;
    }
    else {
        xl_node *a_tail = dst->tail;
        xl_node *b_head = src->head;

        xl_node *a_prev = pxor(NULL, a_tail->link);
        xl_node *b_next = pxor(NULL, b_head->link);

        a_tail->link = pxor(a_prev, b_head);
        b_head->link = pxor(a_tail, b_next);

        dst->tail = src->tail;
        dst->size += src->size;

        src->head = NULL;
        src->tail = NULL;
        src->size = 0;
        return XL_OK;
    }
}

/* ---- debug -------------------------------------------------------------- */

void xor_list_debug_print(const xor_list *xl) {
    if (!xl || xl->size == 0) {
        printf("[]\n");
        return;
    }
    else {
        printf("[");
        const xl_node *prev = NULL;
        const xl_node *cur  = xl->head;
        size_t i = 0;
        while (cur) {
            printf("%s%d", (i ? ", " : ""), cur->v);
            const xl_node *next = pxor((xl_node*)prev, (xl_node*)cur->link);
            prev = cur;
            cur  = next;
            i++;
        }
        printf("]\n");
        return;
    }
}

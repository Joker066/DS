#include "queue.h"

#include <stdlib.h>
#include <stdio.h>   /* only used by queue_debug_print */

/* Internal node (doubly linked to allow future extensions). */
typedef struct node {
    int          v;
    struct node *prev;
    struct node *next;
} node;

/* queue representation. */
struct queue {
    node  *head;
    node  *tail;
    size_t size;
};

/* ---- internal helpers --------------------------------------------------- */

static node *node_new(int v) {
    node *n = (node*)malloc(sizeof(*n));
    if (!n) {
        return NULL;
    }
    else {
        n->v = v;
        n->prev = NULL;
        n->next = NULL;
        return n;
    }
}

/* ---- public API --------------------------------------------------------- */

queue *queue_new(void) {
    queue *q = (queue*)calloc(1, sizeof(*q));
    if (!q) {
        return NULL;
    }
    else {
        /* head, tail, size are zero-initialized by calloc */
        return q;
    }
}

void queue_free(queue *q) {
    if (!q) {
        return;
    }
    else {
        queue_clear(q);
        free(q);
        return;
    }
}

void queue_clear(queue *q) {
    if (!q) {
        return;
    }
    else {
        node *cur = q->head;
        while (cur) {
            node *next = cur->next;
            free(cur);
            cur = next;
        }
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
        return;
    }
}

size_t queue_size(const queue *q) {
    if (!q) {
        return 0;
    }
    else {
        return q->size;
    }
}

bool queue_is_empty(const queue *q) {
    if (!q) {
        return true;
    }
    else {
        return q->size == 0;
    }
}

int queue_peek(const queue *q, int *out) {
    if (!q || q->size == 0) {
        return QUEUE_ERR_EMPTY;
    }
    else {
        if (out) {
            *out = q->head->v;
        }
        return QUEUE_OK;
    }
}

int queue_push(queue *q, int v) {
    if (!q) {
        return QUEUE_ERR_RANGE;
    }
    else {
        node *n = node_new(v);
        if (!n) {
            return QUEUE_ERR_OOM;
        }
        else {
            if (!q->tail) {
                /* empty queue */
                q->head = n;
                q->tail = n;
            }
            else {
                q->tail->next = n;
                n->prev = q->tail;
                q->tail = n;
            }
            q->size += 1;
            return QUEUE_OK;
        }
    }
}

int queue_pop(queue *q, int *out) {
    if (!q || q->size == 0) {
        return QUEUE_ERR_EMPTY;
    }
    else {
        node *n = q->head;
        if (out) {
            *out = n->v;
        }

        q->head = n->next;
        if (q->head) {
            q->head->prev = NULL;
        }
        else {
            /* queue became empty */
            q->tail = NULL;
        }

        free(n);
        q->size -= 1;
        return QUEUE_OK;
    }
}

/* ---- Debug -------------------------------------------------------------- */

void queue_debug_print(const queue *q) {
    if (!q) {
        printf("queue(NULL)\n");
        return;
    }
    else {
        printf("queue(size=%zu): [", q->size);
        const node *cur = q->head;
        size_t i = 0;
        while (cur) {
            printf("%s%d", (i ? ", " : ""), cur->v);
            cur = cur->next;
            i++;
        }
        printf("]\n");
        return;
    }
}

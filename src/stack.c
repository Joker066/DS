#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

/* ---- internal types ----------------------------------------------------- */

typedef struct node {
    int          v;
    struct node *prev;
} node;

struct stack {
    node  *top;
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
        return n;
    }
}

/* ---- public API --------------------------------------------------------- */

stack *stack_new(void) {
    stack *s = (stack*)calloc(1, sizeof(*s));
    if (!s) {
        return NULL;
    }
    else {
        return s;
    }
}

void stack_free(stack *s) {
    if (!s) {
        return;
    }
    else {
        stack_clear(s);
        free(s);
        return;
    }
}

void stack_clear(stack *s) {
    if (!s) {
        return;
    }
    else {
        node *cur = s->top;
        while (cur) {
            node *prev = cur->prev;
            free(cur);
            cur = prev;
        }
        s->top = NULL;
        s->size = 0;
        return;
    }
}

size_t stack_size(const stack *s) {
    if (!s) {
        return 0;
    }
    else {
        return s->size;
    }
}

bool stack_is_empty(const stack *s) {
    if (!s) {
        return true;
    }
    else {
        return s->size == 0;
    }
}

int stack_peek(const stack *s, int *out) {
    if (!s || s->size == 0) {
        return STACK_ERR_EMPTY;
    }
    else {
        if (out) {
            *out = s->top->v;
        }
        return STACK_OK;
    }
}

int stack_push(stack *s, int v) {
    if (!s) {
        return STACK_ERR_RANGE;
    }
    else {
        node *n = node_new(v);
        if (!n) {
            return STACK_ERR_OOM;
        }
        else {
            n->prev = s->top;
            s->top = n;
            s->size += 1;
            return STACK_OK;
        }
    }
}

int stack_pop(stack *s, int *out) {
    if (!s || s->size == 0) {
        return STACK_ERR_EMPTY;
    }
    else {
        node *n = s->top;
        if (out) {
            *out = n->v;
        }
        s->top = n->prev;
        free(n);
        s->size -= 1;
        return STACK_OK;
    }
}

/* ---- debug -------------------------------------------------------------- */

void stack_debug_print(const stack *s) {
    if (!s) {
        printf("stack(NULL)\n");
        return;
    }
    else {
        printf("stack(size=%zu): [top", s->size);
        const node *cur = s->top;
        size_t i = 0;
        while (cur) {
            printf("%s %d", (i ? " ->" : " ->"), cur->v);
            cur = cur->prev;
            i++;
        }
        printf(" ]\n");
        return;
    }
}

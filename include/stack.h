#ifndef DS_STACK_H
#define DS_STACK_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status codes ------------------------------------------------------- */
#define STACK_OK         0
#define STACK_ERR_EMPTY -1
#define STACK_ERR_OOM   -2
#define STACK_ERR_RANGE -3

/* Opaque handle */
typedef struct stack stack;

/* ---- Lifecycle ---------------------------------------------------------- */
stack *stack_new(void);
void   stack_free(stack *s);
void   stack_clear(stack *s);

/* ---- Queries ------------------------------------------------------------ */
size_t stack_size(const stack *s);
bool   stack_is_empty(const stack *s);

/* ---- Accessors ---------------------------------------------------------- */
int    stack_peek(const stack *s, int *out);     /* EMPTY? */

/* ---- Mutations ---------------------------------------------------------- */
int    stack_push(stack *s, int v);              /* OOM? */
int    stack_pop(stack *s, int *out);            /* EMPTY? */

/* ---- Debug -------------------------------------------------------------- */
void   stack_debug_print(const stack *s);

#ifdef __cplusplus
}
#endif
#endif /* DS_STACK_H */

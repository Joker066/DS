#ifndef DS_QUEUE_H
#define DS_QUEUE_H

/* Public interface for a FIFO queue of ints using a linked list.
   Implementation details are hidden; operations return status codes. */

#include <stddef.h>  /* size_t */
#include <stdbool.h> /* bool */

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status codes ------------------------------------------------------- */
#define QUEUE_OK         0
#define QUEUE_ERR_EMPTY -1
#define QUEUE_ERR_OOM   -2
#define QUEUE_ERR_RANGE -3

/* Opaque handle */
typedef struct queue queue;

/* ---- Lifecycle ---------------------------------------------------------- */
queue *queue_new(void);        /* Returns NULL on allocation failure */
void    queue_free(queue *q);
void    queue_clear(queue *q); /* Removes all elements, queue remains usable */

/* ---- Queries ------------------------------------------------------------ */
size_t  queue_size(const queue *q);
bool    queue_is_empty(const queue *q);

/* ---- Accessors ---------------------------------------------------------- */
/* Peeks without removing; writes to *out on success. */
int     queue_peek(const queue *q, int *out);   /* EMPTY? */

/* ---- Mutations ---------------------------------------------------------- */
int     queue_push(queue *q, int v);            /* OK or OOM */
int     queue_pop(queue *q, int *out);          /* OK or EMPTY */

/* ---- Debug -------------------------------------------------------------- */
void    queue_debug_print(const queue *q);      /* optional; prints a short summary */

#ifdef __cplusplus
}
#endif
#endif /* DS_QUEUE_H */

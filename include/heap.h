#ifndef DS_HEAP_H
#define DS_HEAP_H

/* Public interface for a min-heap of (value, weight) pairs.
   The heap orders by ascending weight (smaller weight = higher priority).
   Implementation details are hidden; operations return explicit status codes. */

#include <stddef.h>  /* size_t */
#include <stdbool.h> /* bool */

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status codes ------------------------------------------------------- */
#define HEAP_OK          0
#define HEAP_ERR_EMPTY  -1
#define HEAP_ERR_OOM    -2
#define HEAP_ERR_RANGE  -3

/* Opaque handle */
typedef struct heap heap;

/* ---- Lifecycle ---------------------------------------------------------- */
heap  *heap_new(void);                 /* NULL on allocation failure */
void   heap_free(heap *h);
void   heap_clear(heap *h);            /* removes all elements, keeps capacity */

/* ---- Queries ------------------------------------------------------------ */
size_t heap_size(const heap *h);
bool   heap_is_empty(const heap *h);

/* ---- Accessors ---------------------------------------------------------- */
/* Writes the smallest (value, weight) into out params without removing it. */
int    heap_peek_min(const heap *h, int *out_value, int *out_weight); /* EMPTY? */

/* ---- Mutations ---------------------------------------------------------- */
int    heap_push(heap *h, int value, int weight);                     /* OOM? */
int    heap_pop_min(heap *h, int *out_value, int *out_weight);        /* EMPTY? */

/* ---- Debug -------------------------------------------------------------- */
void   heap_debug_print(const heap *h);  /* optional helper; prints a short summary */

#ifdef __cplusplus
}
#endif
#endif /* DS_HEAP_H */

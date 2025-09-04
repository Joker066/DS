#ifndef DS_VECTOR_H
#define DS_VECTOR_H

/* Public interface for a ring-buffer-backed integer vector.
   The type is opaque; internal layout is hidden in the .c file.
   All operations return explicit status codes; diagnostic printing is avoided. */

#include <stddef.h>  /* size_t */
#include <stdbool.h> /* bool */

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status codes ------------------------------------------------------- */
#define VECTOR_OK            0
#define VECTOR_ERR_EMPTY    -1
#define VECTOR_ERR_OOM      -2
#define VECTOR_ERR_RANGE    -3
#define VECTOR_ERR_NOTFOUND -4

/* Opaque handle */
typedef struct vector vector;

/* ---- Lifecycle ---------------------------------------------------------- */
vector *vector_new(void);            /* Returns NULL on allocation failure */
void    vector_free(vector *v);

/* ---- Capacity control --------------------------------------------------- */
/* Grows capacity to at least min_capacity; never shrinks implicitly. */
int     vector_reserve(vector *v, size_t min_capacity); /* OK or OOM */
size_t  vector_capacity(const vector *v);

/* ---- Queries ------------------------------------------------------------ */
size_t  vector_size(const vector *v);
bool    vector_is_empty(const vector *v);

/* Value results are written via out-parameters; return status indicates success. */
int     vector_peek_front(const vector *v, int *out);           /* EMPTY on failure */
int     vector_peek_back (const vector *v, int *out);           /* EMPTY on failure */
int     vector_get       (const vector *v, size_t index, int *out); /* RANGE on OOB */

/* ---- Mutations ---------------------------------------------------------- */
/* Each returns VECTOR_OK or an error (EMPTY / RANGE / OOM). */
int     vector_set       (vector *v, size_t index, int value);
int     vector_push_front(vector *v, int value);
int     vector_push_back (vector *v, int value);
int     vector_pop_front (vector *v, int *out);
int     vector_pop_back  (vector *v, int *out);
int     vector_insert    (vector *v, size_t index, int value);  /* insert at [0..size] */
int     vector_remove    (vector *v, size_t index, int *out);

/* ---- Search ------------------------------------------------------------- */
/* On success writes logical index into *out_index and returns VECTOR_OK;
   returns VECTOR_ERR_NOTFOUND when the value does not occur. */
int     vector_search(const vector *v, int value, size_t *out_index);

/* ---- Debug -------------------------------------------------------------- */
/* Convenience helper for ad-hoc inspection; not used for error reporting. */
void    vector_print(const vector *v);

#ifdef __cplusplus
}
#endif
#endif /* DS_VECTOR_H */

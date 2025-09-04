#ifndef DS_ORDERED_ARRAY_H
#define DS_ORDERED_ARRAY_H

/* Public interface for a dynamically-resizing ordered array of ints.
   Elements are kept sorted in ascending order. No diagnostic printing. */

#include <stddef.h>  /* size_t */
#include <stdbool.h> /* bool */

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status codes ------------------------------------------------------- */
#define OARR_OK            0
#define OARR_ERR_EMPTY    -1
#define OARR_ERR_OOM      -2
#define OARR_ERR_RANGE    -3
#define OARR_ERR_NOTFOUND -4

/* Opaque handle */
typedef struct ordered_array oarr;

/* ---- Lifecycle ---------------------------------------------------------- */
oarr  *oarr_new(void);                             /* default capacity */
oarr  *oarr_new_with_capacity(size_t capacity);    /* returns NULL on OOM */
oarr  *oarr_new_from(const int *arr, size_t len);  /* builds sorted copy; NULL on OOM */
void   oarr_free(oarr *oa);

/* ---- Capacity control --------------------------------------------------- */
int    oarr_reserve(oarr *oa, size_t min_capacity); /* grows only; OK/OOM */
size_t oarr_capacity(const oarr *oa);

/* ---- Queries ------------------------------------------------------------ */
size_t oarr_size(const oarr *oa);
bool   oarr_is_empty(const oarr *oa);

/* ---- Access ------------------------------------------------------------- */
int    oarr_get(const oarr *oa, size_t index, int *out);     /* RANGE? */

/* ---- Search ------------------------------------------------------------- */
/* If found, writes index to *out_index and returns OARR_OK; else OARR_ERR_NOTFOUND. */
int    oarr_search(const oarr *oa, int value, size_t *out_index);

/* ---- Mutations ---------------------------------------------------------- */
int    oarr_insert(oarr *oa, int value);                     /* OK/OOM */
int    oarr_update(oarr *oa, size_t index, int value);       /* RANGE?/OOM */
int    oarr_remove(oarr *oa, size_t index, int *out_value);  /* RANGE? */

/* ---- Debug -------------------------------------------------------------- */
void   oarr_print(const oarr *oa); /* prints ascending list with '\n' */

#ifdef __cplusplus
}
#endif
#endif /* DS_ORDERED_ARRAY_H */

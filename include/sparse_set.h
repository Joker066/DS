#ifndef DS_SPARSE_SET_H
#define DS_SPARSE_SET_H

/* Public interface for a sparse set over integer keys in [0, universe).
   Supports O(1) insert, erase, and membership queries. Elements are stored
   in a dense array; order is unspecified and may change after erases. */

#include <stddef.h>  /* size_t */
#include <stdbool.h> /* bool */

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status codes ------------------------------------------------------- */
#define SSET_OK            0
#define SSET_ERR_EMPTY    -1  /* reserved */
#define SSET_ERR_OOM      -2
#define SSET_ERR_RANGE    -3  /* key out of [0, universe) */
#define SSET_ERR_NOTFOUND -4
#define SSET_ERR_DUPLICATE -7
#define SSET_ERR_FULL     -8

/* Opaque handle */
typedef struct sparse_set sset;

/* ---- Lifecycle ---------------------------------------------------------- */
/* Creates a set with key universe [0, universe) and capacity <= capacity.
   Returns NULL on allocation failure or invalid parameters. */
sset   *sset_new(size_t universe, size_t capacity);
void    sset_free(sset *s);
void    sset_clear(sset *s);                /* removes all elements, keeps buffers */

/* ---- Queries ------------------------------------------------------------ */
size_t  sset_universe(const sset *s);       /* maximum number of distinct keys representable */
size_t  sset_capacity(const sset *s);       /* maximum elements that can be stored */
size_t  sset_size(const sset *s);           /* current number of stored keys */
bool    sset_contains(const sset *s, size_t key);

/* ---- Mutations ---------------------------------------------------------- */
int     sset_insert(sset *s, size_t key);   /* OK, DUPLICATE, FULL, or RANGE */
int     sset_erase (sset *s, size_t key);   /* OK, NOTFOUND, or RANGE */

/* ---- Access ------------------------------------------------------------- */
/* Provides the key stored at index i (0 <= i < size). */
int     sset_get_at(const sset *s, size_t i, size_t *out_key); /* OK or RANGE */

/* ---- Debug -------------------------------------------------------------- */
void    sset_debug_print(const sset *s);

#ifdef __cplusplus
}
#endif
#endif /* DS_SPARSE_SET_H */

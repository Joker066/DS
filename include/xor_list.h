#ifndef DS_XOR_LIST_H
#define DS_XOR_LIST_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status codes ------------------------------------------------------- */
#define XL_OK          0
#define XL_ERR_EMPTY  -1
#define XL_ERR_OOM    -2
#define XL_ERR_RANGE  -3

/* Opaque handle */
typedef struct xor_list xor_list;

/* ---- Lifecycle ---------------------------------------------------------- */
xor_list *xor_list_new(void);
void      xor_list_free(xor_list *xl);
void      xor_list_clear(xor_list *xl);

/* ---- Queries ------------------------------------------------------------ */
size_t    xor_list_size(const xor_list *xl);
bool      xor_list_is_empty(const xor_list *xl);

/* ---- Mutations ---------------------------------------------------------- */
int       xor_list_push_front(xor_list *xl, int v);     /* OK/OOM */
int       xor_list_push_back (xor_list *xl, int v);     /* OK/OOM */
int       xor_list_pop_front (xor_list *xl, int *out);  /* OK/EMPTY */
int       xor_list_pop_back  (xor_list *xl, int *out);  /* OK/EMPTY */

/* Reverses traversal direction (swaps head/tail). */
void      xor_list_reverse(xor_list *xl);

/* Appends all nodes of `src` to the end of `dst` in O(1) and empties `src`. */
int       xor_list_splice_back(xor_list *dst, xor_list *src);  /* OK/RANGE */

/* ---- Debug -------------------------------------------------------------- */
void      xor_list_debug_print(const xor_list *xl);

#ifdef __cplusplus
}
#endif
#endif /* DS_XOR_LIST_H */

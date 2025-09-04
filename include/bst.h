#ifndef DS_BST_H
#define DS_BST_H

/* Public interface for a simple integer BST.
   Internal node layout is hidden; callers use the opaque bst handle.
   Operations return explicit status codes; no diagnostic printing is performed. */

#include <stddef.h>  /* size_t */
#include <stdbool.h> /* bool */

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status codes ------------------------------------------------------- */
#define BST_OK             0
#define BST_ERR_OOM       -2
#define BST_ERR_NOTFOUND  -4
#define BST_ERR_DUPLICATE -7

/* Opaque handle */
typedef struct binary_tree bst;

/* ---- Lifecycle ---------------------------------------------------------- */
bst   *new_bst(void);         /* Returns NULL on allocation failure */
void   bst_dispose(bst *self);

/* ---- Queries ------------------------------------------------------------ */
size_t bst_size(const bst *self);
bool   bst_search(const bst *self, int v);   /* true if present */

/* ---- Mutations ---------------------------------------------------------- */
int    bst_insert(bst *self, int v);         /* OK, DUPLICATE, or OOM */
int    bst_del(bst *self, int v);            /* OK or NOTFOUND */

/* ---- Debug -------------------------------------------------------------- */
void   bst_print_inorder(const bst *self);   /* prints ascending order with '\n' */

#ifdef __cplusplus
}
#endif
#endif /* DS_BST_H */

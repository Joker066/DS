#ifndef DS_RB_TREE_H
#define DS_RB_TREE_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status codes ------------------------------------------------------- */
#define RB_OK             0
#define RB_ERR_EMPTY     -1
#define RB_ERR_OOM       -2
#define RB_ERR_RANGE     -3
#define RB_ERR_NOTFOUND  -4
#define RB_ERR_DUPLICATE -7

/* Opaque handle */
typedef struct rb_tree rb_tree;

/* ---- Lifecycle ---------------------------------------------------------- */
rb_tree *rb_tree_new(void);
void     rb_tree_free(rb_tree *t);
void     rb_tree_clear(rb_tree *t);

/* ---- Queries ------------------------------------------------------------ */
size_t   rb_tree_size(const rb_tree *t);
bool     rb_tree_contains(const rb_tree *t, int key);

/* ---- Extremes ----------------------------------------------------------- */
int      rb_tree_min(const rb_tree *t, int *out);  /* EMPTY? */
int      rb_tree_max(const rb_tree *t, int *out);  /* EMPTY? */

/* ---- Mutations ---------------------------------------------------------- */
int      rb_tree_insert(rb_tree *t, int key);      /* OK/DUPLICATE/OOM/RANGE */
int      rb_tree_remove(rb_tree *t, int key);      /* OK/NOTFOUND */

/* ---- Debug -------------------------------------------------------------- */
void     rb_tree_debug_inorder(const rb_tree *t);

#ifdef __cplusplus
}
#endif
#endif /* DS_RB_TREE_H */

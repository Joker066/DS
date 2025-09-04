#ifndef DS_SEG_TREE_H
#define DS_SEG_TREE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SEG_OK           0
#define SEG_ERR_EMPTY   -1
#define SEG_ERR_OOM     -2
#define SEG_ERR_RANGE   -3

typedef struct seg_tree seg_tree;

/* Builds a segment tree over arr[0..len-1]. Returns NULL on OOM or len==0. */
seg_tree *seg_tree_new_from_array(const int *arr, size_t len);

/* Frees the tree (safe on NULL). */
void      seg_tree_free(seg_tree *st);

/* Number of elements covered by the tree. */
size_t    seg_tree_len(const seg_tree *st);

/* Range sum query over [l, r] inclusive. Writes result to *out_sum. */
int       seg_tree_query(const seg_tree *st, size_t l, size_t r, int *out_sum);

/* Point update: arr[index] = value; updates internal tree accordingly. */
int       seg_tree_update(seg_tree *st, size_t index, int value);

/* Debug helper: prints the leaf values in order followed by '\n'. */
void      seg_tree_debug_print(const seg_tree *st);

#ifdef __cplusplus
}
#endif
#endif /* DS_SEG_TREE_H */

#include "seg_tree.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct STNode STNode;
struct STNode {
    size_t  left;
    size_t  right;
    int     sum;
    STNode *left_c;
    STNode *right_c;
};

struct seg_tree {
    STNode *root;
    size_t  n;
};

/* ---- internal helpers ---- */

static STNode *st_node_new(size_t l, size_t r, int sum, STNode *lc, STNode *rc) {
    STNode *n = (STNode*)malloc(sizeof(*n));
    if (!n) {
        return NULL;
    }
    else {
        n->left = l;
        n->right = r;
        n->sum = sum;
        n->left_c = lc;
        n->right_c = rc;
        return n;
    }
}

static void st_node_free(STNode *x) {
    if (!x) {
        return;
    }
    else {
        st_node_free(x->left_c);
        st_node_free(x->right_c);
        free(x);
        return;
    }
}

static STNode *st_build(size_t l, size_t r, const int *arr) {
    if (l == r) {
        return st_node_new(l, r, arr[l], NULL, NULL);
    }
    else {
        size_t mid = l + (r - l) / 2u;
        STNode *lc = st_build(l, mid, arr);
        if (!lc) {
            return NULL;
        }
        else {
            STNode *rc = st_build(mid + 1u, r, arr);
            if (!rc) {
                st_node_free(lc);
                return NULL;
            }
            else {
                int sum = lc->sum + rc->sum;
                STNode *rt = st_node_new(l, r, sum, lc, rc);
                if (!rt) {
                    st_node_free(lc);
                    st_node_free(rc);
                    return NULL;
                }
                else {
                    return rt;
                }
            }
        }
    }
}

static void st_point_update(STNode *x, size_t index, int value) {
    if (x->left == x->right) {
        x->sum = value;
        return;
    }
    else {
        size_t mid = x->left + (x->right - x->left) / 2u;
        if (index <= mid) {
            st_point_update(x->left_c, index, value);
        }
        else {
            st_point_update(x->right_c, index, value);
        }
        x->sum = x->left_c->sum + x->right_c->sum;
        return;
    }
}

static int st_range_sum(const STNode *x, size_t l, size_t r) {
    if (x->right < l || x->left > r) {
        return 0;
    }
    else if (l <= x->left && x->right <= r) {
        return x->sum;
    }
    else {
        int s1 = st_range_sum(x->left_c, l, r);
        int s2 = st_range_sum(x->right_c, l, r);
        return s1 + s2;
    }
}

static void st_print_leaves(const STNode *x) {
    if (!x) {
        return;
    }
    else if (!x->left_c && !x->right_c) {
        printf("%d ", x->sum);
        return;
    }
    else {
        st_print_leaves(x->left_c);
        st_print_leaves(x->right_c);
        return;
    }
}

/* ---- public API ---- */

seg_tree *seg_tree_new_from_array(const int *arr, size_t len) {
    if (!arr || len == 0) {
        return NULL;
    }
    else {
        seg_tree *st = (seg_tree*)malloc(sizeof(*st));
        if (!st) {
            return NULL;
        }
        else {
            STNode *root = st_build(0u, len - 1u, arr);
            if (!root) {
                free(st);
                return NULL;
            }
            else {
                st->root = root;
                st->n = len;
                return st;
            }
        }
    }
}

void seg_tree_free(seg_tree *st) {
    if (!st) {
        return;
    }
    else {
        st_node_free(st->root);
        free(st);
        return;
    }
}

size_t seg_tree_len(const seg_tree *st) {
    if (!st) {
        return 0;
    }
    else {
        return st->n;
    }
}

int seg_tree_query(const seg_tree *st, size_t l, size_t r, int *out_sum) {
    if (!st || !st->root || st->n == 0) {
        return SEG_ERR_EMPTY;
    }
    else if (l > r || r >= st->n) {
        return SEG_ERR_RANGE;
    }
    else {
        int s = st_range_sum(st->root, l, r);
        if (out_sum) {
            *out_sum = s;
        }
        return SEG_OK;
    }
}

int seg_tree_update(seg_tree *st, size_t index, int value) {
    if (!st || !st->root || st->n == 0) {
        return SEG_ERR_EMPTY;
    }
    else if (index >= st->n) {
        return SEG_ERR_RANGE;
    }
    else {
        st_point_update(st->root, index, value);
        return SEG_OK;
    }
}

void seg_tree_debug_print(const seg_tree *st) {
    if (!st || !st->root) {
        printf("\n");
        return;
    }
    else {
        st_print_leaves(st->root);
        printf("\n");
        return;
    }
}

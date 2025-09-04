#include "rb_tree.h"
#include <stdlib.h>
#include <stdio.h>

/* ---- internal types ----------------------------------------------------- */

typedef enum { RB_BLACK = 0, RB_RED = 1 } rb_color;

typedef struct rb_node {
    int               key;
    rb_color          color;
    struct rb_node   *parent;
    struct rb_node   *left;
    struct rb_node   *right;
} rb_node;

struct rb_tree {
    rb_node *root;
    rb_node *nil;     /* sentinel (BLACK) */
    size_t   size;
};

/* ---- small helpers ------------------------------------------------------ */

static rb_node *node_new(rb_tree *t, int key) {
    rb_node *n = (rb_node*)malloc(sizeof(*n));
    if (!n) {
        return NULL;
    }
    else {
        n->key = key;
        n->color = RB_RED;
        n->parent = t->nil;
        n->left = t->nil;
        n->right = t->nil;
        return n;
    }
}

static rb_node *tree_min_node(const rb_tree *t, rb_node *x) {
    while (x->left != t->nil) {
        x = x->left;
    }
    return x;
}

static rb_node *tree_max_node(const rb_tree *t, rb_node *x) {
    while (x->right != t->nil) {
        x = x->right;
    }
    return x;
}

/* ---- rotations ---------------------------------------------------------- */

static void rotate_left(rb_tree *t, rb_node *x) {
    rb_node *y = x->right;
    x->right = y->left;
    if (y->left != t->nil) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == t->nil) {
        t->root = y;
    }
    else if (x == x->parent->left) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

static void rotate_right(rb_tree *t, rb_node *y) {
    rb_node *x = y->left;
    y->left = x->right;
    if (x->right != t->nil) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == t->nil) {
        t->root = x;
    }
    else if (y == y->parent->left) {
        y->parent->left = x;
    }
    else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

/* ---- insertion ---------------------------------------------------------- */

static void insert_fixup(rb_tree *t, rb_node *z) {
    while (z->parent->color == RB_RED) {
        if (z->parent == z->parent->parent->left) {
            rb_node *y = z->parent->parent->right; /* uncle */
            if (y->color == RB_RED) {
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rotate_left(t, z);
                }
                z->parent->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                rotate_right(t, z->parent->parent);
            }
        }
        else {
            rb_node *y = z->parent->parent->left; /* uncle */
            if (y->color == RB_RED) {
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotate_right(t, z);
                }
                z->parent->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                rotate_left(t, z->parent->parent);
            }
        }
    }
    t->root->color = RB_BLACK;
}

int rb_tree_insert(rb_tree *t, int key) {
    if (!t) {
        return RB_ERR_RANGE;
    }
    else {
        rb_node *y = t->nil;
        rb_node *x = t->root;

        while (x != t->nil) {
            y = x;
            if (key < x->key) {
                x = x->left;
            }
            else if (key > x->key) {
                x = x->right;
            }
            else {
                return RB_ERR_DUPLICATE;
            }
        }

        rb_node *z = node_new(t, key);
        if (!z) {
            return RB_ERR_OOM;
        }
        else {
            z->parent = y;
            if (y == t->nil) {
                t->root = z;
            }
            else if (z->key < y->key) {
                y->left = z;
            }
            else {
                y->right = z;
            }

            insert_fixup(t, z);
            t->size += 1;
            return RB_OK;
        }
    }
}

/* ---- deletion ----------------------------------------------------------- */

static void transplant(rb_tree *t, rb_node *u, rb_node *v) {
    if (u->parent == t->nil) {
        t->root = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

static void delete_fixup(rb_tree *t, rb_node *x) {
    while (x != t->root && x->color == RB_BLACK) {
        if (x == x->parent->left) {
            rb_node *w = x->parent->right;
            if (w->color == RB_RED) {
                w->color = RB_BLACK;
                x->parent->color = RB_RED;
                rotate_left(t, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == RB_BLACK && w->right->color == RB_BLACK) {
                w->color = RB_RED;
                x = x->parent;
            }
            else {
                if (w->right->color == RB_BLACK) {
                    w->left->color = RB_BLACK;
                    w->color = RB_RED;
                    rotate_right(t, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = RB_BLACK;
                w->right->color = RB_BLACK;
                rotate_left(t, x->parent);
                x = t->root;
            }
        }
        else {
            rb_node *w = x->parent->left;
            if (w->color == RB_RED) {
                w->color = RB_BLACK;
                x->parent->color = RB_RED;
                rotate_right(t, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == RB_BLACK && w->left->color == RB_BLACK) {
                w->color = RB_RED;
                x = x->parent;
            }
            else {
                if (w->left->color == RB_BLACK) {
                    w->right->color = RB_BLACK;
                    w->color = RB_RED;
                    rotate_left(t, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = RB_BLACK;
                w->left->color = RB_BLACK;
                rotate_right(t, x->parent);
                x = t->root;
            }
        }
    }
    x->color = RB_BLACK;
}

int rb_tree_remove(rb_tree *t, int key) {
    if (!t) {
        return RB_ERR_RANGE;
    }
    else {
        rb_node *z = t->root;
        while (z != t->nil) {
            if (key < z->key) {
                z = z->left;
            }
            else if (key > z->key) {
                z = z->right;
            }
            else {
                break;
            }
        }
        if (z == t->nil) {
            return RB_ERR_NOTFOUND;
        }
        else {
            rb_node *y = z;
            rb_color y_orig = y->color;
            rb_node *x = t->nil;

            if (z->left == t->nil) {
                x = z->right;
                transplant(t, z, z->right);
            }
            else if (z->right == t->nil) {
                x = z->left;
                transplant(t, z, z->left);
            }
            else {
                y = tree_min_node(t, z->right);
                y_orig = y->color;
                x = y->right;
                if (y->parent == z) {
                    x->parent = y;
                }
                else {
                    transplant(t, y, y->right);
                    y->right = z->right;
                    y->right->parent = y;
                }
                transplant(t, z, y);
                y->left = z->left;
                y->left->parent = y;
                y->color = z->color;
            }

            free(z);
            t->size -= 1;
            if (y_orig == RB_BLACK) {
                delete_fixup(t, x);
            }
            return RB_OK;
        }
    }
}

/* ---- lifecycle ---------------------------------------------------------- */

rb_tree *rb_tree_new(void) {
    rb_tree *t = (rb_tree*)malloc(sizeof(*t));
    if (!t) {
        return NULL;
    }
    else {
        rb_node *nil = (rb_node*)malloc(sizeof(*nil));
        if (!nil) {
            free(t);
            return NULL;
        }
        else {
            nil->key = 0;
            nil->color = RB_BLACK;
            nil->parent = nil;
            nil->left = nil;
            nil->right = nil;

            t->nil = nil;
            t->root = t->nil;
            t->size = 0;
            return t;
        }
    }
}

static void free_subtree(rb_tree *t, rb_node *x) {
    if (x == t->nil) {
        return;
    }
    else {
        free_subtree(t, x->left);
        free_subtree(t, x->right);
        free(x);
        return;
    }
}

void rb_tree_clear(rb_tree *t) {
    if (!t) {
        return;
    }
    else {
        free_subtree(t, t->root);
        t->root = t->nil;
        t->size = 0;
        return;
    }
}

void rb_tree_free(rb_tree *t) {
    if (!t) {
        return;
    }
    else {
        rb_tree_clear(t);
        free(t->nil);
        free(t);
        return;
    }
}

/* ---- queries / extremes ------------------------------------------------- */

size_t rb_tree_size(const rb_tree *t) {
    if (!t) {
        return 0;
    }
    else {
        return t->size;
    }
}

bool rb_tree_contains(const rb_tree *t, int key) {
    if (!t) {
        return false;
    }
    else {
        rb_node *x = t->root;
        while (x != t->nil) {
            if (key < x->key) {
                x = x->left;
            }
            else if (key > x->key) {
                x = x->right;
            }
            else {
                return true;
            }
        }
        return false;
    }
}

int rb_tree_min(const rb_tree *t, int *out) {
    if (!t || t->size == 0) {
        return RB_ERR_EMPTY;
    }
    else {
        rb_node *m = tree_min_node(t, t->root);
        if (out) {
            *out = m->key;
        }
        return RB_OK;
    }
}

int rb_tree_max(const rb_tree *t, int *out) {
    if (!t || t->size == 0) {
        return RB_ERR_EMPTY;
    }
    else {
        rb_node *m = tree_max_node(t, t->root);
        if (out) {
            *out = m->key;
        }
        return RB_OK;
    }
}

/* ---- debug -------------------------------------------------------------- */

static void inorder_walk(const rb_tree *t, const rb_node *x) {
    if (x == t->nil) {
        return;
    }
    else {
        inorder_walk(t, x->left);
        printf("%d ", x->key);
        inorder_walk(t, x->right);
        return;
    }
}

void rb_tree_debug_inorder(const rb_tree *t) {
    if (!t || t->size == 0) {
        printf("\n");
        return;
    }
    else {
        inorder_walk(t, t->root);
        printf("\n");
        return;
    }
}

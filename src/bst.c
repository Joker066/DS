#include "bst.h"

#include <stdlib.h>
#include <stdio.h>

/* Internal node for the BST. Parent pointer simplifies deletion. */
typedef struct node node;
struct node {
    int   v;
    node *p;
    node *l;
    node *r;
};

/* Opaque tree representation. */
struct binary_tree {
    node  *root;
    size_t size;
};

/* ---- internal helpers --------------------------------------------------- */

static node *node_new(int v) {
    node *n = (node*)malloc(sizeof(*n));
    if (!n) {
        return NULL;
    }
    else {
        n->v = v;
        n->p = NULL;
        n->l = NULL;
        n->r = NULL;
        return n;
    }
}

static void node_free_subtree(node *n) {
    if (!n) {
        return;
    }
    else {
        node_free_subtree(n->l);
        node_free_subtree(n->r);
        free(n);
        return;
    }
}

static node *node_min(node *n) {
    if (!n) {
        return NULL;
    }
    else {
        while (n->l) {
            n = n->l;
        }
        return n;
    }
}

static node *node_search(node *n, int v) {
    while (n) {
        if (v == n->v) {
            return n;
        }
        else if (v < n->v) {
            n = n->l;
        }
        else {
            n = n->r;
        }
    }
    return NULL;
}

/* Replaces subtree rooted at u with subtree rooted at v.
   Updates parent pointers and the tree root as needed. */
static void bst_transplant(bst *t, node *u, node *v) {
    if (!u->p) {
        t->root = v;
    }
    else if (u == u->p->l) {
        u->p->l = v;
    }
    else {
        u->p->r = v;
    }

    if (v) {
        v->p = u->p;
    }
}

/* Inorder print for debugging. */
static void node_inorder(const node *n) {
    if (!n) {
        return;
    }
    else {
        node_inorder(n->l);
        printf("%d ", n->v);
        node_inorder(n->r);
        return;
    }
}

/* ---- public API --------------------------------------------------------- */

bst *new_bst(void) {
    bst *t = (bst*)malloc(sizeof(*t));
    if (!t) {
        return NULL;
    }
    else {
        t->root = NULL;
        t->size = 0;
        return t;
    }
}

void bst_dispose(bst *self) {
    if (!self) {
        return;
    }
    else {
        node_free_subtree(self->root);
        free(self);
        return;
    }
}

size_t bst_size(const bst *self) {
    if (!self) {
        return 0;
    }
    else {
        return self->size;
    }
}

bool bst_search(const bst *self, int v) {
    if (!self) {
        return false;
    }
    else {
        return node_search(self->root, v) != NULL;
    }
}

int bst_insert(bst *self, int v) {
    if (!self) {
        return BST_ERR_OOM; /* treat as failure */
    }
    else if (!self->root) {
        node *n = node_new(v);
        if (!n) {
            return BST_ERR_OOM;
        }
        else {
            self->root = n;
            self->size = 1;
            return BST_OK;
        }
    }
    else {
        node *cur = self->root;
        node *parent = NULL;

        while (cur) {
            parent = cur;
            if (v == cur->v) {
                return BST_ERR_DUPLICATE;
            }
            else if (v < cur->v) {
                cur = cur->l;
            }
            else {
                cur = cur->r;
            }
        }

        node *n = node_new(v);
        if (!n) {
            return BST_ERR_OOM;
        }
        else {
            n->p = parent;
            if (v < parent->v) {
                parent->l = n;
            }
            else {
                parent->r = n;
            }
            self->size += 1;
            return BST_OK;
        }
    }
}

int bst_del(bst *self, int v) {
    if (!self || !self->root) {
        return BST_ERR_NOTFOUND;
    }
    else {
        node *z = node_search(self->root, v);
        if (!z) {
            return BST_ERR_NOTFOUND;
        }
        else {
            if (!z->l) {
                bst_transplant(self, z, z->r);
            }
            else if (!z->r) {
                bst_transplant(self, z, z->l);
            }
            else {
                node *y = node_min(z->r); /* successor */
                if (y->p != z) {
                    bst_transplant(self, y, y->r);
                    y->r = z->r;
                    if (y->r) {
                        y->r->p = y;
                    }
                }
                bst_transplant(self, z, y);
                y->l = z->l;
                if (y->l) {
                    y->l->p = y;
                }
            }

            free(z);
            self->size -= 1;
            return BST_OK;
        }
    }
}

void bst_print_inorder(const bst *self) {
    if (!self || !self->root) {
        printf("\n");
        return;
    }
    else {
        node_inorder(self->root);
        printf("\n");
        return;
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct node node;
struct node {
		int v;
		node *p, *l, *r;
};

node *node_new(int v) {
		node *n = (node *)malloc(sizeof(node));
		n->v = v, n->p = NULL, n->l = NULL, n->r = NULL;
		return n;
}

static void node_dispose(node *self) {
		if (!self) return;
		if (self->l) node_dispose(self->l);
		if (self->r) node_dispose(self->r);
		free(self);
}

static void node_inorder(const node *self) {
		if (self->l) node_inorder(self->l);
		printf("%d ", self->v);
		if (self->r) node_inorder(self->r);
}

static void node_insert(node *self, int v, int l_r, int c_l_r) {
		node *old_child = l_r ? self->l : self->r;
		node *n = node_new(v);
		n->p = self;
		if (c_l_r) n->l = old_child;
		else n->r = old_child;
		if (old_child) old_child->p = n;
		if (l_r) self->l = n;
		else self->r = n;
}

static void node_delete(node *self, int l_r) {
		node *n = NULL;
		n = l_r ? self->l : self->r;
		//node *old = l_r ? self->l : self->r;
		if (self->p->l == self) self->p->l = n;
		else self->p->r = n;
		if (n) n->p = self->p;
		//free(old);
}

typedef struct binary_tree bst;
void bst_dispose(bst*);
void bst_print_inorder(const bst*);
int bst_search(const bst*, int);
void bst_insert(bst*, int);
void bst_del(bst*, int);

struct binary_tree {
		node *d;
		int size;
		void (*dispose)(bst *self);
		void (*print_inorder)(const bst *self);
		int (*search)(const bst *self, int v);
    void (*insert)(bst *self, int v);
    void (*del)(bst *self, int v);
};

bst *new_bst() {
		bst *b = (bst*)malloc(sizeof(bst));
		b->d = node_new(-1);
		b->size = 0;
    b->dispose = bst_dispose;
    b->print_inorder = bst_print_inorder;
    b->search = bst_search;
    b->insert = bst_insert;
    b->del = bst_del;
		return b;
}

void bst_dispose(bst *self) {
		node_dispose(self->d);
		free(self);
}

void bst_print_inorder(const bst *self) {
		node_inorder(self->d->l);
		printf("\n");
}

int bst_search(const bst *self, int v) {
		node *cur = self->d->l;
		while (cur) {
				if (cur->v == v) return 1;
				cur = v > cur->v ? cur->r : cur->l;
		}
		return 0;
}

void bst_insert(bst *self, int v) {
		if (!self->size) {
				self->d->l = node_new(v);
				self->size++;
				return;
		}
		if (self->search(self, v)) return;
		node *cur = self->d->l, *pre = NULL;
		int l_r;
		while (cur) {
				pre = cur;
				l_r = v < cur->v ? 1 : 0;
				cur = v < cur->v ? cur->l : cur->r;
		}
		node_insert(pre, v, l_r, 1);
		self->size++;
}

void bst_del(bst *self, int v) {
		if (!self->search(self, v)) return;
		self->size--;
		node *cur = self->d->l;
		while (cur) {
				if (cur->v == v) break;
				cur = v < cur->v ? cur->l : cur->r;
		}
		if (cur->l) {
				node *n = cur->l;
				while (1) {
						if (!n->r) {
								cur->v = n->v;
								node_delete(n, 1);
								return;
						}
						n = n->r;
				}
		}
		else if (cur->r) {
				node *n = cur->r;
				while (1) {
						if (!n->l) {
								cur->v = n->v;
								node_delete(n, 0);
								return;
						}
						n = n->l;
				}
		}
		else node_delete(cur, 1);
}

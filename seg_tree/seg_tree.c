#include <stdio.h>
#include <stdlib.h>
typedef struct node node;
struct node {
	int left, right;
	int sum;
	node *left_c, *right_c;
};
node *node_new(const int l, const int r, const int d, node *l_c, node *r_c){
	node *n = (node*)malloc(sizeof(node));
	n->left = l, n->right = r;
	n->sum = d;
	n->left_c = l_c, n->right_c = r_c;
	return n;
}

void node_dispose(node *self){
	if (self->left_c) {
		node_dispose(self->left_c);
	}
	if (self->right_c) {
		node_dispose(self->right_c);
	}
	free(self);
}

node *root_build(int l, int r, int *arr){
	if (l == r) {
		return node_new(l, r, arr[l], NULL, NULL);
	}
	int mid = (l + r) / 2;
	node *root = node_new(l, r, 0, NULL, NULL);
	root->left_c  = root_build(l, mid, arr);
	root->right_c = root_build(mid + 1, r, arr);
	root->sum = root->left_c->sum + root->right_c->sum;
	return root;
}

void node_modify(node *root, int index, int d){
	if (root->left == root->right) {
		root->sum = d;
		return;
	}
	int mid = (root->left + root->right) / 2;
	if ((root->left <= index) && (index <= mid)) {
		node_modify(root->left_c, index, d);
	}
	else if ((mid <= index) && (index <= root->right)) {
		node_modify(root->right_c, index, d);
	}
	root->sum = root->left_c->sum + root->right_c->sum;
}

typedef struct seg_tree seg_tree;

seg_tree *new_seg_tree(int*, int);
int seg_sum(node*, int, int);
int seg_query(seg_tree*, int, int);
void seg_modify(seg_tree*, int, int);
void print_arr(node*);
void seg_tree_print(seg_tree*);

struct seg_tree {
	int * arr, len;
	node *root;
	int (*sum)(node *root, int l, int r);
	int (*query)(seg_tree *self, int l, int r);
	int (*modify)(seg_tree *self, int l, int r);
	void (*print)(seg_tree *self);

};

seg_tree *new_seg_tree(int *arr, int len){
	seg_tree *s = (seg_tree*)malloc(sizeof(seg_tree));
	s->arr = arr, s->len = len;
	s->root = root_build(0, len - 1, arr);
	s->sum = seg_sum;
	s->query = seg_query;
	s->modify = seg_query;
	s->print = seg_tree_print;
	return s;
}

int seg_sum(node *root, int l, int r){
	if ((root->left > r) || (root->right < l)) {
		return 0;
	}
	if ((root->left >= l) && (root->right <= r)) {
		return root->sum;
	}
	return seg_sum(root->left_c, l, r) + seg_sum(root->right_c, l, r);
}

int seg_query(seg_tree *self, int l, int r){
	return seg_sum(self->root, l, r);
}

void seg_modify(seg_tree *self, int index, int d){
	node_modify(self->root, index, d);
}

void print_arr(node *root){
	if (!root->left_c && !root->right_c) {
		printf("%d ", root->sum);
		return;
	}
	print_arr(root->left_c);
	print_arr(root->right_c);
}

void seg_tree_print(seg_tree *self){
	print_arr(self->root);
	printf("\n");
}

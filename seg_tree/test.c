#include "seg_tree.c"
int main(){
	int A[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	seg_tree *a = new_seg_tree(A, 10);
	a->print(a);
	printf("%d\n", a->query(a, 0, 5));
	a->modify(a, 5, 10);
	a->print(a);
	printf("%d\n", a->query(a, 0, 5));
	return 0;
}

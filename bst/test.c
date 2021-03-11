#include "bst.c"
int main() {
		srand(time(NULL));
		bst *a = new_bst();
		for (int i = 0; i < 15; i++)
				a->insert(a, rand() % 15);
		a->print_inorder(a);
		printf("%d\n", a->search(a, 1));
		a->del(a, 1);
		a->print_inorder(a);
		a->dispose(a);
		return 0;
}

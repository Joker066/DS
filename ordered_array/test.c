#include "ordered_array.c"

int main(){
		int a[8] = {8, 6, 5, 4, 3, 2, 1, 0};
		ordered_array *A = new_ordered_array(a, 8);
		A->print(A);
		A->insert(A, 7);
		A->print(A);
		A->update(A, 6, 10);
		A->print(A);
		A->del(A, 0);
		A->print(A);
		printf("%d\n%d\n", A->search(A, 10), A->search(A, 0));
		return 0;
}

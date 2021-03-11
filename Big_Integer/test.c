#include "Big_Integer.c"
int main() {
		big *a = new_big("11111111111111111111111111111111");
		a->print(a);
		a->add(a, new_big("11111111111111111111111111111111"));
		a->print(a);
		a->sub(a, new_big("11111111111111111111111111111111"));
		a->print(a);
		a->mul(a, new_big("11111111111111111111111111111111"));
		a->print(a);
		a->mulint(a, 0);
		a->print(a);
		a->dispose(a);
		return 0;
}

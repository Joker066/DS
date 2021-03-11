#include "dictionary.c"
int main(){
		dict *d = new_dict();
		d->insert(d, "cat", "bad");
		d->insert(d, "dog", "good");
		printf("%s\n%s\n", d->search(d, "cat"), d->search(d, "dog"));
		d->del(d, "dog");
		printf("%s\n", d->search(d, "dog"));
		return 0;
}

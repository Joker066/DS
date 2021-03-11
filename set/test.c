#include "set.c"
int main(){
    set *d = new_set();
    d->insert(d, "cat");
    d->insert(d, "dog");
    printf("%s\n%s\n", d->search(d, "cat"), d->search(d, "dog"));
    d->del(d, "dog");
    printf("%s\n", d->search(d, "dog"));
    return 0;
}

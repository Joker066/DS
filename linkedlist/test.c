#include <stdio.h>
#include "linkedlist.c"

int main() {
    linkedlist *ll = llist_new();
    for(int i = 0; i < 10; i++)
        ll->push_back(ll, i);
    ll->print(ll);
    ll->delete_index(ll, 0);
    ll->print(ll);
    ll->delete_v(ll, 5);
    ll->print(ll);
    ll->insert(ll, 5, 100);
    ll->print(ll);
    printf("%d\n", ll->indexof(ll, 100));
    printf("%d\n", ll->get_index(ll, 6));
    return 0;
}

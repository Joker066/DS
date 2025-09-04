#include "ds.h"
#include <stdio.h>

int main(void) {
    /* queue */
    queue *q = queue_new();
    queue_push(q, 10);
    queue_push(q, 20);
    int x;
    queue_pop(q, &x);  printf("queue pop: %d\n", x);
    queue_free(q);

    /* stack */
    stack *s = stack_new();
    stack_push(s, 7);
    stack_peek(s, &x); printf("stack peek: %d\n", x);
    stack_free(s);

    /* ordered array */
    oarr *oa = oarr_new();
    oarr_insert(oa, 5);
    oarr_insert(oa, 2);
    oarr_insert(oa, 9);
    oarr_print(oa);
    oarr_free(oa);

    /* rb_tree */
    rb_tree *t = rb_tree_new();
    rb_tree_insert(t, 3);
    rb_tree_insert(t, 1);
    rb_tree_insert(t, 4);
    rb_tree_contains(t, 2) ? printf("found 2\n") : printf("no 2\n");
    rb_tree_remove(t, 3);
    rb_tree_debug_inorder(t);
    rb_tree_free(t);

    return 0;
}

#include "queue.c"
#include <stdio.h>
int main() {
	queue *q = new_queue();
	for (int i = 0; i < 50; i++)
		q->push(q, i);
	for (int i = 0; i < 50; i++)
		printf("%d\n", q->pop(q));
	printf("%d\n", q->peek(q));
    q->dispose(q);
	return 0;
}

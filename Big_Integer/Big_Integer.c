#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct Big_Integer big;
int big_eq(big*, big*);
void big_dispose(big*);
void big_print(big*);
int big_gt(big*, big*);
void big_add(big*, big*);
void big_sub(big*, big*);
void big_mul(big*, big*);
void big_mulint(big*, int);

struct Big_Integer {
		int num[1000];
		void (*print)(big *self);
		int (*eq)(big *a, big *b);
		int (*gt)(big *a, big *b);
		void (*add)(big *a, big *b);
		void (*sub)(big *a, big *b);
		void (*mul)(big *a, big *b);
		void (*mulint)(big *a, int);
		void (*dispose)(big *self);
};

big *new_big(const char *s) {
		big *b = (big*)malloc(sizeof(big));
		for (int i = 0; i < 100; i++) b->num[i] = 0;
		int len = strlen(s), index = 0, count = 0, pow;
		for (int i = len - 1; i >= 0; i--) {
				if (count == 0) pow = 1;
				else if (count == 1) pow = 10;
				else if (count == 2) pow = 100;
				else if (count == 3) pow = 1000;
				int digit = s[i] - '0';
				b->num[index] += digit * pow;
				count++;
				if (count == 4) {
						count = 0, index++;
				}
		}
		b->dispose = big_dispose;
		b->print = big_print;
		b->eq = big_eq;
		b->gt = big_gt;
		b->add = big_add;
		b->sub = big_sub;
		b->mul = big_mul;
		b->mulint = big_mulint;
		return b;
}

void big_dispose(big *self){
		if (self) free(self);
}

void big_print(big *a) {
		if (big_eq(a, new_big("0"))) printf("0");
		int i = 0, index = 99;
		for (i = 99; i >= 0 && a->num[i] == 0; i--)
				index = i - 1;
		for (int j = index - 1; j >= 0; j--) {
				if (999 < a->num[j] && a->num[j] < 10000) printf("%s", "");
				else if (99 < a->num[j] && a->num[j] < 1000) printf("0");
				else if (9 < a->num[j] && a->num[j] < 100) printf("00");
				else printf("000");
				printf("%d", a->num[j]);
		}
		printf("\n");
}

int big_eq(big *a, big *b) {
		for (int i = 0; i < 100; i++)
				if (a->num[i] != b->num[i]) return 0;
		return 1;
}

int big_gt(big *a, big *b) {
		for (int i = 99; i >= 0; i--) {
				if (a->num[i] > b->num[i]) return 1;
				if (a->num[i] < b->num[i]) return 0;
		}
		return 0;
}

void big_add(big *a, big *b) {
		int carry = 0;
		for (int i = 0; i < 100; i++) {
				a->num[i] += b->num[i] + carry;
				carry = a->num[i] / 10000;
				a->num[i] %= 10000;
		}
}

void big_sub(big *a, big *b) {
		int borrow = 0;
		for (int i = 0; i < 100; i++) {
				a->num[i] -= borrow + b->num[i];
				borrow = (a->num[i] < 0);
				a->num[i] += 10000 * borrow;
		}
}

void big_mul(big *a, big *b) {
		int c[200] = {};
		for (int i = 0; i < 100; i++)
				for (int j = 0; j < 100; j++)
						c[i + j] += a->num[i] * b->num[j];
		for (int i = 0; i < 200; i++)
				if (c[i] > 10000) {
						c[i + 1] += c[i] / 10000;
						c[i] %= 10000;
				}
		for (int i = 0; i < 100; i++) a->num[i] = c[i];
}

void big_mulint(big *a, int b) {
		for (int i = 0; i < 100; i++)
				a->num[i] *= b;
		for (int i = 0; i < 99; i++)
				if (a->num[i] > 10000) {
						a->num[i + 1] += a->num[i] / 10000;
						a->num[i] %= 10000;
				}
}

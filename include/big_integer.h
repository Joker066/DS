#ifndef DS_BIG_INTEGER_H
#define DS_BIG_INTEGER_H

/* Public interface for a base-10000 big integer with fixed limb capacity.
   The struct is opaque; internal layout is hidden in the .c file.
   Operations return explicit status codes; no error printing is performed. */

#include <stddef.h>  /* size_t */
#include <stdbool.h> /* bool */

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status codes ------------------------------------------------------- */
#define BIG_OK            0
#define BIG_ERR_OOM      -2  /* allocation failure */
#define BIG_ERR_RANGE    -3  /* invalid range (e.g., negative result not supported) */
#define BIG_ERR_OVERFLOW -5  /* result exceeds configured capacity */
#define BIG_ERR_FORMAT   -6  /* invalid input string */

/* Opaque handle */
typedef struct Big_Integer big;

/* ---- Lifecycle ---------------------------------------------------------- */
/* Parses a non-negative decimal string "s" (e.g., "0", "12345").
   Returns NULL on allocation failure or invalid format. */
big *new_big(const char *s);

/* Frees the instance (safe on NULL). */
void big_dispose(big *self);

/* ---- Queries ------------------------------------------------------------ */
void big_print(const big *self);         /* debug helper; prints a decimal representation with '\n' */
bool big_eq(const big *a, const big *b); /* equality: 1 if equal, 0 otherwise */
bool big_gt(const big *a, const big *b); /* strict greater-than: 1 if a>b, 0 otherwise */

/* ---- Arithmetic (in-place) ---------------------------------------------- */
/* All operations write the result back into the left operand ("a"). */
int big_add(big *a, const big *b);   /* a = a + b */
int big_sub(big *a, const big *b);   /* a = a - b; returns BIG_ERR_RANGE if a < b (no negative support) */
int big_mul(big *a, const big *b);   /* a = a * b */
int big_mulint(big *a, int m);       /* a = a * m; m must be >= 0 */

#ifdef __cplusplus
}
#endif
#endif /* DS_BIG_INTEGER_H */

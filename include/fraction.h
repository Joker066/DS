#ifndef DS_FRACTION_H
#define DS_FRACTION_H

/* Public interface for rational numbers using 64-bit signed numerator/denominator.
   Fractions are always kept in normalized form (gcd=1, denominator > 0).
   Operations return explicit status codes; no diagnostic printing is performed. */

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status codes ------------------------------------------------------- */
#define FRAC_OK             0
#define FRAC_ERR_ZERO_DEN  -10   /* denominator is zero */
#define FRAC_ERR_OVERFLOW  -11   /* 64-bit overflow detected */
#define FRAC_ERR_RANGE     -3    /* invalid input range */

/* Value type (exposed deliberately; always normalized by API) */
typedef struct fraction {
    long long num;  /* numerator */
    long long den;  /* denominator > 0 */
} fraction;

/* ---- Constructors / normalization -------------------------------------- */
/* Creates a normalized fraction (num/den); returns ZERO_DEN if den==0. */
int  fraction_make(long long num, long long den, fraction *out);

/* Reduces to lowest terms and fixes sign so denominator > 0. */
int  fraction_normalize(fraction *io);

/* ---- Queries ------------------------------------------------------------ */
static inline long long fraction_num(const fraction *x) { return x->num; }
static inline long long fraction_den(const fraction *x) { return x->den; }
bool fraction_is_zero(const fraction *x);

/* Comparison: writes -1, 0, +1 to *out_sign comparing x ? y. */
int  fraction_cmp(const fraction *x, const fraction *y, int *out_sign);

/* ---- Arithmetic --------------------------------------------------------- */
/* All results are normalized on success. */
int  fraction_add(const fraction *x, const fraction *y, fraction *out);
int  fraction_sub(const fraction *x, const fraction *y, fraction *out);
int  fraction_mul(const fraction *x, const fraction *y, fraction *out);
int  fraction_div(const fraction *x, const fraction *y, fraction *out); /* ZERO_DEN if y==0 */

#ifdef __cplusplus
}
#endif
#endif /* DS_FRACTION_H */

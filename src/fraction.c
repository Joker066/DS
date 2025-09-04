#include "fraction.h"

#include <limits.h>
#include <stdlib.h>

/* Greatest common divisor (non-negative result).
   Iterative form avoids deep recursion. */
static long long gcdll(long long a, long long b) {
    if (a < 0) {
        a = -a;
    }
    if (b < 0) {
        b = -b;
    }
    while (b != 0) {
        long long t = a % b;
        a = b;
        b = t;
    }
    return (a < 0) ? -a : a;
}

/* Safe multiply overflow check for 64-bit signed integers.
   Returns 1 if x*y would overflow, else 0. */
static int mul_overflow_ll(long long x, long long y) {
    if (x == 0 || y == 0) {
        return 0;
    }
    if (x == -1) {
        return y == LLONG_MIN;
    }
    if (y == -1) {
        return x == LLONG_MIN;
    }
    if (x > 0) {
        if (y > 0) {
            return x > LLONG_MAX / y;
        }
        else {
            return y < LLONG_MIN / x;
        }
    }
    else {
        if (y > 0) {
            return x < LLONG_MIN / y;
        }
        else {
            /* both negative */
            return x != 0 && y < LLONG_MAX / x;
        }
    }
}

/* Safe add overflow check for 64-bit signed integers.
   Returns 1 if x+y would overflow, else 0. */
static int add_overflow_ll(long long x, long long y) {
    if (y > 0) {
        return x > LLONG_MAX - y;
    }
    else {
        return x < LLONG_MIN - y;
    }
}

/* Cross-cancel helper: divides *a by g=gcd(|*a|, *d) and *d by g. */
static void cancel_pair(long long *a, long long *d) {
    long long g = gcdll(*a, *d);
    if (g > 1) {
        *a /= g;
        *d /= g;
    }
}

/* Ensures denominator > 0; flips signs if needed. */
static void normalize_sign(fraction *x) {
    if (x->den < 0) {
        x->den = -x->den;
        x->num = -x->num;
    }
}

int fraction_make(long long num, long long den, fraction *out) {
    if (!out) {
        return FRAC_ERR_RANGE;
    }
    else if (den == 0) {
        return FRAC_ERR_ZERO_DEN;
    }
    else {
        fraction r = { num, den };
        normalize_sign(&r);
        (void)fraction_normalize(&r);
        *out = r;
        return FRAC_OK;
    }
}

int fraction_normalize(fraction *io) {
    if (!io) {
        return FRAC_ERR_RANGE;
    }
    else if (io->den == 0) {
        return FRAC_ERR_ZERO_DEN;
    }
    else {
        normalize_sign(io);
        long long g = gcdll(io->num, io->den);
        if (g > 1) {
            io->num /= g;
            io->den /= g;
        }
        return FRAC_OK;
    }
}

bool fraction_is_zero(const fraction *x) {
    if (!x) {
        return true;
    }
    else {
        return x->num == 0;
    }
}

int fraction_cmp(const fraction *x, const fraction *y, int *out_sign) {
    if (!x || !y || !out_sign) {
        return FRAC_ERR_RANGE;
    }
    else {
        /* Compare x.num/x.den ? y.num/y.den using cross multiplication with cancellation. */
        long long a = x->num;
        long long b = x->den;
        long long c = y->num;
        long long d = y->den;

        /* Cross-cancel to reduce overflow risk: (a/b ? c/d) -> (a/g1)/(b/g2) and (c/g2)/(d/g1) */
        cancel_pair(&a, &d);
        cancel_pair(&c, &b);

        /* Compare a*d ? c*b; after cancellation this is less likely to overflow. */
        if (mul_overflow_ll(a, d) || mul_overflow_ll(c, b)) {
            /* Fallback by converting to long double for comparison only (no exactness guarantee).
               This avoids UB while giving a reasonable ordering for extreme values. */
            long double ld1 = (long double)x->num / (long double)x->den;
            long double ld2 = (long double)y->num / (long double)y->den;
            if (ld1 < ld2) {
                *out_sign = -1;
            }
            else if (ld1 > ld2) {
                *out_sign = 1;
            }
            else {
                *out_sign = 0;
            }
            return FRAC_OK;
        }
        else {
            long long left  = a * d;
            long long right = c * b;
            if (left < right) {
                *out_sign = -1;
            }
            else if (left > right) {
                *out_sign = 1;
            }
            else {
                *out_sign = 0;
            }
            return FRAC_OK;
        }
    }
}

int fraction_add(const fraction *x, const fraction *y, fraction *out) {
    if (!x || !y || !out) {
        return FRAC_ERR_RANGE;
    }
    else {
        /* Cross-cancel to minimize overflow: (a/b)+(c/d) = (a*d + c*b)/(b*d) */
        long long a = x->num, b = x->den;
        long long c = y->num, d = y->den;

        cancel_pair(&a, &d);
        cancel_pair(&c, &b);

        /* Check multiplications and addition for overflow. */
        if (mul_overflow_ll(a, d) || mul_overflow_ll(c, b)) {
            return FRAC_ERR_OVERFLOW;
        }
        long long ad = a * d;
        long long cb = c * b;

        if (add_overflow_ll(ad, cb)) {
            return FRAC_ERR_OVERFLOW;
        }
        long long num = ad + cb;

        if (mul_overflow_ll(b, d)) {
            return FRAC_ERR_OVERFLOW;
        }
        long long den = b * d;

        fraction r = { num, den };
        (void)fraction_normalize(&r);
        *out = r;
        return FRAC_OK;
    }
}

int fraction_sub(const fraction *x, const fraction *y, fraction *out) {
    if (!x || !y || !out) {
        return FRAC_ERR_RANGE;
    }
    else {
        fraction negy = { -y->num, y->den };
        return fraction_add(x, &negy, out);
    }
}

int fraction_mul(const fraction *x, const fraction *y, fraction *out) {
    if (!x || !y || !out) {
        return FRAC_ERR_RANGE;
    }
    else {
        /* Cross-cancel before multiply: (a/b)*(c/d) */
        long long a = x->num, b = x->den;
        long long c = y->num, d = y->den;

        cancel_pair(&a, &d);
        cancel_pair(&c, &b);

        if (mul_overflow_ll(a, c) || mul_overflow_ll(b, d)) {
            return FRAC_ERR_OVERFLOW;
        }

        fraction r = { a * c, b * d };
        (void)fraction_normalize(&r);
        *out = r;
        return FRAC_OK;
    }
}

int fraction_div(const fraction *x, const fraction *y, fraction *out) {
    if (!x || !y || !out) {
        return FRAC_ERR_RANGE;
    }
    else if (y->num == 0) {
        return FRAC_ERR_ZERO_DEN; /* division by zero */
    }
    else {
        /* x / y == x * (y.den / y.num) */
        fraction inv = { y->den, y->num };
        return fraction_mul(x, &inv, out);
    }
}

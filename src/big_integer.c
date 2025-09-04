#include "big_integer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Internal configuration:
   - Base 10^4 per limb for compact storage and simple carry handling.
   - Fixed number of limbs to keep the implementation simple and static. */
#ifndef BIG_BASE
#define BIG_BASE       10000u
#endif

#ifndef BIG_BASE_DIG
#define BIG_BASE_DIG   4u
#endif

#ifndef BIG_LIMBS
#define BIG_LIMBS      100u    /* matches original implementation intent */
#endif

/* Internal representation: opaque to callers. */
struct Big_Integer {
    int num[BIG_LIMBS];        /* least significant limb at index 0 */
};

/* ---- internal utilities ------------------------------------------------- */

static void big_zero(big *x) {
    if (!x) {
        return;
    }
    else {
        for (size_t i = 0; i < BIG_LIMBS; ++i) {
            x->num[i] = 0;
        }
        return;
    }
}

static bool big_is_zero(const big *x) {
    if (!x) {
        return true;
    }
    else {
        for (size_t i = 0; i < BIG_LIMBS; ++i) {
            if (x->num[i] != 0) {
                return false;
            }
            else {
                /* continue scanning */
            }
        }
        return true;
    }
}

/* Trims leading zero limbs and returns the index of the highest non-zero limb + 1
   (i.e., logical length in limbs). Zero returns 0. */
static size_t big_logical_len(const big *x) {
    for (size_t i = BIG_LIMBS; i > 0; --i) {
        if (x->num[i - 1] != 0) {
            return i;
        }
        else {
            /* keep scanning downwards */
        }
    }
    return 0;
}

/* Parses a non-negative decimal string into limbs (base 10^4).
   On invalid format, returns BIG_ERR_FORMAT; on success, BIG_OK. */
static int big_parse_decimal(const char *s, big *out) {
    if (!s || !out) {
        return BIG_ERR_FORMAT;
    }
    else {
        /* Skip leading spaces and optional '+' */
        while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r' || *s == '\f' || *s == '\v') {
            ++s;
        }
        if (*s == '+') {
            ++s;
        }

        /* All remaining characters must be digits; allow "0" */
        const char *p = s;
        if (*p == '\0') {
            return BIG_ERR_FORMAT;
        }
        else {
            bool any_digit = false;
            while (*p) {
                if (*p >= '0' && *p <= '9') {
                    any_digit = true;
                    ++p;
                }
                else {
                    return BIG_ERR_FORMAT;
                }
            }
            if (!any_digit) {
                return BIG_ERR_FORMAT;
            }
        }

        big_zero(out);

        /* Consume digits from the end in chunks of BIG_BASE_DIG (4). */
        size_t limb_index = 0;
        size_t len = strlen(s);
        while (len > 0) {
            unsigned chunk = 0;
            unsigned pow10 = 1;
            unsigned count = 0;

            while (len > 0 && count < BIG_BASE_DIG) {
                char c = s[len - 1];
                chunk += (unsigned)(c - '0') * pow10;
                pow10 *= 10u;
                --len;
                ++count;
            }

            if (limb_index >= BIG_LIMBS) {
                return BIG_ERR_OVERFLOW; /* too many digits for configured capacity */
            }
            else {
                out->num[limb_index++] = (int)chunk;
            }
        }

        return BIG_OK;
    }
}

/* Adds b into a with base BIG_BASE and returns BIG_OK or BIG_ERR_OVERFLOW. */
static int big_add_assign(big *a, const big *b) {
    unsigned carry = 0u;
    for (size_t i = 0; i < BIG_LIMBS; ++i) {
        unsigned sum = (unsigned)a->num[i] + (unsigned)b->num[i] + carry;
        a->num[i] = (int)(sum % BIG_BASE);
        carry = sum / BIG_BASE;
    }
    if (carry) {
        return BIG_ERR_OVERFLOW;
    }
    else {
        return BIG_OK;
    }
}

/* Subtracts b from a (assumes a >= b). Returns BIG_OK or BIG_ERR_RANGE. */
static int big_sub_assign(big *a, const big *b) {
    /* Check a >= b */
    for (size_t i = BIG_LIMBS; i > 0; --i) {
        int ai = a->num[i - 1];
        int bi = b->num[i - 1];
        if (ai > bi) {
            break;
        }
        else if (ai < bi) {
            return BIG_ERR_RANGE; /* negative result not supported */
        }
        else {
            /* continue */
        }
    }

    unsigned borrow = 0u;
    for (size_t i = 0; i < BIG_LIMBS; ++i) {
        int diff = a->num[i] - (int)borrow - b->num[i];
        if (diff < 0) {
            diff += (int)BIG_BASE;
            borrow = 1u;
        }
        else {
            borrow = 0u;
        }
        a->num[i] = diff;
    }
    return BIG_OK;
}

/* ---- public API --------------------------------------------------------- */

big *new_big(const char *s) {
    big *b = (big*)malloc(sizeof(*b));
    if (!b) {
        return NULL;
    }
    else {
        big_zero(b);
        if (!s) {
            /* treat NULL as zero */
            return b;
        }
        else {
            int rc = big_parse_decimal(s, b);
            if (rc != BIG_OK) {
                free(b);
                return NULL;
            }
            else {
                return b;
            }
        }
    }
}

void big_dispose(big *self) {
    if (self) {
        free(self);
    }
    else {
        /* nothing */
    }
}

void big_print(const big *a) {
    if (!a || big_is_zero(a)) {
        printf("0\n");
        return;
    }
    else {
        /* Find highest non-zero limb */
        size_t n = big_logical_len(a);
        size_t i = n - 1;

        /* Print the highest limb without zero padding */
        printf("%d", a->num[i]);

        /* Print remaining limbs with leading zeros to width BIG_BASE_DIG */
        for (; i > 0; --i) {
            int limb = a->num[i - 1];
            /* zero-pad to BIG_BASE_DIG=4 */
            printf("%0*d", (int)BIG_BASE_DIG, limb);
        }
        printf("\n");
        return;
    }
}

bool big_eq(const big *a, const big *b) {
    if (a == b) {
        return true;
    }
    else if (!a || !b) {
        return false;
    }
    else {
        for (size_t i = 0; i < BIG_LIMBS; ++i) {
            if (a->num[i] != b->num[i]) {
                return false;
            }
            else {
                /* keep scanning */
            }
        }
        return true;
    }
}

bool big_gt(const big *a, const big *b) {
    if (!a || !b) {
        return false;
    }
    else {
        for (size_t i = BIG_LIMBS; i > 0; --i) {
            int ai = a->num[i - 1];
            int bi = b->num[i - 1];
            if (ai > bi) {
                return true;
            }
            else if (ai < bi) {
                return false;
            }
            else {
                /* equal at this limb; continue */
            }
        }
        return false;
    }
}

int big_add(big *a, const big *b) {
    if (!a || !b) {
        return BIG_ERR_RANGE;
    }
    else {
        return big_add_assign(a, b);
    }
}

int big_sub(big *a, const big *b) {
    if (!a || !b) {
        return BIG_ERR_RANGE;
    }
    else {
        return big_sub_assign(a, b);
    }
}

int big_mul(big *a, const big *b) {
    if (!a || !b) {
        return BIG_ERR_RANGE;
    }
    else {
        /* 2*N limbs workspace to detect overflow after carry propagation */
        unsigned long long c[2 * BIG_LIMBS];
        for (size_t i = 0; i < 2 * BIG_LIMBS; ++i) {
            c[i] = 0ull;
        }

        for (size_t i = 0; i < BIG_LIMBS; ++i) {
            if (a->num[i] == 0) {
                continue;
            }
            else {
                for (size_t j = 0; j < BIG_LIMBS; ++j) {
                    if (b->num[j] == 0) {
                        continue;
                    }
                    else {
                        c[i + j] += (unsigned long long)(a->num[i]) * (unsigned long long)(b->num[j]);
                    }
                }
            }
        }

        /* Carry propagate across the workspace */
        for (size_t i = 0; i < 2 * BIG_LIMBS - 1; ++i) {
            unsigned long long carry = c[i] / BIG_BASE;
            c[i] %= BIG_BASE;
            c[i + 1] += carry;
        }

        /* Check overflow beyond BIG_LIMBS limbs */
        for (size_t i = BIG_LIMBS; i < 2 * BIG_LIMBS; ++i) {
            if (c[i] != 0ull) {
                return BIG_ERR_OVERFLOW;
            }
            else {
                /* keep checking */
            }
        }

        /* Commit back into "a" */
        for (size_t i = 0; i < BIG_LIMBS; ++i) {
            a->num[i] = (int)c[i];
        }
        return BIG_OK;
    }
}

int big_mulint(big *a, int m) {
    if (!a || m < 0) {
        return BIG_ERR_RANGE;
    }
    else {
        unsigned long long carry = 0ull;
        for (size_t i = 0; i < BIG_LIMBS; ++i) {
            unsigned long long prod = (unsigned long long)a->num[i] * (unsigned long long)m + carry;
            a->num[i] = (int)(prod % BIG_BASE);
            carry = prod / BIG_BASE;
        }
        if (carry != 0ull) {
            return BIG_ERR_OVERFLOW;
        }
        else {
            return BIG_OK;
        }
    }
}

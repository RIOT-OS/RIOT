/**
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_frac
 * @{
 * @file
 * @brief    Integer fraction function implementations
 *
 * @author   Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include "assert.h"
#include "frac.h"
#include "bitarithm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

uint32_t gcd32(uint32_t u, uint32_t v)
{
    /* Source: https://en.wikipedia.org/wiki/Binary_GCD_algorithm#Iterative_version_in_C */
    unsigned shift;

    /* GCD(0,v) == v; GCD(u,0) == u, GCD(0,0) == 0 */
    if (u == 0) {
        return v;
    }
    if (v == 0) {
        return u;
    }

    /* Let shift := log2 K, where K is the greatest power of 2
     * dividing both u and v. */
    for (shift = 0; ((u | v) & 1) == 0; ++shift) {
        u >>= 1;
        v >>= 1;
    }

    /* remove all factors of 2 in u */
    while ((u & 1) == 0) {
        u >>= 1;
    }

    /* From here on, u is always odd. */
    do {
        /* remove all factors of 2 in v -- they are not common */
        /*   note: v is not zero, so while will terminate */
        while ((v & 1) == 0) {
            v >>= 1;
        }

        /* Now u and v are both odd. Swap if necessary so u <= v,
         * then set v = v - u (which is even). */
        if (u > v) {
            /* Swap u and v */
            uint32_t t = v;
            v = u;
            u = t;
        }

        v = v - u; /* Here v >= u */
    } while (v != 0);

    /* restore common factors of 2 */
    return u << shift;
}

static uint32_t frac_long_divide(uint32_t num, uint32_t den, int *prec, uint32_t *rem)
{
    /* Binary long division with adaptive number of fractional bits */
    /* The result will be a Qx.y number where x is the number of bits in the
     * integer part and y = 64 - x. Similar to floating point, except the result
     * is unsigned, and we can only represent numbers in the range 2**-32..(2**32 - 1) */
    assert(den); /* divide by zero */

    uint32_t q = 0; /* Quotient */
    uint64_t r = 0;  /* Remainder */
    if (prec) {
        *prec = 0;
    }
    if (num == 0) {
        if (rem) {
            *rem = 0;
        }
        return 0;
    }
    unsigned p = bitarithm_msb(num);
    int i_bits = p + 1; /* Number of integer bits in the result */
    uint32_t num_mask = (1ul << p);
    for (unsigned k = 0; k < (64u + p); ++k) {
        r <<= 1;
        q <<= 1;
        if (num & num_mask) {
            r |= 1;
        }
        num_mask >>= 1;
        if (r >= den) {
            r -= den;
            q |= 1;
        }
        if (q == 0) {
            --i_bits;
        }
        if (q & (1ul << 31u)) {
            /* result register is full */
            break;
        }
        if ((r == 0) && (num == 0)) {
            /* divides evenly */
            break;
        }
    }
    if (r > 0) {
        ++q;
    }
    if (prec) {
        *prec = i_bits;
    }
    if (rem) {
        *rem = r;
    }

    return q;
}

void frac_init(frac_t *frac, uint32_t num, uint32_t den)
{
    DEBUG("frac_init32(%p, %" PRIu32 ", %" PRIu32 ")\n", (const void *)frac, num, den);
    assert(den);
    /* Reduce the fraction to shortest possible form by dividing by the greatest
     * common divisor */
    uint32_t gcd = gcd32(num, den);
    /* Divide den and num by their greatest common divisor */
    den /= gcd;
    num /= gcd;
    int prec = 0;
    uint32_t rem = 0;
    frac->frac = frac_long_divide(num, den, &prec, &rem);
    frac->shift = (sizeof(frac->frac) * 8) - prec;
    DEBUG("frac_init32: gcd = %" PRIu32 " num = %" PRIu32 " den = %" PRIu32 " frac = 0x%08" PRIx32 " shift = %02d, rem = 0x%08" PRIx32 "\n",
          gcd, num, den, frac->frac, frac->shift, rem);
}

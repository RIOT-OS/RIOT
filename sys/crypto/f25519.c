/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 * Copyright (C) 2014 Daniel Beer <dlbeer@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       Implementation of arithmetic on integers mod 2^255-19
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Daniel Beer <dlbeer@gmail.com>
 * @author      D. J. Bernstein
 *
 */

#include "crypto/f25519.h"

/* Identity constants */
const uint8_t f25519_zero[F25519_SIZE] = {0};
const uint8_t f25519_one[F25519_SIZE] = {1};

/* Load a small constant */
inline void f25519_load(uint8_t *x, uint32_t c)
{
    uint32_t i;

    for (i = 0; i < sizeof(c); i++) {
        x[i] = c;
        c >>= 8;
    }

    for (; i < F25519_SIZE; i++) {
        x[i] = 0;
    }
}

/* Normalize a field point x < 2*p by subtracting p if necessary */
inline void f25519_normalize(uint8_t *x)
{
    uint8_t minusp[F25519_SIZE];
    uint16_t c;
    int i;

    /* Reduce using 2^255 = 19 mod p */
    c = (x[31] >> 7) * 19;
    x[31] &= 127;

    for (i = 0; i < F25519_SIZE; i++) {
        c += x[i];
        x[i] = c;
        c >>= 8;
    }

    /* The number is now less than 2^255 + 18, and therefore less than * 2 */
    c = 19;

    for (i = 0; i + 1 < F25519_SIZE; i++) {
        c += x[i];
        minusp[i] = c;
        c >>= 8;
    }

    c += ((uint16_t)x[i]) - 128;
    minusp[31] = c;

    /* Load x-p if no underflow */
    f25519_select(x, minusp, x, (c >> 15) & 1);
}

/* Compare two field points in constant time */
uint8_t f25519_eq(const uint8_t *x, const uint8_t *y)
{
    uint8_t sum = 0;
    int i;

    for (i = 0; i < F25519_SIZE; i++) {
        sum |= x[i] ^ y[i];
    }

    sum |= (sum >> 4);
    sum |= (sum >> 2);
    sum |= (sum >> 1);

    return (sum ^ 1) & 1;
}

/* Conditional copy. If condition == 0, then zero is copied to dst */
inline void f25519_select(uint8_t *dst, const uint8_t *zero, const uint8_t *one, uint8_t condition)
{
    const uint8_t mask = -condition;
    int i;

    for (i = 0; i < F25519_SIZE; i++) {
        dst[i] = zero[i] ^ (mask & (one[i] ^ zero[i]));
    }
}

/* Add two field points */
inline void f25519_add(uint8_t *r, const uint8_t *a, const uint8_t *b)
{
    uint16_t c = 0;
    int i;

    /* Add */
    for (i = 0; i < F25519_SIZE; i++) {
        c >>= 8;
        c += ((uint16_t)a[i]) + ((uint16_t)b[i]);
        r[i] = c;
    }

    /* Reduce with 2^255 = 19 mod p */
    r[31] &= 127;
    c = (c >> 7) * 19;

    for (i = 0; i < F25519_SIZE; i++) {
        c += r[i];
        r[i] = c;
        c >>= 8;
    }
}

/* Subtract two field points */
inline void f25519_sub(uint8_t *r, const uint8_t *a, const uint8_t *b)
{
    uint32_t c = 0;
    int i;

    /* Calculate a + 2p - b, to ainline void underflow */
    c = 218;

    for (i = 0; i + 1 < F25519_SIZE; i++) {
        c += 65280 + ((uint32_t)a[i]) - ((uint32_t)b[i]);
        r[i] = c;
        c >>= 8;
    }

    c += ((uint32_t)a[31]) - ((uint32_t)b[31]);
    r[31] = c & 127;
    c = (c >> 7) * 19;

    for (i = 0; i < F25519_SIZE; i++) {
        c += r[i];
        r[i] = c;
        c >>= 8;
    }
}

/* Unary negation */
inline void f25519_neg(uint8_t *r, const uint8_t *a)
{
    uint32_t c = 0;
    int i;

    /* Calculate 2p - a, to ainline void underflow */
    c = 218;

    for (i = 0; i + 1 < F25519_SIZE; i++) {
        c += 65280 - ((uint32_t)a[i]);
        r[i] = c;
        c >>= 8;
    }

    c -= ((uint32_t)a[31]);
    r[31] = c & 127;
    c = (c >> 7) * 19;

    for (i = 0; i < F25519_SIZE; i++) {
        c += r[i];
        r[i] = c;
        c >>= 8;
    }
}

/* Multiply two field points */
inline void f25519_mul__distinct(uint8_t *r, const uint8_t *a, const uint8_t *b)
{
    uint32_t c = 0;
    int i;

    for (i = 0; i < F25519_SIZE; i++) {
        int j;

        c >>= 8;

        for (j = 0; j <= i; j++) {
            c += ((uint32_t)a[j]) * ((uint32_t)b[i - j]);
        }

        for (; j < F25519_SIZE; j++)
            c += ((uint32_t)a[j]) *
                 ((uint32_t)b[i + F25519_SIZE - j]) * 38;

        r[i] = c;
    }

    r[31] &= 127;
    c = (c >> 7) * 19;

    for (i = 0; i < F25519_SIZE; i++) {
        c += r[i];
        r[i] = c;
        c >>= 8;
    }
}

/* Multiply two field points */
inline void f25519_mul(uint8_t *r, const uint8_t *a, const uint8_t *b)
{
    uint8_t tmp[F25519_SIZE];

    f25519_mul__distinct(tmp, a, b);
    f25519_copy(r, tmp);
}

/* Multiply a point by a small constant */
inline void f25519_mul_c(uint8_t *r, const uint8_t *a, uint32_t b)
{
    uint32_t c = 0;
    int i;

    for (i = 0; i < F25519_SIZE; i++) {
        c >>= 8;
        c += b * ((uint32_t)a[i]);
        r[i] = c;
    }

    r[31] &= 127;
    c >>= 7;
    c *= 19;

    for (i = 0; i < F25519_SIZE; i++) {
        c += r[i];
        r[i] = c;
        c >>= 8;
    }
}

/* Take the reciprocal of a field point */
inline void f25519_inv__distinct(uint8_t *r, const uint8_t *x)
{
    uint8_t s[F25519_SIZE];
    int i;

    /* 1 1 */
    f25519_mul__distinct(s, x, x);
    f25519_mul__distinct(r, s, x);

    /* 1 x 248 */
    for (i = 0; i < 248; i++) {
        f25519_mul__distinct(s, r, r);
        f25519_mul__distinct(r, s, x);
    }

    /* 0 */
    f25519_mul__distinct(s, r, r);

    /* 1 */
    f25519_mul__distinct(r, s, s);
    f25519_mul__distinct(s, r, x);

    /* 0 */
    f25519_mul__distinct(r, s, s);

    /* 1 */
    f25519_mul__distinct(s, r, r);
    f25519_mul__distinct(r, s, x);

    /* 1 */
    f25519_mul__distinct(s, r, r);
    f25519_mul__distinct(r, s, x);
}

/* Take the reciprocal of a field point */
inline void f25519_inv(uint8_t *r, const uint8_t *x)
{
    uint8_t tmp[F25519_SIZE];

    f25519_inv__distinct(tmp, x);
    f25519_copy(r, tmp);
}

/* Raise x to the power of (p-5)/8 = 2^252-3 */
static void exp2523(uint8_t *r, const uint8_t *x, uint8_t *s)
{
    int i;

    /* 1 1 */
    f25519_mul__distinct(r, x, x);
    f25519_mul__distinct(s, r, x);

    /* 1 x 248 */
    for (i = 0; i < 248; i++) {
        f25519_mul__distinct(r, s, s);
        f25519_mul__distinct(s, r, x);
    }

    /* 0 */
    f25519_mul__distinct(r, s, s);

    /* 1 */
    f25519_mul__distinct(s, r, r);
    f25519_mul__distinct(r, s, x);
}

/* Compute one of the square roots of the field element */
inline void f25519_sqrt(uint8_t *r, const uint8_t *a)
{
    uint8_t v[F25519_SIZE];
    uint8_t i[F25519_SIZE];
    uint8_t x[F25519_SIZE];
    uint8_t y[F25519_SIZE];

    /* v = (2a)^((p-5)/8) [x = 2a] */
    f25519_mul_c(x, a, 2);
    exp2523(v, x, y);

    /* i = 2av^2 - 1 */
    f25519_mul__distinct(y, v, v);
    f25519_mul__distinct(i, x, y);
    f25519_load(y, 1);
    f25519_sub(i, i, y);

    /* r = avi */
    f25519_mul__distinct(x, v, a);
    f25519_mul__distinct(r, x, i);
}

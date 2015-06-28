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
 * @brief       Implementation of arithmetic on integers modulo arbitrary primes
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Daniel Beer <dlbeer@gmail.com>
 * @author      D. J. Bernstein
 *
 */

#include "crypto/fprime.h"

/* Add two field points */
static void raw_add(uint8_t *x, const uint8_t *p)
{
    uint16_t c = 0;
    int i;

    for (i = 0; i < FPRIME_SIZE; i++) {
        c += ((uint16_t)x[i]) + ((uint16_t)p[i]);
        x[i] = c;
        c >>= 8;
    }
}

/* Subtract two field points */
static void raw_try_sub(uint8_t *x, const uint8_t *p)
{
    uint8_t minusp[FPRIME_SIZE];
    uint16_t c = 0;
    int i;

    for (i = 0; i < FPRIME_SIZE; i++) {
        c = ((uint16_t)x[i]) - ((uint16_t)p[i]) - c;
        minusp[i] = c;
        c = (c >> 8) & 1;
    }

    fprime_select(x, minusp, x, c);
}

/* Return most significant bit */
static int prime_msb(const uint8_t *p)
{
    int i;
    uint8_t x;

    for (i = FPRIME_SIZE - 1; i >= 0; i--)
        if (p[i]) {
            break;
        }

    x = p[i];
    i <<= 3;

    while (x) {
        x >>= 1;
        i++;
    }

    return i - 1;
}

/* Shift field point by N bits */
static void shift_n_bits(uint8_t *x, int n)
{
    uint16_t c = 0;
    int i;

    for (i = 0; i < FPRIME_SIZE; i++) {
        c |= ((uint16_t)x[i]) << n;
        x[i] = c;
        c >>= 8;
    }
}

/* Load a small constant */
void fprime_load(uint8_t *x, uint32_t c)
{
    uint32_t i;

    for (i = 0; i < sizeof(c); i++) {
        x[i] = c;
        c >>= 8;
    }

    for (; i < FPRIME_SIZE; i++) {
        x[i] = 0;
    }
}

/* Return smaller integer value */
static inline int min_int(int a, int b)
{
    return a < b ? a : b;
}

/* Load a large constant */
void fprime_from_bytes(uint8_t *n, const uint8_t *x, size_t len, const uint8_t *modulus)
{
    const int preload_total = min_int(prime_msb(modulus) - 1, len << 3);
    const int preload_bytes = preload_total >> 3;
    const int preload_bits = preload_total & 7;
    const int rbits = (len << 3) - preload_total;
    int i;

    memset(n, 0, FPRIME_SIZE);

    for (i = 0; i < preload_bytes; i++) {
        n[i] = x[len - preload_bytes + i];
    }

    if (preload_bits) {
        shift_n_bits(n, preload_bits);
        n[0] |= x[len - preload_bytes - 1] >> (8 - preload_bits);
    }

    for (i = rbits - 1; i >= 0; i--) {
        const uint8_t bit = (x[i >> 3] >> (i & 7)) & 1;

        shift_n_bits(n, 1);
        n[0] |= bit;
        raw_try_sub(n, modulus);
    }
}

/* Normalize a field element */
void fprime_normalize(uint8_t *x, const uint8_t *modulus)
{
    uint8_t n[FPRIME_SIZE];

    fprime_from_bytes(n, x, FPRIME_SIZE, modulus);
    fprime_copy(x, n);
}

/* Compare two field points in constant time */
uint8_t fprime_eq(const uint8_t *x, const uint8_t *y)
{
    uint8_t sum = 0;
    int i;

    for (i = 0; i < FPRIME_SIZE; i++) {
        sum |= x[i] ^ y[i];
    }

    sum |= (sum >> 4);
    sum |= (sum >> 2);
    sum |= (sum >> 1);

    return (sum ^ 1) & 1;
}

/* Conditional copy based on condition */
void fprime_select(uint8_t *dst, const uint8_t *zero, const uint8_t *one, uint8_t condition)
{
    const uint8_t mask = -condition;
    int i;

    for (i = 0; i < FPRIME_SIZE; i++) {
        dst[i] = zero[i] ^ (mask & (one[i] ^ zero[i]));
    }
}

/* Add one value to another. The two pointers must be distinct. */
void fprime_add(uint8_t *r, const uint8_t *a, const uint8_t *modulus)
{
    raw_add(r, a);
    raw_try_sub(r, modulus);
}

/* Subtract one value from another. The two pointers must be distinct. */
void fprime_sub(uint8_t *r, const uint8_t *a, const uint8_t *modulus)
{
    raw_add(r, modulus);
    raw_try_sub(r, a);
    raw_try_sub(r, modulus);
}

/* Multiply two values to get a third. r must be distinct from a and b */
void fprime_mul(uint8_t *r, const uint8_t *a, const uint8_t *b,
                const uint8_t *modulus)
{
    int i;

    memset(r, 0, FPRIME_SIZE);

    for (i = prime_msb(modulus); i >= 0; i--) {
        const uint8_t bit = (b[i >> 3] >> (i & 7)) & 1;
        uint8_t plusa[FPRIME_SIZE];

        shift_n_bits(r, 1);
        raw_try_sub(r, modulus);

        fprime_copy(plusa, r);
        fprime_add(plusa, a, modulus);

        fprime_select(r, r, plusa, bit);
    }
}

/* Compute multiplicative inverse. r must be distinct from a */
void fprime_inv(uint8_t *r, const uint8_t *a, const uint8_t *modulus)
{
    uint8_t pm2[FPRIME_SIZE];
    uint16_t c = 2;
    int i;

    /* Compute (p-2) */
    fprime_copy(pm2, modulus);

    for (i = 0; i < FPRIME_SIZE; i++) {
        c = modulus[i] - c;
        pm2[i] = c;
        c >>= 8;
    }

    /* Binary exponentiation */
    fprime_load(r, 1);

    for (i = prime_msb(modulus); i >= 0; i--) {
        uint8_t r2[FPRIME_SIZE];

        fprime_mul(r2, r, r, modulus);

        if ((pm2[i >> 3] >> (i & 7)) & 1) {
            fprime_mul(r, r2, a, modulus);
        }
        else {
            fprime_copy(r, r2);
        }
    }
}

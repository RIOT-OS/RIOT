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
 * @brief       Implementation the Edwards-curve equivalent of Curve25519
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Daniel Beer <dlbeer@gmail.com>
 * @author      D. J. Bernstein
 *
 */

#include "crypto/ed25519.h"

/* Projective coordinates - base points */
const struct ed25519_pt ed25519_base = {
    .x = {
        0x1a, 0xd5, 0x25, 0x8f, 0x60, 0x2d, 0x56, 0xc9,
        0xb2, 0xa7, 0x25, 0x95, 0x60, 0xc7, 0x2c, 0x69,
        0x5c, 0xdc, 0xd6, 0xfd, 0x31, 0xe2, 0xa4, 0xc0,
        0xfe, 0x53, 0x6e, 0xcd, 0xd3, 0x36, 0x69, 0x21
    },
    .y = {
        0x58, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
        0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
        0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
        0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66
    },
    .t = {
        0xa3, 0xdd, 0xb7, 0xa5, 0xb3, 0x8a, 0xde, 0x6d,
        0xf5, 0x52, 0x51, 0x77, 0x80, 0x9f, 0xf0, 0x20,
        0x7d, 0xe3, 0xab, 0x64, 0x8e, 0x4e, 0xea, 0x66,
        0x65, 0x76, 0x8b, 0xd7, 0x0f, 0x5f, 0x87, 0x67
    },
    .z = {1, 0}
};

/* Projective coordinates - neurtral */
const struct ed25519_pt ed25519_neutral = {
    .x = {0},
    .y = {1, 0},
    .t = {0},
    .z = {1, 0}
};

/* Conversion to projective coordinates */
void ed25519_project(struct ed25519_pt *p, const uint8_t *x, const uint8_t *y)
{
    f25519_copy(p->x, x);
    f25519_copy(p->y, y);
    f25519_load(p->z, 1);
    f25519_mul__distinct(p->t, x, y);
}

/* Conversion from projective coordinates */
void ed25519_unproject(uint8_t *x, uint8_t *y, const struct ed25519_pt *p)
{
    uint8_t z1[F25519_SIZE];

    f25519_inv__distinct(z1, p->z);
    f25519_mul__distinct(x, p->x, z1);
    f25519_mul__distinct(y, p->y, z1);

    f25519_normalize(x);
    f25519_normalize(y);
}

static const uint8_t ed25519_d[F25519_SIZE] = {
    0xa3, 0x78, 0x59, 0x13, 0xca, 0x4d, 0xeb, 0x75,
    0xab, 0xd8, 0x41, 0x41, 0x4d, 0x0a, 0x70, 0x00,
    0x98, 0xe8, 0x79, 0x77, 0x79, 0x40, 0xc7, 0x8c,
    0x73, 0xfe, 0x6f, 0x2b, 0xee, 0x6c, 0x03, 0x52
};

/* Compress coordinate points */
void ed25519_pack(uint8_t *c, const uint8_t *x, const uint8_t *y)
{
    uint8_t tmp[F25519_SIZE];
    uint8_t parity;

    f25519_copy(tmp, x);
    f25519_normalize(tmp);
    parity = (tmp[0] & 1) << 7;

    f25519_copy(c, y);
    f25519_normalize(c);
    c[31] |= parity;
}

/* Uncompress point and check if it is on the curve */
uint8_t ed25519_try_unpack(uint8_t *x, uint8_t *y, const uint8_t *comp)
{
    const int parity = comp[31] >> 7;
    uint8_t a[F25519_SIZE];
    uint8_t b[F25519_SIZE];
    uint8_t c[F25519_SIZE];

    /* Unpack y */
    f25519_copy(y, comp);
    y[31] &= 127;

    /* Compute c = y^2 */
    f25519_mul__distinct(c, y, y);

    /* Compute b = (1+dy^2)^-1 */
    f25519_mul__distinct(b, c, ed25519_d);
    f25519_add(a, b, f25519_one);
    f25519_inv__distinct(b, a);

    /* Compute a = y^2-1 */
    f25519_sub(a, c, f25519_one);

    /* Compute c = a*b = (y^2-1)/(1-dy^2) */
    f25519_mul__distinct(c, a, b);

    /* Compute a, b = +/-sqrt(c), if c is square */
    f25519_sqrt(a, c);
    f25519_neg(b, a);

    /* Select one of them, based on the compressed parity bit */
    f25519_select(x, a, b, (a[0] ^ parity) & 1);

    /* Verify that x^2 = c */
    f25519_mul__distinct(a, x, x);
    f25519_normalize(a);
    f25519_normalize(c);

    return f25519_eq(a, c);
}

/* k = 2d */
static const uint8_t ed25519_k[F25519_SIZE] = {
    0x59, 0xf1, 0xb2, 0x26, 0x94, 0x9b, 0xd6, 0xeb,
    0x56, 0xb1, 0x83, 0x82, 0x9a, 0x14, 0xe0, 0x00,
    0x30, 0xd1, 0xf3, 0xee, 0xf2, 0x80, 0x8e, 0x19,
    0xe7, 0xfc, 0xdf, 0x56, 0xdc, 0xd9, 0x06, 0x24
};

/* Addition operation (Assumptions: Z1=1 and Z2=1 and k=2*d) */
void ed25519_add(struct ed25519_pt *r, const struct ed25519_pt *p1, const struct ed25519_pt *p2)
{
    uint8_t a[F25519_SIZE];
    uint8_t b[F25519_SIZE];
    uint8_t c[F25519_SIZE];
    uint8_t d[F25519_SIZE];
    uint8_t e[F25519_SIZE];
    uint8_t f[F25519_SIZE];
    uint8_t g[F25519_SIZE];
    uint8_t h[F25519_SIZE];

    /* A = (Y1-X1)(Y2-X2) */
    f25519_sub(c, p1->y, p1->x);
    f25519_sub(d, p2->y, p2->x);
    f25519_mul__distinct(a, c, d);

    /* B = (Y1+X1)(Y2+X2) */
    f25519_add(c, p1->y, p1->x);
    f25519_add(d, p2->y, p2->x);
    f25519_mul__distinct(b, c, d);

    /* C = T1 k T2 */
    f25519_mul__distinct(d, p1->t, p2->t);
    f25519_mul__distinct(c, d, ed25519_k);

    /* D = Z1 2 Z2 */
    f25519_mul__distinct(d, p1->z, p2->z);
    f25519_add(d, d, d);

    /* E = B - A */
    f25519_sub(e, b, a);

    /* F = D - C */
    f25519_sub(f, d, c);

    /* G = D + C */
    f25519_add(g, d, c);

    /* H = B + A */
    f25519_add(h, b, a);

    /* X3 = E F */
    f25519_mul__distinct(r->x, e, f);

    /* Y3 = G H */
    f25519_mul__distinct(r->y, g, h);

    /* T3 = E H */
    f25519_mul__distinct(r->t, e, h);

    /* Z3 = F G */
    f25519_mul__distinct(r->z, f, g);
}

/* Doubling operation (Assumptions: Z1=1) */
void ed25519_double(struct ed25519_pt *r, const struct ed25519_pt *p)
{
    uint8_t a[F25519_SIZE];
    uint8_t b[F25519_SIZE];
    uint8_t c[F25519_SIZE];
    uint8_t e[F25519_SIZE];
    uint8_t f[F25519_SIZE];
    uint8_t g[F25519_SIZE];
    uint8_t h[F25519_SIZE];

    /* A = X1^2 */
    f25519_mul__distinct(a, p->x, p->x);

    /* B = Y1^2 */
    f25519_mul__distinct(b, p->y, p->y);

    /* C = 2 Z1^2 */
    f25519_mul__distinct(c, p->z, p->z);
    f25519_add(c, c, c);

    /* D = a A (alter sign) */
    /* E = (X1+Y1)^2-A-B */
    f25519_add(f, p->x, p->y);
    f25519_mul__distinct(e, f, f);
    f25519_sub(e, e, a);
    f25519_sub(e, e, b);

    /* G = D + B */
    f25519_sub(g, b, a);

    /* F = G - C */
    f25519_sub(f, g, c);

    /* H = D - B */
    f25519_neg(h, b);
    f25519_sub(h, h, a);

    /* X3 = E F */
    f25519_mul__distinct(r->x, e, f);

    /* Y3 = G H */
    f25519_mul__distinct(r->y, g, h);

    /* T3 = E H */
    f25519_mul__distinct(r->t, e, h);

    /* Z3 = F G */
    f25519_mul__distinct(r->z, f, g);
}

/* Scalar multiplication */
void ed25519_smult(struct ed25519_pt *r_out, const struct ed25519_pt *p, const uint8_t *e)
{
    struct ed25519_pt r;
    int i;

    ed25519_copy(&r, &ed25519_neutral);

    for (i = 255; i >= 0; i--) {
        const uint8_t bit = (e[i >> 3] >> (i & 7)) & 1;
        struct ed25519_pt s;

        ed25519_double(&r, &r);
        ed25519_add(&s, &r, p);

        f25519_select(r.x, r.x, s.x, bit);
        f25519_select(r.y, r.y, s.y, bit);
        f25519_select(r.z, r.z, s.z, bit);
        f25519_select(r.t, r.t, s.t, bit);
    }

    ed25519_copy(r_out, &r);
}

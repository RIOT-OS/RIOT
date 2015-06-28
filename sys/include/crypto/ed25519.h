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
 * @brief       Headers for the implementation of the Edwards-curve equivalent of Curve25519 
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Daniel Beer <dlbeer@gmail.com>
 * @author      D. J. Bernstein
 *
 */

#ifndef __ED25519_H_
#define __ED25519_H_

#include "f25519.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Struct for projective coordinates
 */
struct ed25519_pt {
    uint8_t x[F25519_SIZE];
    uint8_t y[F25519_SIZE];
    uint8_t t[F25519_SIZE];
    uint8_t z[F25519_SIZE];
};

/**
 * @name Operation constants
 * @{
 */
extern const struct ed25519_pt ed25519_base;
extern const struct ed25519_pt ed25519_neutral;

/* Pack points */
#define ED25519_PACK_SIZE	F25519_SIZE
/* Add, double and scalar multiply */
#define ED25519_EXPONENT_SIZE	32
/** @} */

/**
 * @brief  Convert projective and affine coordinates (x/y in F25519)
 *
 * @param[out]  p     Projective coordinates
 * @param[in]   x     Affine coordinate X
 * @param[in]   y     Affine coordinate Y
 */
void ed25519_project(struct ed25519_pt *p, const uint8_t *x, const uint8_t *y);

/**
 * @brief  Convert affine and projective coordinates (x/y in F25519)
 *
 * @param[out]   x     Affine coordinate X
 * @param[out]   y     Affine coordinate Y
 * @param[in]    p     Projective coordinates
 */
void ed25519_unproject(uint8_t *x, uint8_t *y, const struct ed25519_pt *p);

/**
 * @brief Compress field points
 *
 * @param[out]  c     Compressed output
 * @param[in]   x     Field point X
 * @param[in]   y     Field point Y
 */
void ed25519_pack(uint8_t *c, const uint8_t *x, const uint8_t *y);

/**
 * @brief  Uncompress field points
 *
 * @param[in]   x     Field point X
 * @param[in]   y     Field point Y
 * @param[in]   comp  Compressed parity bits
 */
uint8_t ed25519_try_unpack(uint8_t *x, uint8_t *y, const uint8_t *comp);

/**
 * @brief  Add two field points
 *
 * @param[out]  r     Result
 * @param[in]   a     Field point A
 * @param[in]   b     Field point B
 */
void ed25519_add(struct ed25519_pt *r, const struct ed25519_pt *a, const struct ed25519_pt *b);

/**
 * @brief  Double a field point
 *
 * @param[out]  r     Result
 * @param[in]   a     Field point
 */
void ed25519_double(struct ed25519_pt *r, const struct ed25519_pt *a);

/**
 * @brief  Scalar multiply coordinate
 *
 * @param[out]  r     Result of multiplicaiton
 * @param[in]   a     Input projective coordinate
 * @param[in]   e     Scalar multiply exponent
 */
void ed25519_smult(struct ed25519_pt *r, const struct ed25519_pt *a, const uint8_t *e);

/**
 * @brief  Prepare an exponent by clamping appropriate bits
 *
 * @param[in]   e     Exponent to clamp
 */
static inline void ed25519_prepare(uint8_t *e)
{
    e[0] &= 0xf8;
    e[31] &= 0x7f;
    e[31] |= 0x40;
}

/**
 * @brief  Copy a field point
 *
 * @param[out]  dst   Destination field point
 * @param[in]   src   Source field point
 */
static inline void ed25519_copy(struct ed25519_pt *dst, const struct ed25519_pt *src)
{
    memcpy(dst, src, sizeof(*dst));
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __ED25519_H_ */

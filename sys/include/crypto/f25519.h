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
 * @brief       Headers for the implementation of arithmetic on integers mod 2^255-19
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Daniel Beer <dlbeer@gmail.com>
 * @author      D. J. Bernstein
 *
 */

#ifndef __F25519_H_
#define __F25519_H_

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Algorithm/Arithmetic constants
 * @{
 */
#define F25519_SIZE	32

/* Identity constants */
extern const uint8_t f25519_zero[F25519_SIZE];
extern const uint8_t f25519_one[F25519_SIZE];
/** @} */

/**
 * @brief  Load a small constant
 *
 * @param[out]  x     Field point
 * @param[in]   c     Constant
 */
void f25519_load(uint8_t *x, uint32_t c);

/**
 * @brief  Normalize a field point
 *
 * @param[in]   x     Field point
 */
void f25519_normalize(uint8_t *x);

/**
 * @brief  Compare two field points in constant time
 *
 * @param[in]   x     Field point X
 * @param[in]   y     Field point Y
 *
 * @return 1 if equal, 0 otherwise
 */
uint8_t f25519_eq(const uint8_t *x, const uint8_t *y);

/**
 * @brief  Conditional copy
 *
 * @param[out]  dst		Output variable
 * @param[in]   zero		Source variable 0
 * @param[in]   one		Source variable 1
 * @param[in]   condition	Condition (0 or 1)
 */
void f25519_select(uint8_t *dst, const uint8_t *zero, const uint8_t *one, uint8_t condition);

/**
 * @brief  Add two field points
 *
 * @param[out]  r     Result
 * @param[in]   a     Field point A
 * @param[in]   b     Field point B
 */
void f25519_add(uint8_t *r, const uint8_t *a, const uint8_t *b);

/**
 * @brief  Substract two field points
 *
 * @param[out]  r     Result
 * @param[in]   a     Field point A
 * @param[in]   b     Field point B
 */
void f25519_sub(uint8_t *r, const uint8_t *a, const uint8_t *b);

/**
 * @brief  Unary negation
 *
 * @param[out]  r     Result
 * @param[in]   a     Field point
 */
void f25519_neg(uint8_t *r, const uint8_t *a);

/**
 * @brief  Multiply two field points
 *
 * @param[out]  r     Result
 * @param[in]   a     Field point A
 * @param[in]   b     Field point B
 */
void f25519_mul(uint8_t *r, const uint8_t *a, const uint8_t *b);

/**
 * @brief  Multiply two field points (distinct)
 *
 * @param[out]  r     Result
 * @param[in]   a     Field point A
 * @param[in]   b     Field point B
 */
void f25519_mul__distinct(uint8_t *r, const uint8_t *a, const uint8_t *b);

/**
 * @brief  Multiply a point by a small constant
 *
 * @param[out]  r     Result
 * @param[in]   a     Constant value
 * @param[in]   b     Field point
 */
void f25519_mul_c(uint8_t *r, const uint8_t *a, uint32_t b);

/**
 * @brief  Take the reciprocal of a field point
 *
 * @param[out]  r     Result
 * @param[in]   x     Field point
 */
void f25519_inv(uint8_t *r, const uint8_t *x);

/**
 * @brief  Take the reciprocal of a field point (distinct)
 *
 * @param[out]  r     Result
 * @param[in]   x     Field point
 */
void f25519_inv__distinct(uint8_t *r, const uint8_t *x);

/**
 * @brief  Compute one of the square roots of the field element
 *
 * @param[out]  r     Result
 * @param[in]   x     Field point
 */
void f25519_sqrt(uint8_t *r, const uint8_t *x);

/**
 * @brief  Copy a field point
 *
 * @param[out]  dst   Destination field point
 * @param[in]   src   Source field point
 */
static inline void f25519_copy(uint8_t *dst, const uint8_t *src)
{
    memcpy(dst, src, F25519_SIZE);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __F25519_H_ */

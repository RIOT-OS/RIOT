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
 * @brief       Headers for the implementation of arithmetic on integers modulo arbitrary primes
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Daniel Beer <dlbeer@gmail.com>
 * @author      D. J. Bernstein
 *
 */

#ifndef __FPRIME_H_
#define __FPRIME_H_

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Arithmetic constants
 * @{
 */
/* Maximum size of a field element */
#define FPRIME_SIZE	32

/* Useful constants */
extern const uint8_t fprime_zero[FPRIME_SIZE];
extern const uint8_t fprime_one[FPRIME_SIZE];
/** @} */

/**
 * @brief  Load a small constant
 *
 * @param[out]  x     Field point
 * @param[in]   c     Constant
 */
void fprime_load(uint8_t *x, uint32_t c);

/**
 * @brief  Load a large constant
 *
 * @param[out]  x		Field point
 * @param[in]   in		Constant byte array
 * @param[in]   len		Length of constant byte array
 * @param[in]   modulus		Modulus input constant 
 */
void fprime_from_bytes(uint8_t *x, const uint8_t *in, size_t len, const uint8_t *modulus);

/**
 * @brief  Normalize a field point
 *
 * @param[in]   x		Field point
 * @param[in]   modulus		Modulus input constant 
 */
void fprime_normalize(uint8_t *x, const uint8_t *modulus);

/**
 * @brief  Compare two field points in constant time
 *
 * @param[in]   x		Field point X
 * @param[in]   y		Field point Y
 *
 * @return 1 if equal, 0 otherwise
 */
uint8_t fprime_eq(const uint8_t *x, const uint8_t *y);

/**
 * @brief  Conditional copy
 *
 * @param[out]  dst             Output variable
 * @param[in]   zero            Source variable 0
 * @param[in]   one             Source variable 1
 * @param[in]   condition       Condition (0 or 1)
 */
void fprime_select(uint8_t *dst, const uint8_t *zero, const uint8_t *one, uint8_t condition);

/**
 * @brief  Add one field point to another
 *
 * @param[out]  r		Result
 * @param[in]   a		Field point
 * @param[in]   modulus		Modulus input constant 
 */
void fprime_add(uint8_t *r, const uint8_t *a, const uint8_t *modulus);

/**
 * @brief  Substract one field point from another
 *
 * @param[out]  r		Result
 * @param[in]   a		Field point
 * @param[in]   modulus		Modulus input constant 
 */
void fprime_sub(uint8_t *r, const uint8_t *a, const uint8_t *modulus);

/**
 * @brief  Multiply two field points to get a thrid
 *
 * @param[out]  r		Result
 * @param[in]   a		Field point A
 * @param[in]   b		Field point B
 * @param[in]   modulus		Modulus input constant 
 */
void fprime_mul(uint8_t *r, const uint8_t *a, const uint8_t *b, const uint8_t *modulus);

/**
 * @brief  Compute multiplicative inverse
 *
 * @param[out]  r		Result
 * @param[in]   a		Field point
 * @param[in]   modulus		Modulus input constant 
 */
void fprime_inv(uint8_t *r, const uint8_t *a, const uint8_t *modulus);

/**
 * @brief  Copy a field point
 *
 * @param[out]  dst		Destination field point
 * @param[in]   src		Source field point
 */
static inline void fprime_copy(uint8_t *dst, const uint8_t *src)
{
    memcpy(dst, src, FPRIME_SIZE);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __FPRIME_H_ */

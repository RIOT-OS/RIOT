/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Eistec AB
 *               2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup  sys_div   Integer division functions
 * @ingroup   sys
 *
 * This header provides some integer division functions that can be used
 * to prevent linking in compiler-generated ones, which are often larger.
 *
 * @file
 * @ingroup   sys
 *
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author    Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @{
 */

#ifndef DIV_H
#define DIV_H

#include <stdint.h>

#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Calculate 8-bit multiplicative inverse
 *
 * Result is calculated during compilation (requires const num). Bitshifted by
 * 9 to give the most representation to the smallest interesting number (3)
 *
 * @param[in] num  reciprocal
 *
 * @return         result, bitshifted by 9
 */
__attribute__((always_inline)) static inline uint8_t div_inv_8(
                                             const uint8_t num)
{
    /* cannot be represented due to bit shift */
    assert(num > 2);

    const uint16_t numerator = (1 << 9);

    if ((numerator % num) * 2 >= num) {
        /* fraction is >= 0.5, so round up */
        return (uint8_t)(numerator / num) + 1;
    }
    else {
        return (uint8_t)(numerator / num);
    }
}

/**
 * @brief   Calculate 16-bit multiplicative inverse
 *
 * Result is calculated during compilation (requires const num). Bitshifted by
 * 17 to give the most representation to the smallest interesting number (3)
 *
 * @param[in] num  reciprocal
 *
 * @return         result, bitshifted by 17
 */
__attribute__((always_inline)) static inline uint16_t div_inv_16(
                                             const uint16_t num)
{
    /* cannot be represented due to bit shift */
    assert(num > 2);

    const uint32_t numerator = (1UL << 17);

    if ((numerator % num) * 2 >= num) {
        /* fraction is >= 0.5, so round up */
        return (uint16_t)(numerator / num) + 1;
    }
    else {
        return (uint16_t)(numerator / num);
    }
}

/**
 * @brief   Calculate 32-bit multiplicative inverse
 *
 * Result is calculated during compilation (requires const num). Bitshifted by
 * 33 to give the most representation to the smallest interesting number (3)
 *
 * @param[in] num  reciprocal
 *
 * @return         result, bitshifted by 33
 */
__attribute__((always_inline)) static inline uint32_t div_inv_32(
                                             const uint32_t num)
{
    /* cannot be represented due to bit shift */
    assert(num > 2);

    const uint64_t numerator = (1ULL << 33);

    if ((numerator % num) * 2 >= num) {
        /* fraction is >= 0.5, so round up */
        return (uint32_t)(numerator / num) + 1;
    }
    else {
        return (uint32_t)(numerator / num);
    }
}

/**
 * @brief   Calculate 64-bit multiplicative inverse
 *
 * Result is calculated during compilation (requires const num). Bitshifted by
 * 65 to give the most representation to the smallest interesting number (3)
 *
 * @param[in] num  reciprocal
 *
 * @return         result, bitshifted by 65
 */
__attribute__((always_inline)) static inline uint64_t div_inv_64(
                                             const uint64_t num)
{
    /* cannot be represented due to bit shift */
    assert(num > 2);

    /* find the last bits without using 128-bit ints */
    const uint64_t numerator = (1ULL << 63);
    const uint64_t most_sig = (numerator / num) << 2;
    const uint64_t least_sig = ((numerator % num) << 2) / num;

    if ((((numerator % num) << 2) % num) * 2 >= num) {
        /* fraction is >= 0.5, so round up */
        return (most_sig + least_sig + 1);
    }
    else {
        return (most_sig + least_sig);
    }
}

/**
 * @internal
 * @brief Multiply two 64 bit integers into a 128 bit integer and return the upper half.
 *
 * The implementation only uses 64 bit integers internally, no __int128 support
 * is necessary.
 *
 * @see http://stackoverflow.com/questions/28868367/getting-the-high-part-of-64-bit-integer-multiplication

 * @param[in]   a     operand a
 * @param[in]   b     operand b
 * @return      (((uint128_t)a * b) >> 64)
 */
uint64_t _div_mulhi64(const uint64_t a, const uint64_t b);

/**
 * @brief   Divide 8-bit number using an inverse
 *
 * @param[in] val  numerator
 * @param[in] inv  multiplicative inverse of denominator (bitshifted by 9)
 *
 * @return         result
 */
static inline uint8_t div_u8_by_inv(const uint8_t val, const uint8_t inv)
{
    const uint16_t tmp = (uint16_t)val * (uint16_t)inv;

    return (uint8_t)(tmp >> 9);
}

/**
 * @brief   Divide 16-bit number using an inverse
 *
 * @param[in] val  numerator
 * @param[in] inv  multiplicative inverse of denominator (bitshifted by 17)
 *
 * @return         result
 */
static inline uint16_t div_u16_by_inv(const uint16_t val, const uint16_t inv)
{
    const uint32_t tmp = (uint32_t)val * (uint32_t)inv;

    return (uint16_t)(tmp >> 17);
}

/**
 * @brief   Divide 32-bit number using an inverse
 *
 * @param[in] val  numerator
 * @param[in] inv  multiplicative inverse of denominator (bitshifted by 33)
 *
 * @return         result
 */
static inline uint32_t div_u32_by_inv(const uint32_t val, const uint32_t inv)
{
    const uint64_t tmp = (uint64_t)val * (uint64_t)inv;

    return (uint32_t)(tmp >> 33);
}

/**
 * @brief   Divide 64-bit number using an inverse
 *
 * @param[in] val  numerator
 * @param[in] inv  multiplicative inverse of denominator (bitshifted by 65)
 *
 * @return         result
 */
static inline uint64_t div_u64_by_inv(const uint64_t val, const uint64_t inv)
{
    return (_div_mulhi64(val, inv) >> 1);
}

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* DIV_H */

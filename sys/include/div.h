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

#include <inttypes.h>
#include <stdint.h>

#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Internal 65-bit shift and comparison: 2*x > y
 *
 * @param[in] x    greater / 2
 * @param[in] y    lesser
 *
 * @return         if 2*x > y then 1, else 0
 */
__attribute__((always_inline)) __attribute__((optimize("merge-all-constants")))
    static inline uint8_t _div_shcom(const uint64_t x, const uint64_t y)
{
    if (x & (1ULL << 63)) {
        return 1;
    }

    if ((x << 1) > y) {
        return 1;
    }

    return 0;
}

/**
 * @brief   Internal 65-bit shift and subtraction: 2*x - y
 *
 * @param[in] x    minuend / 2
 * @param[in] y    subtrahend
 *
 * @return         difference
 */
__attribute__((always_inline)) __attribute__((optimize("merge-all-constants")))
    static inline uint64_t _div_shsub(const uint64_t x, const uint64_t y)
{
    if (x & (1ULL << 63)) {
        uint64_t tmp;

        /* = 2^64 - 1 - y */
        tmp = (uint64_t)(-1) - y;

        /* = (2^64 - 1 - y) + (2*x - 2^64) */
        /* = 2*x - y - 1 */
        tmp += (x << 1);

        /* = 2*x - y */
        return tmp + 1;
    }

    return (x << 1) - y;
}

/**
 * @brief   Internal 64-bit fraction calculation
 *
 * @param[in] num  numerator
 * @param[in] den  denominator
 *
 * @return         result, bitshifted by 64
 */
__attribute__((always_inline))
__attribute__((optimize("unroll-all-loops", "merge-all-constants")))
    static inline uint64_t _div_frac(const uint64_t num, const uint64_t den)
{
    uint64_t ans = 0, rem = num;

    /* binary long division */
    for (uint8_t i = 0; i < 64; i++) {
        if (_div_shcom(rem, den)) {
            /* den goes into num for this bit */
            ans |= (1ULL << (63 - i));

            /* subtract and move to next bit */
            rem = _div_shsub(rem, den);
        }
        else {
            /* move to next bit */
            rem <<= 1;
        }
    }

    /* rounding */
    if (_div_shcom(rem, den)) {
        ans++;
    }

    return ans;
}

/**
 * @brief   Compile-time calculate 16-bit fraction
 *
 * For fractions that are less than one. Result is calculated during
 * compilation (requires const input).
 *
 * @param[in] num  numerator
 * @param[in] den  denominator
 *
 * @return         result, bitshifted by 16
 */
__attribute__((always_inline)) __attribute__((optimize("merge-all-constants")))
    static inline uint16_t div_frac_16(const uint16_t num, const uint16_t den)
{
    assert((den > 0) && (num < den));

    /* done during compile time, so trade efficiency for simplicity */
    const uint64_t ans = _div_frac((uint64_t)num << 48, (uint64_t)den << 48);

    if ((ans << 16) > (1ULL << 63)) {
        /* round the result */
        return (uint16_t)(ans >> 48) + 1;
    }
    else {
        return (uint16_t)(ans >> 48);
    }
}

/**
 * @brief   Compile-time calculate 32-bit fraction
 *
 * For fractions that are less than one. Result is calculated during
 * compilation (requires const input).
 *
 * @param[in] num  numerator
 * @param[in] den  denominator
 *
 * @return         result, bitshifted by 32
 */
__attribute__((always_inline)) __attribute__((optimize("merge-all-constants")))
    static inline uint32_t div_frac_32(const uint32_t num, const uint32_t den)
{
    assert((den > 0) && (num < den));

    /* done during compile time, so trade efficiency for simplicity */
    const uint64_t ans = _div_frac((uint64_t)num << 32, (uint64_t)den << 32);

    if ((ans << 32) > (1ULL << 63)) {
        /* round the result */
        return (uint32_t)(ans >> 32) + 1;
    }
    else {
        return (uint32_t)(ans >> 32);
    }
}

/**
 * @brief   Compile-time calculate 64-bit fraction
 *
 * For fractions that are less than one. Result is calculated during
 * compilation (requires const input).
 *
 * @param[in] num  numerator
 * @param[in] den  denominator
 *
 * @return         result, bitshifted by 64
 */
__attribute__((always_inline)) __attribute__((optimize("merge-all-constants")))
    static inline uint64_t div_frac_64(const uint64_t num, const uint64_t den)
{
    assert((den > 0) && (num < den));

    const uint64_t ans = _div_frac(num, den);

    return ans;
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
 * @brief   Multiply 16-bit number with a fraction
 *
 * @param[in] val   integer
 * @param[in] frac  fraction (bitshifted by 16)
 *
 * @return          result
 */
static inline uint16_t div_mul_w_frac_16(const uint16_t val, const uint16_t frac)
{
    const uint32_t tmp = (uint32_t)val * (uint32_t)frac;

    return (uint16_t)(tmp >> 16);
}

/**
 * @brief   Multiply 32-bit number with a fraction
 *
 * @param[in] val   integer
 * @param[in] frac  fraction (bitshifted by 32)
 *
 * @return          result
 */
static inline uint32_t div_mul_w_frac_32(const uint32_t val, const uint32_t frac)
{
    const uint64_t tmp = (uint64_t)val * (uint64_t)frac;

    return (uint32_t)(tmp >> 32);
}

/**
 * @brief   Multiply 64-bit number with a fraction
 *
 * @param[in] val   integer
 * @param[in] frac  fraction (bitshifted by 64)
 *
 * @return          result
 */
static inline uint64_t div_mul_w_frac_64(const uint64_t val, const uint64_t frac)
{
    return _div_mulhi64(val, frac);
}

/**
 * @brief   Division of a 16-bit number with high-accuracy
 *
 * This is partially calculated during compilation for speed, which requires
 * that the denominator is a constant.
 *
 * @param[in] num   numerator
 * @param[in] den   denominator
 *
 * @return          result
 */
__attribute__((always_inline))
__attribute__((optimize("unroll-all-loops", "merge-all-constants")))
    static inline uint16_t div_16(const uint16_t num, const uint16_t den)
{
    uint8_t exp;

    /* find highest power of two less than den */
    for (exp = 15; exp > 0; exp--) {
        if ((1U << exp) < den) {
            break;
        }
    }

    /* Make inverse: (1 > inv >= 0.5) for greatest accuracy */
    const uint16_t inv = div_frac_16((1U << exp), den);

    /* last step is only thing calculated at runtime */
    return (div_mul_w_frac_16(num, inv) >> exp);
}

/**
 * @brief   Division of a 32-bit number with high-accuracy
 *
 * This is partially calculated during compilation for speed, which requires
 * that the denominator is a constant.
 *
 * @param[in] num   numerator
 * @param[in] den   denominator
 *
 * @return          result
 */
__attribute__((always_inline))
__attribute__((optimize("unroll-all-loops", "merge-all-constants")))
    static inline uint32_t div_32(const uint32_t num, const uint32_t den)
{
    uint8_t exp;

    /* find highest power of two less than den */
    for (exp = 31; exp > 0; exp--) {
        if ((1UL << exp) < den) {
            break;
        }
    }

    /* Make inverse: (1 > inv >= 0.5) for greatest accuracy */
    const uint32_t inv = div_frac_32((1UL << exp), den);

    /* last step is only thing calculated at runtime */
    return (div_mul_w_frac_32(num, inv) >> exp);
}

/**
 * @brief   Division of a 64-bit number with high-accuracy
 *
 * This is partially calculated during compilation for speed, which requires
 * that the denominator is a constant.
 *
 * @param[in] num   numerator
 * @param[in] den   denominator
 *
 * @return          result
 */
__attribute__((always_inline))
__attribute__((optimize("unroll-all-loops", "merge-all-constants")))
    static inline uint64_t div_64(const uint64_t num, const uint64_t den)
{
    uint8_t exp;

    /* find highest power of two less than den */
    for (exp = 63; exp > 0; exp--) {
        if ((1ULL << exp) < den) {
            break;
        }
    }

    /* Make inverse: (1 > inv >= 0.5) for greatest accuracy */
    const uint64_t inv = div_frac_64((1ULL << exp), den);

    /* last step is only thing calculated at runtime */
    return (div_mul_w_frac_64(num, inv) >> exp);
}

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* DIV_H */

/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup  sys_div   Integer division functions
 * @ingroup   sys_math
 *
 * This header provides some integer division functions that can be used
 * to prevent linking in compiler-generated ones, which are often larger.
 *
 * @file
 * @ingroup   sys_div
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @{
 */

#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Approximation of (2**l)/d for d=15625, l=12, 32 bits
 */
#define DIV_H_INV_15625_32    0x431bde83ul

/**
 * @brief Approximation of (2**l)/d for d=15625, l=12, 64 bits
 */
#define DIV_H_INV_15625_64    0x431bde82d7b634dbull

/**
 * @brief Required shifts for division by 15625, l above
 */
#define DIV_H_INV_15625_SHIFT 12

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
 * @brief Integer divide val by 15625, 64 bit version
 *
 * @param[in]   val     dividend
 * @return      (val / 15625)
 */
static inline uint64_t div_u64_by_15625(uint64_t val)
{
    if (val > 16383999997ull) {
        return (_div_mulhi64(DIV_H_INV_15625_64, val) >> DIV_H_INV_15625_SHIFT);
    }
    return (val * DIV_H_INV_15625_32) >> (DIV_H_INV_15625_SHIFT + 32);
}

/**
 * @brief Integer divide val by 125
 *
 * This function can be used to convert uint64_t microsecond times (or
 * intervals) to milliseconds and store them in uint32_t variables, with up to
 * ~50 days worth of milliseconds ((2**32*1000) -1).
 * Use e.g.,  ms = div_u64_by_125(microseconds >> 3)
 *
 * @pre val <= 536870911999 ((2**32 * 125) -1)
 *
 * @param[in]   val     dividend
 * @return      (val / 125)
 */
static inline uint32_t div_u64_by_125(uint64_t val)
{
  /* a higher value would overflow the result type */
  assert(val <= 536870911999LLU);

  uint32_t hi = val >> 32;
  uint32_t lo = val;
  uint32_t r = (lo >> 16) + (hi << 16);
  uint32_t res = r / 125;
  r = ((r % 125) << 16) + (lo & 0xFFFF);
  res = (res << 16) + r / 125;
  return res;
}

/**
 * @brief Integer divide val by 1000000
 *
 * @param[in]   val     dividend
 * @return      (val / 1000000)
 */
static inline uint64_t div_u64_by_1000000(uint64_t val)
{
    return div_u64_by_15625(val) >> 6;
}

/**
 * @brief Divide val by (15625/512)
 *
 * This is used to quantize a 1MHz value to the closest 32768Hz value,
 * e.g., for timers.
 *
 * The algorithm uses the modular multiplicative inverse of 15625 to use only
 * multiplication and bit shifts to perform the division.
 *
 * The result will be equal to the mathematical expression: floor((val * 512) / 15625)
 *
 * @param[in]   val     dividend
 * @return      (val / (15625/512))
 */
static inline uint32_t div_u32_by_15625div512(uint32_t val)
{
    return ((uint64_t)(val) * DIV_H_INV_15625_32) >> (DIV_H_INV_15625_SHIFT + 32 - 9);
}

/**
 * @brief Divide val by (15625/512)
 *
 * This is used to quantize a 1MHz value to the closest 32768Hz value,
 * e.g., for timers.
 *
 * @param[in]   val     dividend
 * @return      (val / (15625/512))
 */
static inline uint64_t div_u64_by_15625div512(uint64_t val)
{
    /*
     * This saves around 1400 bytes of ROM on Cortex-M platforms (both ARMv6 and
     * ARMv7) from avoiding linking against __aeabi_uldivmod and related helpers
     */
    if (val > 16383999997ull) {
        /* this would overflow 2^64 in the multiplication that follows, need to
         * use the long version */
        return (_div_mulhi64(DIV_H_INV_15625_64, val) >> (DIV_H_INV_15625_SHIFT - 9));
    }
    return (val * DIV_H_INV_15625_32) >> (DIV_H_INV_15625_SHIFT + 32 - 9);
}

/**
 * @brief Integer divide val by 44488
 *
 * @param[in]   val     dividend
 * @return      (val / 44488)
 */
static inline uint32_t div_u32_by_44488(uint32_t val)
{
    return ((uint64_t)val * 0xBC8F1391UL) >> (15 + 32);
}

/**
 * @brief Modulo 44488
 *
 * @param[in]   val     dividend
 * @return      (val % 44488)
 */
static inline uint32_t div_u32_mod_44488(uint32_t val)
{
    return val - (div_u32_by_44488(val)*44488);
}

#ifdef __cplusplus
}
#endif
/** @} */

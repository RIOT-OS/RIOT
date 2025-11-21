/*
 * SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       Helper functions for bit arithmetic
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdint.h>

#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def SETBIT
 * @brief Sets a bitmask for a bitfield
 *
 * @param[in] val   The bitfield
 * @param[in] bit   Specifies the bits to be set
 *
 * @return The modified bitfield
 */
#define SETBIT(val, bit)    val |= (bit)

/**
 * @def CLRBIT
 * @brief Clears bitmask for a bitfield
 *
 * @param[in] val   The bitfield
 * @param[in] bit   Specifies the bits to be cleared
 *
 * @return The modified bitfield
 */
#define CLRBIT(val, bit)    val &= (~(bit))

/**
 * @name    Single Bit Defines
 * @{
 */
#ifndef BIT0
#define BIT0  0x00000001 /**< Bit 0 set define */
#define BIT1  0x00000002 /**< Bit 1 set define */
#define BIT2  0x00000004 /**< Bit 2 set define */
#define BIT3  0x00000008 /**< Bit 3 set define */
#define BIT4  0x00000010 /**< Bit 4 set define */
#define BIT5  0x00000020 /**< Bit 5 set define */
#define BIT6  0x00000040 /**< Bit 6 set define */
#define BIT7  0x00000080 /**< Bit 7 set define */
#define BIT8  0x00000100 /**< Bit 8 set define */
#define BIT9  0x00000200 /**< Bit 9 set define */
#endif
#ifndef BIT10
#define BIT10 0x00000400 /**< Bit 10 set define */
#define BIT11 0x00000800 /**< Bit 11 set define */
#define BIT12 0x00001000 /**< Bit 12 set define */
#define BIT13 0x00002000 /**< Bit 13 set define */
#define BIT14 0x00004000 /**< Bit 14 set define */
#define BIT15 0x00008000 /**< Bit 15 set define */
#endif
#ifndef BIT16
#define BIT16 0x00010000 /**< Bit 16 set define */
#define BIT17 0x00020000 /**< Bit 17 set define */
#define BIT18 0x00040000 /**< Bit 18 set define */
#define BIT19 0x00080000 /**< Bit 19 set define */
#define BIT20 0x00100000 /**< Bit 20 set define */
#define BIT21 0x00200000 /**< Bit 21 set define */
#define BIT22 0x00400000 /**< Bit 22 set define */
#define BIT23 0x00800000 /**< Bit 23 set define */
#define BIT24 0x01000000 /**< Bit 24 set define */
#define BIT25 0x02000000 /**< Bit 25 set define */
#define BIT26 0x04000000 /**< Bit 26 set define */
#define BIT27 0x08000000 /**< Bit 27 set define */
#define BIT28 0x10000000 /**< Bit 28 set define */
#define BIT29 0x20000000 /**< Bit 29 set define */
#define BIT30 0x40000000 /**< Bit 30 set define */
#define BIT31 0x80000000 /**< Bit 31 set define */
#endif
/** @} */

#define ARCH_32_BIT   (__INT_MAX__ == 2147483647) /**< 1 for 32 bit architectures, 0 otherwise */

/**
 * @brief   Returns the number of the highest '1' bit in a value
 * @param[in]   v   Input value
 * @return          Bit Number
 *
 * Source: http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
 */
static inline unsigned bitarithm_msb(unsigned v);

/**
 * @brief   Returns the number of the lowest '1' bit in a value
 * @param[in]   v   Input value - must be unequal to '0', otherwise the
 *                  function will produce an infinite loop
 * @return          Bit Number
 *
 */
static inline unsigned bitarithm_lsb(unsigned v);

/**
 * @brief   Returns the number of bits set in a value
 * @param[in]   v   Input value with platform-dependent word size
 * @return          Number of set bits
 *
 */
unsigned bitarithm_bits_set(unsigned v);

/**
 * @brief   Returns the (uint32_t version) number of bits set in a value
 * @param[in]   v   Input value with 32 bit size
 * @return          Number of set bits
 *
 */
#if ARCH_32_BIT
static inline uint8_t bitarithm_bits_set_u32(uint32_t v)
{
    return bitarithm_bits_set(v);
}
#else
uint8_t bitarithm_bits_set_u32(uint32_t v);
#endif

/**
 * @brief   Returns the number of the highest '1' bit in a value
 *
 *          Internal software implementation for 32 bit platforms,
 *          use @see bitarithm_msb in application code.
 * @param[in]   v   Input value
 * @return          Bit Number
 */
unsigned bitarith_msb_32bit_no_native_clz(unsigned v);

/* implementations */

static inline unsigned bitarithm_msb(unsigned v)
{
#if defined(BITARITHM_HAS_CLZ)
    return 8 * sizeof(v) - __builtin_clz(v) - 1;
#elif ARCH_32_BIT
    return bitarith_msb_32bit_no_native_clz(v);
#else
    unsigned r = 0;
    while (v >>= 1) { /* unroll for more speed... */
        ++r;
    }
    return r;
#endif
}

/**
 * @brief   Returns the number of leading 0-bits in @p x, starting at the most
 *          significant bit position.
 *          If x is 0, the result is undefined.
 *
 * @param[in]   x   Input value
 * @return          Number of leading zero bits
 */
static inline uint8_t bitarithm_clzb(uint8_t x)
{
#if defined(BITARITHM_HAS_CLZ)
    /* clz operates on `unsigned int`, so `x` will be promoted to the size
       of an `unsigned int` */
    return __builtin_clz(x) - 8 * (sizeof(unsigned) - 1);
#else
    uint8_t l = 0;
    while (!(x & 0x80)) {
        ++l;
        x <<= 1;
    }
    return l;
#endif
}

/**
 * @private
 *
 * @brief Lookup table for a fast CLS / LSB implementations.
 *
 * This is not supposed to be public, and should be declared `extern const` in
 * the two functions which use it -- but that causes [transpiler issues], so it
 * is declared here as a workaround. (Once that issue is resolved and part of
 * CI, this line will be removed again).
 *
 * [transpiler issues]: https://github.com/immunant/c2rust/issues/423
 */
extern const uint8_t bitarithm_MultiplyDeBruijnBitPosition[32];

static inline unsigned bitarithm_lsb(unsigned v)
#if defined(BITARITHM_LSB_BUILTIN)
{
    return __builtin_ffs(v) - 1;
}
#elif defined(BITARITHM_LSB_LOOKUP)
{
/* Source: http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightMultLookup */
    /* cppcheck-suppress oppositeExpression
     * (reason: `x & -x` is a bit twiddling idiom to extract the LSB; the
     * check treats opposite arguments as indicator for poor copy-pasting
     * as e.g. `x + -x` or `x & ~x` don't make sense. ) */
    return bitarithm_MultiplyDeBruijnBitPosition[((uint32_t)((v & -v) * 0x077CB531U)) >>
                                       27];
}
#else
{
/* Source: http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious */
    unsigned r = 0;

    while ((v & 0x01) == 0) {
        v >>= 1;
        r++;
    }

    return r;
}
#endif

/**
 * @brief   Used for iterating over the bits in @p state.
 *          Returns the index of a set bit in @p state, returns @p state with that bit cleared.
 *
 * @warning This is a low-level helper function, arguments are not checked.
 *          It is intended to iterate over a bit map until all bits are cleared.
 *          Whether it starts with the highest or lowest bit will depend on what is fastest
 *          on the given hardware.
 *
 * @warning @p state must not be zero.
 *
 * @param[in]   state   Bit Map with at least one bit set
 * @param[out]  index   Index of the first set bit. Must be initialized with 0 before the
 *                      first call to this function, must not be modified between subsequent
 *                      calls.
 *
 * @return      new state value - must be treated as opaque value
 *
 */
static inline unsigned bitarithm_test_and_clear(unsigned state, uint8_t *index)
{
#if defined(BITARITHM_HAS_CLZ)
    *index = 8 * sizeof(state) - __builtin_clz(state) - 1;
    return state & ~(1 << *index);
#elif defined(BITARITHM_LSB_LOOKUP)
    /* Source: http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightMultLookup */
    /* cppcheck-suppress oppositeExpression
     * (reason: `x & -x` is a bit twiddling idiom to extract the LSB; the
     * check treats opposite arguments as indicator for poor copy-pasting
     * as e.g. `x + -x` or `x & ~x` don't make sense. ) */
    uint32_t least_bit = state & -state;
    *index = bitarithm_MultiplyDeBruijnBitPosition[(least_bit * 0x077CB531U) >> 27];
    return state & ~least_bit;
#else
    while ((state & 1) == 0) {
        *index += 1;
        state >>= 1;
    }
    return state & ~1;
#endif
}

#ifdef __cplusplus
}
#endif

/** @} */

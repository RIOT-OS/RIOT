/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file
 * @brief       Helper functions for bit arithmetic
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef BITARITHM_H
#define BITARITHM_H

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
#define BIT0  0x00000001
#define BIT1  0x00000002
#define BIT2  0x00000004
#define BIT3  0x00000008
#define BIT4  0x00000010
#define BIT5  0x00000020
#define BIT6  0x00000040
#define BIT7  0x00000080
#define BIT8  0x00000100
#define BIT9  0x00000200
#define BIT10 0x00000400
#define BIT11 0x00000800
#define BIT12 0x00001000
#define BIT13 0x00002000
#define BIT14 0x00004000
#define BIT15 0x00008000
#endif
#ifndef BIT16
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000
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
unsigned bitarithm_msb(unsigned v);

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

/* implementations */

static inline unsigned bitarithm_lsb(unsigned v)
#if defined(BITARITHM_LSB_BUILTIN)
{
    return __builtin_ffs(v) - 1;
}
#elif defined(BITARITHM_LSB_LOOKUP)
{
/* Source: http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightMultLookup */
    extern const uint8_t MultiplyDeBruijnBitPosition[32];
    return MultiplyDeBruijnBitPosition[((uint32_t)((v & -v) * 0x077CB531U)) >> 27];
}
#else
{
/* Source: http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious */
    unsigned r = 0;

    while ((v & 0x01) == 0) {
        v >>= 1;
        r++;
    };

    return r;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* BITARITHM_H */
/** @} */

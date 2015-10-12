/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief     Provides integer division functions
 *
 * This header provides some integer division functions that can be used
 * to prevent linking in compiler-generated ones, which are often larger.
 *
 * @file
 * @ingroup   sys_util
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @{
 */

#ifndef DIV_H
#define DIV_H

#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Integer divide val by 15625
 *
 * @pre val <= 16383999997
 *
 * @param[in]   val     dividend
 * @return      (val / 15625)
 */
static inline uint64_t div_u64_by_15625(uint64_t val)
{
    /* a higher value would overflow 2^64 in the multiplication that follows */
    assert(val <= 16383999997LLU);

    return (val * 0x431bde83UL) >> (12 + 32);
}

/**
 * @brief Integer divide val by 1000000
 *
 * @pre val <= 1048575999808
 *
 * @param[in]   val     dividend
 * @return      (val / 1000000)
 */
static inline uint64_t div_u64_by_1000000(uint64_t val)
{
    /* a higher value would overflow 2^64 in the multiplication that follows */
    assert(val <= 1048575999808LLU);

    return div_u64_by_15625(val>>6);
}

/**
 * @brief Divide val by (15625/512)
 *
 * This is used to quantize a 1MHz value to the closest 32768Hz value,
 * e.g., for timers.
 *
 * The algorithm actually multiplies by 512 first, then divides by 15625,
 * keeping the result closer to a floored floating point division.
 *
 * @param[in]   val     dividend
 * @return      (val / (15625/512))
 */
static inline uint32_t div_u32_by_15625div512(uint32_t val)
{
    return ((uint64_t)(val) * 0x431bde83ul) >> (12 + 32 - 9);
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
#endif /* DIV_H */

/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_unaligned unaligned memory access methods
 * @ingroup     sys
 * @brief       Provides functions for safe unaligned memory accesses
 *
 * This header provides functions to read values from pointers that are not
 * necessarily aligned to the type's alignment requirements.
 *
 * E.g.,
 *
 *     uint16_t *foo = 0x123;
 *     printf("%u\n", *foo);
 *
 * ... might cause an unaligned access, if `uint16_t` is usually aligned at
 * 2-byte-boundaries, as foo has an odd address.
 *
 * The current implementation casts a pointer to a packed struct, which forces
 * the compiler to deal with possibly unalignedness.  Idea taken from linux
 * kernel sources.
 *
 * @{
 *
 * @file
 * @brief       Unaligned but safe memory access functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Unaligned access helper struct (uint16_t version) */
typedef struct __attribute__((packed)) {
    uint16_t val;       /**< value */
} uint16_una_t;

/** @brief Unaligned access helper struct (uint32_t version) */
typedef struct __attribute__((packed)) {
    uint32_t val;       /**< value */
} uint32_una_t;

/** @brief Unaligned access helper struct (uint64_t version) */
typedef struct __attribute__((packed)) {
    uint64_t val;       /**< value */
} uint64_una_t;

/**
 * @brief    Get uint16_t from possibly unaligned pointer
 *
 * @param[in]   ptr pointer to read from
 *
 * @returns value read from @p ptr
 */
static inline uint16_t unaligned_get_u16(const void *ptr)
{
    const uint16_una_t *tmp = (const uint16_una_t *)ptr;
    return tmp->val;
}

/**
 * @brief    Get uint32_t from possibly unaligned pointer
 *
 * @param[in]   ptr pointer to read from
 *
 * @returns value read from @p ptr
 */
static inline uint32_t unaligned_get_u32(const void *ptr)
{
    const uint32_una_t *tmp = (const uint32_una_t *)ptr;
    return tmp->val;
}

/**
 * @brief    Get uint64_t from possibly unaligned pointer
 *
 * @param[in]   ptr pointer to read from
 *
 * @returns value read from @p ptr
 */
static inline uint64_t unaligned_get_u64(const void *ptr)
{
    const uint64_una_t *tmp = (const uint64_una_t *)ptr;
    return tmp->val;
}

#ifdef __cplusplus
}
#endif

/** @} */

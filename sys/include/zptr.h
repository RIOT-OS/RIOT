/*
 * SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_util_zptr Pointer Compression
 * @ingroup     sys
 * @brief       Provides 32bit -> 16bit pointer compression
 *
 * On many platforms, some pointers may have to be aligned, e.g., to 4 byte
 * boundaries.
 * On 32bit platforms, that makes it possible to store all possible aligned
 * 32bit pointers in a 16bit value as long as the total memory is small (e.g.,
 * with 4 byte alignment, all pointers within 256kb RAM can be represented by a
 * 16bit value). This can save memory, at the cost of some instructions for
 * compression/decompression.
 *
 * In order to use pointer compression, ZPTR_BASE needs to be defined to a (4
 * byte aligned) base address.
 *
 * A printf format macro (PRIzptr) is provided.
 *
 * You can then use zptr_t instead of a pointer type, using the supplied functions to compress / decompress,
 * e.g.,
 *
 *     void func(void *ptr) {
 *         printf("%"PRIzptr"\n", ptr);
 *         ...
 *         free(ptr);
 * }
 *
 * ... would become
 *
 *     void func(zptr_t zptr);
 *         printf("%"PRIzptr"\n", zptr);
 *         ...
 *         free(zptrd(zptr));
 *     }
 *
 *
 * If ZPTR_BASE is unset, @ref zptr_t / @ref zptrc() / @ref zptrd() will
 * transparently and without overhead compile to normal (uncompressed) pointer
 * operations.
 *
 * @{
 *
 * @file
 * @brief       32bit -> 16bit pointer compression implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#if ZPTR_BASE || defined(DOXYGEN)

/**
 * @brief zptr type definition
 */
typedef uint16_t zptr_t;

/**
 * @brief zptr printf format definition
 */
#define PRIzptr PRIu16

/**
 * @brief zptr highest compressible address
 */
#define ZPTR_MAX_ADDR   ((uintptr_t)ZPTR_BASE + (1 << 18))

/**
 * @brief Determine if a pointer is compressible by zptrc()
 * @param[in]   pointer     pointer to check
 * @returns     1 if pointer can be compressed, 0 if not
 */
static inline int zptr_check(void *pointer)
{
    uintptr_t int_ptr = (uintptr_t)pointer;
    return ((!(int_ptr & 0x3)) \
            && (int_ptr >= (uintptr_t)ZPTR_BASE) \
            && (int_ptr < ZPTR_MAX_ADDR));
}

/**
 * @brief Compress a pointer (if possible)
 *
 * Subtracts ZPTR_BASE, then right-shifts @p pointer by two.
 *
 * @param[in]   pointer     pointer to compress
 * @returns     compressed pointer
 */
static inline zptr_t zptrc(void *pointer)
{
    assert(zptr_check(pointer));
    return (uint16_t)(((uint32_t)pointer - (uint32_t)ZPTR_BASE) >> 2);
}

/**
 * @brief Decompress a pointer
 *
 * Left-shifts zptr_t by two, then adds ZPTR_BASE.
 *
 * @param[in]   zptr    compressed pointer
 * @returns     decompressed pointer
 */
static inline void *zptrd(zptr_t zptr)
{
    return (void *)(ZPTR_BASE + ((uint32_t)zptr << 2));
}

#else /* ZPTR_BASE */
/* fallback implementation */
typedef void *zptr_t;
#define PRIzptr "p"
static inline int zptr_check(void *pointer) { (void)pointer; return 0; }
static inline zptr_t zptrc(void *pointer) { return (zptr_t)pointer; }
static inline void *zptrd(zptr_t zptr) { return (void *)zptr; }
#endif

#ifdef __cplusplus
}
#endif

/** @} */

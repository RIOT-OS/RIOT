/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_architecture    Platform-independent access to architecture
 *                                  details
 * @ingroup     sys
 *
 * This module provides architecture-independent access to architecture details.
 *
 * @{
 *
 * @file
 * @brief       Platform-independent access to architecture details
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <stdint.h>

#include "architecture_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Provide doxygen doc centrally, instead of in every architecture_arch.h */
#ifdef DOXYGEN
/**
 * @brief   Size of a word in bits
 *
 * @details Depending on architecture, this can have a value of 8, 16, or 32
 */
#define ARCHITECTURE_WORD_BITS      <NUM>
/**
 * @brief   Size of a word in bytes
 *
 * @details Depending on architecture, this can have a value or 1, 2, or 4.
 */
#define ARCHITECTURE_WORD_BYTES     <NUM>
/**
 * @brief   Word sized unsigned integer
 *
 * @details Synonym to `uint8_t`, `uint16_t` or `uint32_t` depending on
 *          architecture
 */
typedef uint<num>_t uword_t;
/**
 * @brief   Word sized signed integer
 *
 * @details Synonym to `int8_t`, `int16_t` or `int32_t` depending on
 *          architecture
 *
 * @note    This type is pronounce es-word-tea. When slaying dragons, this is
 *          not the tool you're looking for.
 */
typedef int<num>_t  sword_t;
#elif (ARCHITECTURE_WORD_BITS == 8)
#define ARCHITECTURE_WORD_BYTES     (1U)
typedef uint8_t     uword_t;
typedef int8_t      sword_t;
#elif (ARCHITECTURE_WORD_BITS == 16)
#define ARCHITECTURE_WORD_BYTES     (2U)
typedef uint16_t    uword_t;
typedef int16_t     sword_t;
#elif (ARCHITECTURE_WORD_BITS == 32)
#define ARCHITECTURE_WORD_BYTES     (4U)
typedef uint32_t    uword_t;
typedef int32_t     sword_t;
#else
#error  "Unsupported word size (check ARCHITECTURE_WORD_BITS in architecture_arch.h)"
#endif

/**
 * @brief   Type qualifier to use to align data on word boundaries
 *
 * Use like this:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * char WORD_ALIGNED thread_stack[THREAD_STACKSIZE_DEFAULT];
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define WORD_ALIGNED __attribute__((aligned(ARCHITECTURE_WORD_BYTES)))

/**
 * @brief       Check if @p addr is alignment to @p alignment
 * @param[in]   addr        Address to check for being aligned
 * @param[in]   alignment   Alignment to check for
 * @pre         @p alignment is a power of two. (But this is naturally
 *              fulfilled, as all possible alignment requirements in C
 *              are powers of two.)
 * @retval      1           @p addr is aligned to @p alignment
 * @retval      0           @p addr is unaligned
 */
#define HAS_ALIGNMENT_OF(addr, alignment) (((uintptr_t)(addr) & ((alignment) - 1)) == 0)

/**
 * @brief       Check if @p addr is word-aligned
 * @param[in]   addr        Address to check for word alignment
 * @retval      1           @p addr is word-aligned
 * @retval      0           @p addr is unaligned
 */
#define IS_WORD_ALIGNED(addr) HAS_ALIGNMENT_OF(addr, ARCHITECTURE_WORD_BYTES)

/**
 * @brief   Smallest number an uword_t can hold
 */
#define UWORD_MIN                   0

/**
 * @brief   Highest number an uword_t can hold
 */
#define UWORD_MAX                   ((1ULL << ARCHITECTURE_WORD_BITS) - 1)

/**
 * @brief   Smallest number an sword_t can hold
 */
#define SWORD_MIN                   (-(1LL << (ARCHITECTURE_WORD_BITS - 1)))

/**
 * @brief   Highest number an sword_t can hold
 */
#define SWORD_MAX                   ((1LL << (ARCHITECTURE_WORD_BITS - 1)) - 1)

#ifdef __cplusplus
}
#endif

#endif /* ARCHITECTURE_H */
/** @} */

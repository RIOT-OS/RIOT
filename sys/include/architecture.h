/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

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

#include <stdint.h>
#include <inttypes.h>
#include <limits.h>

#include "architecture_arch.h" /* IWYU pragma: export */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set a breakpoint
 * @warning         If no Debugger is attached, the CPU might get stuck here
 *                  and consume a lot of power until reset.
 * @param[in] value Context value for debugger, usually ignored.
 */
#ifndef ARCHITECTURE_BREAKPOINT
/* If no breakpoint instruction is defined, busy wait for debugger
 * to attach and break to ease backtrace
 */
#define ARCHITECTURE_BREAKPOINT(value)  do {} while (1)
#endif

/* Provide doxygen doc centrally, instead of in every architecture_arch.h */
#ifdef DOXYGEN
/**
 * @brief   Size of a word in bits
 *
 * @details Depending on architecture, this can have a value of 8, 16, 32, or 64
 */
#define ARCHITECTURE_WORD_BITS      <NUM>
/**
 * @brief   Size of a word in bytes
 *
 * @details Depending on architecture, this can have a value or 1, 2, 4, or 8.
 */
#define ARCHITECTURE_WORD_BYTES     <ARCHITECTURE_WORD_BITS / 8>
/**
 * @brief   Word sized unsigned integer
 *
 * @details Synonym to `uint8_t`, `uint16_t`, `uint32_t`, or `uint64_t`
 *          depending on architecture
 */
typedef uint<NUM>_t uword_t;
/**
 * @brief   Word sized signed integer
 *
 * @details Synonym to `int8_t`, `int16_t`, `int32_t`, or `int64_t` depending on
 *          architecture
 *
 * @note    This type is pronounce es-word-tea. When slaying dragons, this is
 *          not the tool you're looking for.
 */
typedef int<NUM>_t  sword_t;
/**
 * @brief   Highest number an sword_t can hold
 */
#define SWORD_MAX   <2^(ARCHITECTURE_WORD_BITS - 1) - 1>
/**
 * @brief   Smallest number an sword_t can hold
 */
#define SWORD_MIN   <-2^(ARCHITECTURE_WORD_BITS - 1)>
/**
 * @brief   Highest number an uword_t can hold
 */
#define UWORD_MAX   <2^ARCHITECTURE_WORD_BITS - 1>

/* end of ifdef DOXYGEN */
#elif (ARCHITECTURE_WORD_BITS == 8)
#define ARCHITECTURE_WORD_BYTES     (1U)
typedef uint8_t     uword_t;
typedef int8_t      sword_t;
#define SWORD_MAX   (INT8_MAX)
#define SWORD_MIN   (INT8_MIN)
#define UWORD_MAX   (UINT8_MAX)
#elif (ARCHITECTURE_WORD_BITS == 16)
#define ARCHITECTURE_WORD_BYTES     (2U)
typedef uint16_t    uword_t;
typedef int16_t     sword_t;
#define SWORD_MAX   (INT16_MAX)
#define SWORD_MIN   (INT16_MIN)
#define UWORD_MAX   (UINT16_MAX)
#elif (ARCHITECTURE_WORD_BITS == 32)
#define ARCHITECTURE_WORD_BYTES     (4U)
typedef uint32_t    uword_t;
typedef int32_t     sword_t;
#define SWORD_MAX   (INT32_MAX)
#define SWORD_MIN   (INT32_MIN)
#define UWORD_MAX   (UINT32_MAX)
#elif (ARCHITECTURE_WORD_BITS == 64)
#define ARCHITECTURE_WORD_BYTES     (8U)
typedef uint64_t    uword_t;
typedef int64_t     sword_t;
#define SWORD_MAX   (INT64_MAX)
#define SWORD_MIN   (INT64_MIN)
#define UWORD_MAX   (UINT64_MAX)
#else
#error  "Unsupported word size (check ARCHITECTURE_WORD_BITS in architecture_arch.h)"
#endif

/**
 * @brief   Smallest number an uword_t can hold
 */
#define UWORD_MIN   (0U)

#if !defined(ARCHITECTURE_LARGE_TXT_PTR) || DOXYGEN
/**
 * @brief   Pointer type to point anywhere in the .text section
 */
typedef uintptr_t   uinttxtptr_t;

/**
 * @brief   Format string macro for text section pointer
 */
#define PRIxTXTPTR PRIxPTR
#endif

#if DOXYGEN
/**
 * @brief   Architecture specific modifier used for printing sizes
 */
#define PRI_SIZE_T_MODIFIER /* implementation defined */
#elif (UINT_MAX == SIZE_MAX)
#define PRI_SIZE_T_MODIFIER ""
#elif (ULONG_MAX == SIZE_MAX)
#define PRI_SIZE_T_MODIFIER "l"
#else
#error Unsupported size_t length
#endif

/**
 * @brief   Macro holding the format specifier to print an `ssize_t` variable
 *          in decimal representation.
 */
#define PRIdSIZE PRI_SIZE_T_MODIFIER "d"
/**
 * @brief   Macro holding the format specifier to print an `ssize_t` variable.
 *
 * Same as @ref PRIdSIZE for output. When used for input (e.g. in `scanf()`),
 * `PRIiSIZE` will also accept hexadecimal and octal numbers if prefixed by
 * `0x` or `0`, respectively.
 */
#define PRIiSIZE PRI_SIZE_T_MODIFIER "i"
/**
 * @brief   Macro holding the format specifier to print an `ssize_t` variable
 *          in octal representation.
 */
#define PRIoSIZE PRI_SIZE_T_MODIFIER "o"
/**
 * @brief   Macro holding the format specifier to print an `size_t` variable
 *          in decimal representation.
 */
#define PRIuSIZE PRI_SIZE_T_MODIFIER "u"
/**
 * @brief   Macro holding the format specifier to print an `size_t` variable
 *          in hexadecimal representation.
 *
 * @details Same as @ref PRIXSIZE for input, but uses lowercase letters for
 *          output (e.g. `2a` for 42).
 */
#define PRIxSIZE PRI_SIZE_T_MODIFIER "x"
/**
 * @brief   Macro holding the format specifier to print an `size_t` variable
 *          in hexadecimal representation.
 *
 * @details Same as @ref PRIxSIZE for input, but uses uppercase letters for
 *          output (e.g. `2A` for 42).
 */
#define PRIXSIZE PRI_SIZE_T_MODIFIER "X"

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

#ifdef __cplusplus
}
#endif

/** @} */

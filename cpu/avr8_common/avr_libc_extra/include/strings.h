/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup cpu_atmega_common
 *
 * This is header file provides `strings.h` that is missing in AVR libc for
 * compatibility.
 *
 * @{
 *
 * @file
 * @brief       strings.h
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <string.h>

#ifndef STRINGS_H
#define STRINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Same as `memset(dest, 0, n_bytes)`, use `memset()` instead
 *
 * @param[in,out]   dest        Memory to clear
 * @param[in]       n_bytes     Size of memory to clear in bytes
 */
static inline void bzero(void *dest, size_t n_bytes)
{
    memset(dest, 0, n_bytes);
}

/**
 * @brief   Same as `memcmp()`, use `memcmp` instead
 *
 * @param[in]   s1      first memory chunk to compare
 * @param[in]   s2      second memory chunk to compare
 * @param[in]   n       number of bytes to compare
 *
 * @retval  `0`     The first @p n bytes of @p s1 and @p s2 are equal
 * @retval  `!=0`   The first @p n bytes of @p s1 and @p s2 differ in at least
 *                  one bit
 */
static inline int bcmp(const void *s1, const void *s2, size_t n)
{
    return memcmp(s1, s2, n);
}

/**
 * @brief   Same as `memmove()`, use `memmove()` or `memcpy()` instead
 *
 * @param[in]   src     memory to copy from
 * @param[out]  dest    memory to copy to
 * @param[in]   n       number of bytes to copy
 *
 * @details @p src and @p dest may overlap
 *
 * @note    It is preferred to use the standardize function `memcpy()` for
 *          non-overlapping memory regions and `memmove()` for overlapping
 *          memory regions instead of this obsolete function.
 */
static inline void bcopy(const void *src, void *dest, size_t n)
{
    memmove(dest, src, n);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* STRINGS_H */

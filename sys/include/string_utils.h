/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_string_utils    Utility functions that are missing in `string.h`
 * @ingroup     sys
 *
 * This header provides utility functions that the standard C libs `string.h`
 * lacks, such as @ref explicit_bzero
 *
 * @{
 *
 * @file
 * @brief       Utility functions that are missing in `string.h`
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <errno.h>
#include <stdint.h>
/* if explicit_bzero() is provided by standard C lib, it may be defined in
 * either `string.h` or `strings.h`, so just include both here */
#include <string.h>
#include <strings.h>
#include <sys/types.h>

#include "modules.h"

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/* explicit_bzero is provided if:
 * - glibc is used as C lib (only with board natvie)
 * - newlib is used and __BSD_VISIBILE is set
 *      - except for ESP8266, which is using an old version of newlib without it
 * - picolibc is used and __BSD_VISIBLE is set
 *
 * for all other cases, we provide it here
 */
#if !defined(CPU_NATIVE) \
    && !(IS_USED(MODULE_PICOLIBC) && __BSD_VISIBLE) \
    && !(IS_USED(MODULE_NEWLIB) && __BSD_VISIBLE && !defined(MCU_ESP8266))

/**
 * @brief   Like `memset(dest, 0, n_bytes)`, but secure
 *
 * Unlike `memset(dest, 0, n_bytes)`, this will zero out the memory even in
 * cases the compiler would optimize out the call to `memset()`.
 *
 * @note    This is only sensible to use for sensitive data. For non-sensitive
 *          data, keep using `memset()` for performance reasons.
 *
 * @param[in,out]   dest        Memory to clear
 * @param[in]       n_bytes     Size of memory to clear in bytes
 */
static inline void explicit_bzero(void *dest, size_t n_bytes)
{
    volatile uint8_t *tmp = dest;
    for (size_t i = 0; i < n_bytes; i++) {
        tmp[i] = 0;
    }
}
#endif

/**
 * @brief Copy the string, or as much of it as fits, into the dest buffer.
 *
 * Preferred to `strncpy` since it always returns a valid string, and doesn't
 * unnecessarily force the tail of the destination buffer to be zeroed.
 * If the zeroing is desired, it's likely cleaner to use `strscpy` with an
 * overflow test, then just memset the tail of the dest buffer.
 *
 * @param[out] dest     Where to copy the string to
 * @param[in]   src     Where to copy the string from
 * @param[in] count     Size of destination buffer
 *
 * @pre       The destination buffer is at least one byte large, as
 *            otherwise the terminating zero byte won't fit
 *
 * @return the number of characters copied (not including the trailing zero)
 * @retval  -E2BIG      the destination buffer wasn't big enough
 */
ssize_t strscpy(char *dest, const char *src, size_t count);

/**
 * @brief   Check if the entire buffer is filled with the same byte.
 *
 * @param[in]   data    The buffer to probe
 * @param[in]   c       The byte to check of
 * @param[in]   len     Size of the buffer
 *
 * @return NULL if the entire buffer is filled with @p c
 * @return pointer to the first non-matching byte
 */
const void *memchk(const void *data, uint8_t c, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* STRING_UTILS_H */
/** @} */

/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_string_utils    Utility functions that are missing in
 *                                  `string.h`
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

/* if explicit_bzero() is provided by standard C lib, it may be defined in
 * either `string.h` or `strings.h`, so just include both here */
#include <stdint.h>
#include <string.h>
#include <strings.h>

#include "kernel_defines.h"

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
#if !defined(BOARD_NATIVE) \
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

#ifdef __cplusplus
}
#endif

#endif /* STRING_UTILS_H */
/** @} */

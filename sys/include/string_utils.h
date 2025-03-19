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

#include <assert.h>
#include <errno.h>
#include <stdint.h>
/* if explicit_bzero() is provided by standard C lib, it may be defined in
 * either `string.h` or `strings.h`, so just include both here */
#include <string.h>
#include <strings.h>
#include <sys/types.h>

#include "flash_utils.h"
#include "modules.h"

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   String Writer structure.
 *          Helper for writing multiple formatted strings to a buffer
 */
typedef struct {
    const char *start;  /**< start of the target buffer */
    char *position;     /**< current write pointer      */
    size_t capacity;    /**< remaining capacity of the buffer */
} string_writer_t;

/**
 * @brief   Initialize a string writer structure
 *
 * @param[out]  sw      String Writer object to initialize
 * @param[in]   buffer  destination buffer
 * @param[in]   len     size of the destination buffer
 */
static inline void string_writer_init(string_writer_t *sw, void *buffer, size_t len)
{
    assert(buffer && len);

    sw->start = buffer;
    sw->position = buffer;
    sw->capacity = len;
    sw->position[0] = 0;
}

/**
 * @brief   Get the size of the string contained by the string writer
 * @param[in]   sw      String Writer to query
 * @return      size of the string
 */
static inline size_t string_writer_len(const string_writer_t *sw)
{
    return sw->position - sw->start;
}

/**
 * @brief   Get the string contained by the string writer
 * @param[in]   sw      String Writer to query
 * @return      the string assembled by string writer
 */
static inline const char *string_writer_str(const string_writer_t *sw)
{
    return sw->start;
}

/**
 * @brief   internal helper macro
 */
#if IS_ACTIVE(HAS_FLASH_UTILS_ARCH)
#define __swprintf flash_swprintf
#else
#define __swprintf swprintf
__attribute__ ((format (printf, 2, 3)))
#endif
/**
 * @brief   Write a formatted string to a buffer
 *          The string will be truncated if there is not enough space left in
 *          the destination buffer.
 *          A terminating `\0` character is always included.
 *
 * @param[in]   sw      String Writer to write to
 * @param[in]   format  format string to write
 *
 * @return      number of bytes written on success
 *              -E2BIG if the string was truncated
 */
int __swprintf(string_writer_t *sw, FLASH_ATTR const char *restrict format, ...);

#if IS_ACTIVE(HAS_FLASH_UTILS_ARCH)
#define swprintf(sw, fmt, ...) flash_swprintf(sw, TO_FLASH(fmt), ## __VA_ARGS__)
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
    && !(IS_USED(MODULE_NEWLIB) && __BSD_VISIBLE && !defined(CPU_ESP8266))

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

/**
 * @brief   Reverse the order of bytes in a buffer
 *
 * @param[in, out]  buf     The buffer to reverse
 * @param[in]       len     Size of the buffer
 */
void reverse_buf(void *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* STRING_UTILS_H */
/** @} */

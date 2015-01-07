/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    oneway_malloc Oneway malloc
 * @ingroup     sys
 *
 * @brief       A malloc implementation without free for boards where the
 *              toolchain does not implement dynamic memory allocation.
 *
 *              The toolchain of MSP-430, for example, does not contain
 *              malloc() and friends.  These functions provide the same
 *              interface as the stdlib functions, but the option to free
 *              memory.
 *
 * @note        You should prefer statically allocated memory whenever possible.
 *
 * @{
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef __MALLOC_H
#define __MALLOC_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Allocation a block of memory.
 * @param[in]   size   Size of the block to allocate in bytes.
 * @returns     The new memory block. `NULL` if the "heap" is exhausted.
 */
void *malloc(size_t size);

/**
 * @brief       Allocated a new block of memory and move the existing content.
 * @details     This function allocates a new block of memory and memcpy()s the content of the ond `ptr` there.
 *
 *              We do not know the size of the old block, so illegal reads would be likely,
 *              if it was not for the fact that the memory heap up.
 * @param[in]   ptr    Old memory block that was allocated with malloc(), calloc() or realloc().
 * @param[in]   size   Size of the new block to allocted in bytes.
 * @returns     The new memory block. `NULL` if the "heap" is exhausted.
 */
void *realloc(void *ptr, size_t size);

/**
 * @brief       Allocate a memory block and set all its content to zeroes.
 * @details     Please see malloc() for more information.
 * @note        This implementation of calloc() does not catch integer overflows
 * @param[in]   size   One factor of the number of bytes to allocated.
 * @param[in]   cnt    The other factor of the number of bytes to allocated.
 * @returns     The new memory block. `NULL` if the "heap" is exhausted.
 */
void *calloc(size_t size, size_t cnt);

/**
 * @brief       This is a no-op.
 * @details     You read correctly: This function does noting.
 * @note        Keep in mind that this function does not free the memory. It does nothing.
 * @param[in]   ptr   The ignored argument.
 */
void free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* __MALLOC_H */

/**
 * @}
 */

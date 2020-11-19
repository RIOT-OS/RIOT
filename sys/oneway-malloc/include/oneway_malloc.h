/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    oneway_malloc Oneway malloc
 * @ingroup     sys_memory_management
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
 * @see         pkg_tlsf
 *
 * @{
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef ONEWAY_MALLOC_H
#define ONEWAY_MALLOC_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Allocation a block of memory that can not be freed.
 *
 * @param[in]   size   Size of the block to allocate in bytes.
 *
 * @returns     The new memory block. `NULL` if the "heap" is exhausted.
 */
void *oneway_malloc(size_t size);

/**
 * @brief       Allocate a memory block and set all its content to zeroes.
 *              The memory block can not be freed.
 *
 * @param[in]   size   One factor of the number of bytes to allocated.
 * @param[in]   cnt    The other factor of the number of bytes to allocated.
 *
 * @returns     The new memory block. `NULL` if the "heap" is exhausted.
 */
void *oneway_calloc(size_t size, size_t cnt);

#ifdef __cplusplus
}
#endif

#endif /* ONEWAY_MALLOC_H */

/**
 * @}
 */

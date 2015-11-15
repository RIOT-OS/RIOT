/*
 * Copyright (C) 2015 James Hollister
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     native_cpu
 *
 * @brief       Malloc header for use with native on OSX since there is no
 *              malloc.h file in the standard include path.
 *
 * @{
 * @file
 *
 * @author      James Hollister <jhollisterjr@gmail.com>
 */

#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Allocate SIZE bytes of memory.
 * @param[in]   size    Size of the block to allocate.
 * @returns     New memory block. NULL if failed to allocate.
 */
extern void *malloc (size_t size);

/**
 * @brief  Allocate NMEMB elements of SIZE bytes each, all initialized to 0.
 * @param[in]   nmemb   Number of elements to be allocated.
 * @param[in]   size    Size of the block to allocate.
 * @returns     New memory block. NULL if failed to allocate.
 */
extern void *calloc (size_t nmemb, size_t size);

/**
 * @brief       Re-allocate the previously allocated block in ptr, making the new
 *              block SIZE bytes long.
 * @param[in]   ptr     Old memory block.
 * @param[in]   size    Size of the new block to allocate.
 * @returns     New memory block. NULL if failed to allocate.
 */
extern void *realloc (void *ptr, size_t size);

/**
 * @brief       Free a block allocated by `malloc', `realloc' or `calloc'.
 * @param[in]   ptr   Memory block that was allocated with 'malloc, 'realloc,
 *              or 'calloc'.
 */
extern void free (void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* malloc.h */

/**
 * @}
 */

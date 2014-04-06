/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file        oneway_malloc.h
 * @brief       Simple malloc wrapper for sbrk
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @detail      Simple malloc implementation for plattforms
 *              without malloc in libc, e.g. msb430.
 */

#ifndef _MALLOC_H_
#define _MALLOC_H_

#include <stddef.h>

/**
 * @brief Allocates a block of `size` bytes of memory
 *
 * @param[in] Size of the memory block, in bytes.
 *
 * @return On success, a pointer to the memory block allocated by the function.
 * @return NULL otherwise.
 */
void *_malloc(size_t size);

/**
 * @brief Changes the size of the memory block pointed to by ptr.
 *
 * @param[in] Pointer to a memory block previously allocated with malloc,
 *            calloc or realloc.
 *
 * @param[in] New size for the memory block, in bytes.
 *
 * @return A pointer to the reallocated memory block, which may be either
 *         the same as ptr or a new location.
 */
void *_realloc(void *ptr, size_t size);

/**
 * @brief Allocates a block of memory for an array of num elements,
 *        each of them size bytes long, and initializes all its bits to zero.
 *
 * @param[in] Number of elements to allocate.
 * @param[in] Size of each element.
 *
 * @return On success, a pointer to the memory block allocated by the function.
 *         If the function failed to allocate the requested block of memory,
 *         a null pointer is returned.
 */
void *_calloc(int size, size_t cnt);

/**
 * @brief  A block of memory previously allocated by a call to malloc,
 *         calloc or realloc is deallocated, making it available again
 *         for further allocations.
 *
 * @param[in] Pointer to a memory block previously allocated with malloc,
 *            calloc or realloc.
 */
void _free(void *ptr);

/** @} */
#endif /* _MALLOC_H_ */

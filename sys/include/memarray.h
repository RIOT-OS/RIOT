/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *               2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_memarray memory array allocator
 * @ingroup     sys_memory_management
 * @brief       memory array allocator
 * @{
 *
 * @brief       pseudo dynamic allocation in static memory arrays
 * @author      Tobias Heider <heidert@nm.ifi.lmu.de>
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Memory pool
 */
typedef struct {
    void *free_data;    /**< memory pool data / head of the free list */
    size_t size;        /**< size of single list element */
} memarray_t;

/**
 * @brief Memory pool element
 *
 * Internal memarray element struct to increase code readability
 *
 * @internal
 */
typedef struct memarray_element {
    struct memarray_element *next;  /**< Pointer to the next element */
} memarray_element_t;

/**
 * @brief Initialize memarray pool with free list
 *
 * @pre `mem != NULL`
 * @pre `data != NULL`
 * @pre `size >= sizeof(void*)`
 * @pre `num != 0`
 *
 * @param[in,out] mem    memarray pool to initialize
 * @param[in]     data   pointer to user-allocated data
 * @param[in]     size   size of a single element in data
 * @param[in]     num    number of elements in @p data
 */
void memarray_init(memarray_t *mem, void *data, size_t size, size_t num);

/**
 * @brief Allocate memory chunk in memarray pool
 *
 * @pre `mem != NULL`
 *
 * @note Allocated structure is not cleared before returned
 *
 * @param[in,out] mem   memarray pool to allocate block in
 *
 * @return pointer to allocated structure, if enough memory was available
 * @return NULL, on failure
 */
static inline void *memarray_alloc(memarray_t *mem)
{
    assert(mem != NULL);

    void *free = mem->free_data;
    if (free) {
        mem->free_data = *((void **)mem->free_data);
    }
    return free;
}

/**
 * @brief Allocate and clear memory chunk in memarray pool
 *
 * @pre `mem != NULL`
 *
 * @param[in,out] mem   memarray pool to allocate block in
 *
 * @return pointer to allocated structure, if enough memory was available
 * @return NULL, on failure
 */
static inline void *memarray_calloc(memarray_t *mem)
{
    void *new = memarray_alloc(mem);
    if (new) {
        memset(new, 0, mem->size);
    }
    return new;
}

/**
 * @brief Free memory chunk in memarray pool
 *
 * @pre `mem != NULL`
 * @pre `ptr != NULL`
 *
 * @param[in,out] mem   memarray pool to free block in
 * @param[in]     ptr   pointer to memarray chunk
 */
static inline void memarray_free(memarray_t *mem, void *ptr)
{
    assert((mem != NULL) && (ptr != NULL));

    memcpy(ptr, &mem->free_data, sizeof(void *));
    mem->free_data = ptr;
}

/**
 * @brief Extend the memarray with a new memory region
 *
 * This function extends the memarray pool with a new memory region. The region
 * must be able to fit the supplied number of elements of the size used when
 * initializing this memarray.
 *
 * @pre `mem != NULL`
 * @pre `data != NULL`
 * @pre `num != 0`
 *
 * @param[in,out] mem   memarray pool to extend
 * @param[in]     data  pointer to user-allocated data
 * @param[in]     num   number of elements in @p data
 */
void memarray_extend(memarray_t *mem, void *data, size_t num);

/**
 * @brief Reduce the memarray space, subtracting the memory pool
 *
 * It is up to the user to free all chunks in the reduced pool. The function
 * will check if all elements in the pool are freed.
 *
 * @param[in,out] mem   memarray pool to reduce
 * @param[in]     data  pointer to the user-allocated data to reduce
 * @param[in]     num   number of elements to reduce the data pool with
 */
int memarray_reduce(memarray_t *mem, void *data, size_t num);

/**
 * @brief Returns the number of blocks available
 *
 * @param[in]   mem     memarray pool
 *
 * @returns             Number of elements available in the memarray pool
 */
size_t memarray_available(memarray_t *mem);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

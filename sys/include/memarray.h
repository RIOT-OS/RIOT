/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_memarray memory array allocator
 * @ingroup     sys
 * @brief       memory array allocator
 * @{
 *
 * @brief       pseudo dynamic allocation in static memory arrays
 * @author      Tobias Heider <heidert@nm.ifi.lmu.de>
 */

#ifndef MEMARRAY_H
#define MEMARRAY_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Memory pool
 */
typedef struct {
    void *free_data;    /**< memory pool data / head of the free list */
    size_t size;        /**< size of single list element */
    size_t num;         /**< max number of elements in list */
} memarray_t;

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
 * @param[in]     num    number of elements in data
 */
void memarray_init(memarray_t *mem, void *data, size_t size, size_t num);

/**
 * @brief Allocate memory chunk in memarray pool
 *
 * @pre `mem != NULL`
 *
 * @param[in,out] mem   memarray pool to allocate block in
 *
 * @return pointer to allocated structure, if enough memory was available
 * @return NULL, on failure
 */
void *memarray_alloc(memarray_t *mem);

/**
 * @brief Free memory chunk in memarray pool
 *
 * @pre `mem != NULL`
 * @pre `ptr != NULL`
 *
 * @param[in,out] mem   memarray pool to free block in
 * @param[in]     ptr   pointer to memarray chunk
 */
void memarray_free(memarray_t *mem, void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* MEMARRAY_H */

/**
 * @}
 */

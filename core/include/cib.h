/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
 * @addtogroup  core_util
 * @{
 *
 * @file
 * @brief       Circular integer buffer interface
 * @details     This structure provides an organizational interface
 *              and combined with an memory array forms a circular buffer.
 *
 * @author      unknown, propably Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef CIB_H
#define CIB_H

#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief circular integer buffer structure
 */
typedef struct {
    unsigned int read_count;    /**< number of (successful) read accesses */
    unsigned int write_count;   /**< number of (successful) write accesses */
    unsigned int mask;          /**< Size of buffer -1, i.e. mask of the bits */
} cib_t;

/**
 * @brief   Initialize cib_t to a given size.
 */
#define CIB_INIT(SIZE) { 0, 0, (SIZE) - 1 }

/**
 * @brief Initialize cib_t to 0 and set size.
 *
 * @param[out] cib      Buffer to initialize.
 *                      Must not be NULL.
 * @param[in]  size     Size of the buffer, must not exceed MAXINT/2.
 *                      Should be equal to 0 or power of 2.
 */
static inline void cib_init(cib_t *__restrict cib, unsigned int size)
{
    /* check if size is a power of 2 by comparing it to its complement */
    assert(!(size & (size - 1)));

    cib_t c = CIB_INIT(size);
    *cib = c;
}

/**
 * @brief Calculates difference between cib_put() and cib_get() accesses.
 *
 * @param[in] cib       the cib_t to check.
 *                      Must not be NULL.
 * @return How often cib_get() can be called before the CIB is empty.
 */
static inline unsigned int cib_avail(cib_t *__restrict cib)
{
    return cib->write_count - cib->read_count;
}

/**
 * @brief Get the index of the next item in buffer.
 *
 * @param[in,out] cib   corresponding *cib* to buffer.
 *                      Must not be NULL.
 * @return index of next item, -1 if the buffer is empty
 */
static inline int cib_get(cib_t *__restrict cib)
{
    unsigned int avail = cib_avail(cib);

    if (avail > 0) {
        return (int) (cib->read_count++ & cib->mask);
    }

    return -1;
}

/**
 * @brief Get index for item in buffer to put to.
 *
 * @param[in,out] cib   corresponding *cib* to buffer.
 *                      Must not be NULL.
 * @return index of item to put to, -1 if the buffer is full
 */
static inline int cib_put(cib_t *__restrict cib)
{
    unsigned int avail = cib_avail(cib);

    /* We use a signed compare, because the mask is -1u for an empty CIB. */
    if ((int) avail <= (int) cib->mask) {
        return (int) (cib->write_count++ & cib->mask);
    }

    return -1;
}

#ifdef __cplusplus
}
#endif

#endif /* CIB_H */
/** @} */

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
 * @file        cib.h
 * @brief       Circular integer buffer interface
 * @details     This structure provides an organizational interface
 *              and combined with an memory array forms a circular buffer.
 *
 * @author      unknown, propably Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __CIB_H
#define __CIB_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief circular integer buffer structure
 */
typedef struct cib_t {
    unsigned int read_count;    /**< count read accesses  */
    unsigned int write_count;   /**< count write accesses */
    unsigned int complement;    /**< hold complement      */
} cib_t;

/**
 * @brief Initialize cib_t to 0 and set size.
 *
 * @param[out] cib      Buffer to initialize.
 *                      Must not be NULL.
 * @param[in]  size     Size of the buffer.
 */
void cib_init(cib_t *cib, unsigned int size);

/**
 * @brief Get the index of the next item in buffer.
 *
 * @param[in,out] cib   corresponding *cib* to buffer.
 *                      Must not be NULL.
 * @return index of next item, -1 on error.
 */
int cib_get(cib_t *cib);

/**
 * @brief Get index for item in buffer to put to.
 *
 * @param[in,out] cib   corresponding *cib* to buffer.
 *                      Must not be NULL.
 * @return index of item to put to, -1 on error.
 */
int cib_put(cib_t *cib);

/**
 * @brief Calculates difference between cib_put() and cib_get() accesses.
 *
 * @param[in] cib       the cib_t to check.
 *                      Must not be NULL.
 * @return Negative number for #cib_get > #cib_put
 * @return 0 for #cib_get == #cib_put
 * @return positive number for #cib_get < #cib_put
 */
int cib_avail(cib_t *cib);

#ifdef __cplusplus
}
#endif

#endif /* __CIB_H */
/** @} */

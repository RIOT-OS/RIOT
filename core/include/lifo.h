/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief   LIFO buffer API, read long description carefully
 * @author  Heiko Will <hwill@inf.fu-berlin.de>
 *
 * @details This LIFO implementation very efficiently handles integer values.
 *          The caveat is that it **can only handle values between 0 and its own
 *          size - 1**. Also it can only handle up to one element of each value.
 *          If you insert a value twice the LIFO will break.
 */

#ifndef LIFO_H
#define LIFO_H

#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Check if the given lifo is empty.
 *
 * @param[in] array The lifo array to check.
 *
 * @return  1, if empty
 * @return  0, otherwise.
 */
static inline int lifo_empty(int *array)
{
    return array[0] == -1;
}

/**
 * @brief               Initialize a lifo array.
 *
 * @pre `array != NULL`
 *
 * @param[in,out] array An array of size *n* + 1. Must not be `NULL`.
 * @param[in] n         Maximum integer value the lifo is able to store.
 */
static inline void lifo_init(int *array, int n)
{
    LOG_DEBUG("lifo_init(%i)\n", n);
    for (int i = 0; i <= n; i++) {
        array[i] = -1;
    }
}

/**
 * @brief               Insert an element into the lifo
 *
 * @pre `array != NULL`
 *
 * @param[in,out] array An integer array of least *i* + 1 size that **does not
 *                      already contain *i***. Must not be `NULL`.
 * @param[in] i         The integer value to store, between 0 and the size of
 *                      the array - 1, must not be stored already.
 *
 */
static inline void lifo_insert(int *array, int i)
{
    LOG_DEBUG("lifo_insert(%i)\n", i);

    int index = i + 1;

#ifdef DEVELHELP
    if ((array[index] != -1) && (array[0] != -1)) {
        LOG_WARNING(
            "lifo_insert: overwriting array[%i] == %i with %i\n\n\n"
            "\t\tThe lifo is broken now.\n\n\n", index,
            array[index], array[0]);
    }
#endif

    array[index] = array[0];
    array[0] = i;
}

/**
 * @brief           Extract the least recently inserted element from the lifo.
 *
 * @pre `array != NULL`
 *
 * @param[in] array An integer array. Must not be NULL.
 *
 * @return          -1, if the lifo is empty.
 * @return          the least recently inserted element, otherwise.
 */
static inline int lifo_get(int *array)
{
    DEBUG("lifo_get\n");
    int head = array[0];

    if (head != -1) {
        array[0] = array[head + 1];
    }

#ifdef DEVELHELP
    /* make sure a double insert does not result in an infinite
     * resource of values */
    array[head + 1] = -1;
#endif

    DEBUG("lifo_get: returning %i\n", head);
    return head;
}

#ifdef __cplusplus
}
#endif

#endif /* LIFO_H */
/** @} */

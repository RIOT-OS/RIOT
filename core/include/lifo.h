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
int lifo_empty(int *array);

/**
 * @brief               Initialize a lifo array.
 *
 * @param[in,out] array An array of size *n* + 1, may not be NULL.
 * @param[in] n         Maximum integer value the lifo is able to store.
 */
void lifo_init(int *array, int n);

/**
 * @brief               Insert an element into the lifo
 *
 * @param[in,out] array An integer array of least *i* + 1 size that **does not
 *                      already contain *i***, may not be NULL.
 * @param[in] i         The integer value to store, between 0 and the size of
 *                      the array - 1, must not be stored already.
 *
 */
void lifo_insert(int *array, int i);

/**
 * @brief           Extract the least recently inserted element from the lifo.
 *
 * @param[in] array An integer array, may not be NULL.
 *
 * @return          -1, if the lifo is empty.
 * @return          the least recently inserted element, otherwise.
 */
int lifo_get(int *array);

#ifdef __cplusplus
}
#endif

#endif /* LIFO_H */
/** @} */

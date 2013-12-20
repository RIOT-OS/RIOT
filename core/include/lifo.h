/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file        lifo.h
 * @brief       LIFO buffer API, read long description carefully
 * @author      probably Kaspar Schleiser
 *
 * @long        This LIFO implementation very efficiently handles
 *              integer values. The caveat is that it can only handle
 *              values between 0 and its own size -1. Also it can only
 *              handle up to one element of each value. If you insert
 *              a value twice the LIFO will break.
 */

#ifndef __LIFO_H
#define __LIFO_H

/**
 * @brief:  check if the given lifo is empty
 * @return: true if empty, false otherwise
 */
int lifo_empty(int *array);

/**
 * @brief:           initialize a lifo array
 *
 * @param array:     an array of int of size n+1
 * @param n:         maximum integer value the lifo is able to store
 */
void lifo_init(int *array, int n);

/**
 * @brief:          insert an element into the lifo
 *
 * @param array:    an integer array of least i+1 size that does not
 *                  already contain i 
 * @param i:        the integer value to store, between 0 and the size
 *                  of the array -1, must not be stored already
 *
 */
void lifo_insert(int *array, int i);

/**
 * @brief:          extract the least recently inserted element from the lifo
 * 
 * @param array:    an integer array
 *
 * @return:         -1 if the lifo is empty, the least recently
 *                  inserted element otherwise
 */
int lifo_get(int *array);

/** @} */
#endif /* __LIFO_H */

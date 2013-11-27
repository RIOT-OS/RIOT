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
 * @brief       LIFO buffer API
 *
 * @author      unknwon
 */

#ifndef __LIFO_H
#define __LIFO_H

int lifo_empty(int *array);
void lifo_init(int *array, int n);
void lifo_insert(int *array, int i);
int lifo_get(int *array);

/** @} */
#endif /* __LIFO_H */

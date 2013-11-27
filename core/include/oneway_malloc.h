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
 * @file        oneway_malloc.h
 * @brief       Malloc interface
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef _MALLOC_H
#define _MALLOC_H

#include <stddef.h>

void *_malloc(size_t size);
void *_realloc(void *ptr, size_t size);
void *_calloc(int size, size_t cnt);
void _free(void *ptr);

/** @} */
#endif /* _MALLOC_H */

/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup oneway_malloc
 * @ingroup sys
 * @{
 *
 * @file
 * @brief       Simple malloc wrapper for SBRK

 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <string.h>

#include "malloc.h"

#include "debug.h"

extern void *sbrk(int incr);

void __attribute__((weak)) *malloc(size_t size)
{
    if (size != 0) {
        void *ptr = sbrk(size);

        DEBUG("malloc(): allocating block of size %u at %p.\n", (unsigned int) size, ptr);

        if (ptr != (void*) -1) {
            return ptr;
        }
    }
    return NULL;
}

void __attribute__((weak)) *realloc(void *ptr, size_t size)
{
    if (ptr == NULL) {
        return malloc(size);
    }
    else if (size == 0) {
        free(ptr);
        return NULL;
    }
    else {
        void *newptr = malloc(size);
        if (newptr) {
            memcpy(newptr, ptr, size);
        }
        return newptr;
    }
}

void __attribute__((weak)) *calloc(size_t size, size_t cnt)
{
    void *mem = malloc(size * cnt);
    if (mem) {
        memset(mem, 0, size * cnt);
    }
    return mem;
}

void __attribute__((weak)) free(void *ptr)
{
    /* who cares about pointers? */
    (void) ptr;

    DEBUG("free(): block at %p lost.\n", ptr);
}

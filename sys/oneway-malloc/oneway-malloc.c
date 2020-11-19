/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup oneway_malloc
 * @{
 *
 * @file
 * @brief       Simple malloc wrapper for SBRK

 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <string.h>
#include "architecture.h"
#include "oneway_malloc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef ARCHITECTURE_WORD_MASK
#define ARCHITECTURE_WORD_MASK (ARCHITECTURE_WORD_BYTES - 1)
#endif

extern void *sbrk(int incr);

void *oneway_malloc(size_t size)
{
    /* ensure we always allocate word-aligned blocks */
    if (size & ARCHITECTURE_WORD_MASK) {
        size += ARCHITECTURE_WORD_BYTES - (size & ARCHITECTURE_WORD_MASK);
    }

    if (size != 0) {
        void *ptr = sbrk(size);

        DEBUG("malloc(): allocating block of size %u at %p.\n",
              (unsigned int)size, ptr);

        if (ptr != (void *)-1) {
            return ptr;
        }
    }
    return NULL;
}

void *oneway_calloc(size_t size, size_t cnt)
{
    /* ensure size * cnt doesn't overflow size_t */
    if (cnt && size > (size_t)-1 / cnt) {
        return NULL;
    }

    void *mem = oneway_malloc(size * cnt);
    if (mem) {
        memset(mem, 0, size * cnt);
    }
    return mem;
}

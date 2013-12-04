/**
 * simple malloc wrapper for sbrk
 *
 * Needed on platforms without malloc in libc, e.g. msb430
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup kernel
 * @{
 * @file
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ENABLE_DEBUG    (0)
#include <debug.h>

extern void *sbrk(int incr);

void *_malloc(size_t size)
{
    void *ptr = sbrk(size);

    DEBUG("_malloc(): allocating block of size %u at 0x%X.\n", (unsigned int) size, (unsigned int)ptr);

    if (ptr != (void*) - 1) {
        return ptr;
    }
    else {
        return NULL;
    }
}

void *_realloc(void *ptr, size_t size)
{
    void *newptr = _malloc(size);
    memcpy(newptr, ptr, size);
    free(ptr);
    return newptr;
}

void *_calloc(int size, size_t cnt)
{
    void *mem = _malloc(size * cnt);
    if (mem) {
        memset(mem, 0, size * cnt);
    }
    return mem;
}

void _free(void *ptr)
{
    /* who cares about pointers? */
    (void) ptr;

    DEBUG("_free(): block at 0x%X lost.\n", (unsigned int)ptr);
}


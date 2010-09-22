/**
 * simple malloc wrapper for sbrk
 *
 * Needed on platforms without malloc in libc, e.g. msb430
 *
 * Copyright (C) 2010 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup kernel
 * @{
 * @file
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <sys/unistd.h>

#define ENABLE_DEBUG
#include <debug.h>

extern void *sbrk(int incr);

void *_malloc(size_t size) {
    void* ptr = sbrk(size);
    
    DEBUG("_malloc(): allocating block of size %u at 0x%X.\n", size, (unsigned int)ptr);
    
    if (ptr != (void*)-1) {
        return ptr;
    } else {
        return NULL;
    }
}

void _free(void* ptr) {
    DEBUG("_free(): block at 0x%X lost.\n", (unsigned int)ptr);
}


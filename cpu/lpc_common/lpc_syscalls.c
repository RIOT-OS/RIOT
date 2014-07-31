/*
 * syscalls.c - MCU dependent syscall implementation for LPCXXXX
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <errno.h>
#include <stdint.h>
#include <sys/unistd.h>
#include <stdio.h>
#include "irq.h"

/**
 * @name Heaps (defined in linker script)
 * @{
 */
#define NUM_HEAPS   3

extern uintptr_t __heap1_start;     ///< start of heap memory space
extern uintptr_t __heap1_max;       ///< maximum for end of heap memory space
extern uintptr_t __heap2_start;     ///< start of heap memory space
extern uintptr_t __heap2_max;       ///< maximum for end of heap memory space
extern uintptr_t __heap3_start;     ///< start of heap memory space
extern uintptr_t __heap3_max;       ///< maximum for end of heap memory space

/// current position in heap
static caddr_t heap[NUM_HEAPS] = {(caddr_t)&__heap1_start,(caddr_t)&__heap3_start,(caddr_t)&__heap2_start}; // add heap3 before heap2 cause Heap3 address is lower then addr of heap2
/// maximum position in heap
static const caddr_t heap_max[NUM_HEAPS] = {(caddr_t)&__heap1_max,(caddr_t)&__heap3_max,(caddr_t)&__heap2_max};
// start position in heap
static const caddr_t heap_start[NUM_HEAPS] = {(caddr_t)&__heap1_start,(caddr_t)&__heap3_start,(caddr_t)&__heap2_start};

/** @} */

/*-----------------------------------------------------------------------------------*/
void heap_stats(void)
{
    for(int i = 0; i < NUM_HEAPS; i++)
        printf("# heap %i: %p -- %p -> %p (%li of %li free)\n", i, heap_start[i], heap[i], heap_max[i],
            (uint32_t)heap_max[i] - (uint32_t)heap[i], (uint32_t)heap_max[i] - (uint32_t)heap_start[i]);
}

/*-----------------------------------------------------------------------------------*/
caddr_t _sbrk_r(struct _reent *r, size_t incr)
{
    uint32_t cpsr = disableIRQ();

    /* check all heaps for a chunk of the requested size */
    for (volatile uint8_t iUsedHeap = 0; iUsedHeap < NUM_HEAPS; iUsedHeap++ ) {
        caddr_t new_heap = heap[iUsedHeap] + incr;

        if( new_heap <= heap_max[iUsedHeap] ) {
            caddr_t prev_heap = heap[iUsedHeap];
            heap[iUsedHeap] = new_heap;

            r->_errno = 0;
            restoreIRQ(cpsr);
            return prev_heap;
        }
    }
    restoreIRQ(cpsr);

    r->_errno = ENOMEM;
    return NULL;
}

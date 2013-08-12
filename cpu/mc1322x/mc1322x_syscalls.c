/*
 * syscalls.c - MCU dependent syscall implementation for mc1322x
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 *
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
extern uintptr_t __heap_start;     ///< start of heap memory space
extern uintptr_t __heap_end;       ///< maximum for end of heap memory space

/// current position in heap
static caddr_t heap = (caddr_t)&__heap_start;
/// maximum position in heap
static const caddr_t heap_max = (caddr_t)&__heap_end;
// start position in heap
static const caddr_t heap_start = (caddr_t)&__heap_start;

/** @} */

/*-----------------------------------------------------------------------------------*/
void heap_stats(void) 
{
    printf("# heap: %p -- %p -> %p (%li of %li free)\n", heap_start, heap, heap_max,
            (uint32_t)heap_max - (uint32_t)heap, (uint32_t)heap_max - (uint32_t)heap_start);
}

/*-----------------------------------------------------------------------------------*/
caddr_t _sbrk_r(struct _reent *r, size_t incr)
{
    uint32_t cpsr = disableIRQ();

    /* check heap for a chunk of the requested size */
    caddr_t new_heap = heap + incr;

#ifdef MODULE_TRACELOG
    trace_pointer(TRACELOG_EV_MEMORY, heap);
#endif
    if( new_heap <= heap_max ) {
        caddr_t prev_heap = heap;
#ifdef MODULE_TRACELOG
        trace_pointer(TRACELOG_EV_MEMORY, new_heap);
#endif
        heap = new_heap;

        r->_errno = 0;
        restoreIRQ(cpsr);
        return prev_heap;
    }
    restoreIRQ(cpsr);
#ifdef MODULE_TRACELOG
    trace_string(TRACELOG_EV_MEMORY, "heap!");                                  // heap full
#endif

    r->_errno = ENOMEM;
    return NULL;
}
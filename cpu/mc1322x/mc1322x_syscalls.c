/*
 * mc1322x_syscalls.c - MCU dependent syscall implementation
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include "irq.h"

extern uintptr_t __heap_start;     ///< start of heap memory space
extern uintptr_t __heap_end;       ///< maximum for end of heap memory space

/// current position in heap
static caddr_t heap = (caddr_t)&__heap_start;
/// maximum position in heap
static const caddr_t heap_max = (caddr_t)&__heap_end;
// start position in heap
static const caddr_t heap_start = (caddr_t)&__heap_start;

/*-----------------------------------------------------------------------------------*/
caddr_t _sbrk_r(struct _reent *r, size_t incr)
{
    uint32_t cpsr = disableIRQ();

    /* check all heaps for a chunk of the requested size */
    caddr_t new_heap = heap + incr;

    if( new_heap <= heap_max ) {
        caddr_t prev_heap = heap;
        heap = new_heap;

        r->_errno = 0;
        restoreIRQ(cpsr);
        return prev_heap;
    }
    restoreIRQ(cpsr);

    r->_errno = ENOMEM;
    return NULL;
}

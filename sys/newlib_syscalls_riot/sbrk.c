/*
 * Copyright (C) 2019 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup    sys_newlib_syscalls_sbrk
 * @{
 *
 * @file
 * @brief       Implement data segment resizing
 *
 * Simple implementation of the sbrk function. This is usually needed by malloc
 * implementations to extend the heap.
 *
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

 /**
 * @brief Allocate memory from the heap.
 *
 * The current heap implementation is very rudimentary, it is only able to allocate
 * memory. But it does not have any means to free memory again
 *
 * @return      pointer to the newly allocated memory on success
 * @return      pointer set to address `-1` on failure
 */

#include <reent.h>
#include <errno.h>
#include <stdint.h>

#include "irq.h"

/**
 * @brief manage the heap
 */
extern char _sheap;                 /* start of the heap */
extern char _eheap;                 /* end of the heap */
char *heap_top = &_sheap + 4;

void *_sbrk_r(struct _reent *r, ptrdiff_t incr)
{
    unsigned int state = irq_disable();
    void *res = heap_top;

    if ((heap_top + incr > &_eheap) || (heap_top + incr < &_sheap)) {
        r->_errno = ENOMEM;
        res = (void *)-1;
    }
    else {
        heap_top += incr;
    }

    irq_restore(state);
    return res;
}

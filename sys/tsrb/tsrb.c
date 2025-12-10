/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys
 * @{
 * @file
 * @brief       thread-safe ringbuffer implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "irq.h"
#include "tsrb.h"
#include "turb.h"

int tsrb_get_one(tsrb_t *rb)
{
    unsigned irq_state = irq_disable();
    int retval = turb_get_one(rb);
    irq_restore(irq_state);
    return retval;
}

int tsrb_peek_one(tsrb_t *rb)
{
    unsigned irq_state = irq_disable();
    int retval = turb_peek_one(rb);
    irq_restore(irq_state);
    return retval;
}

int tsrb_get(tsrb_t *rb, uint8_t *dst, size_t n)
{
    unsigned irq_state = irq_disable();
    int cnt = turb_get(rb, dst, n);
    irq_restore(irq_state);
    return cnt;
}

int tsrb_peek(tsrb_t *rb, uint8_t *dst, size_t n)
{
    unsigned irq_state = irq_disable();
    int idx = turb_peek(rb, dst, n);
    irq_restore(irq_state);
    return idx;
}

int tsrb_drop(tsrb_t *rb, size_t n)
{
    unsigned irq_state = irq_disable();
    int cnt = turb_drop(rb, n);
    irq_restore(irq_state);
    return cnt;
}

int tsrb_add_one(tsrb_t *rb, uint8_t c)
{
    unsigned irq_state = irq_disable();
    int retval = turb_add_one(rb, c);
    irq_restore(irq_state);
    return retval;
}

int tsrb_add(tsrb_t *rb, const uint8_t *src, size_t n)
{
    unsigned irq_state = irq_disable();
    int cnt = turb_add(rb, src, n);
    irq_restore(irq_state);
    return cnt;
}

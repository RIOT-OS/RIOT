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

static void _push(tsrb_t *rb, uint8_t c)
{
    rb->buf[rb->writes++ & (rb->size - 1)] = c;
}

static uint8_t _pop(tsrb_t *rb)
{
    return rb->buf[rb->reads++ & (rb->size - 1)];
}

static uint8_t _peek(tsrb_t *rb, unsigned int idx)
{
    return rb->buf[(rb->reads + idx) & (rb->size - 1)];
}

int tsrb_get_one(tsrb_t *rb)
{
    int retval = -1;
    unsigned irq_state = irq_disable();
    if (!tsrb_empty(rb)) {
        retval = _pop(rb);
    }
    irq_restore(irq_state);
    return retval;
}

int tsrb_peek_one(tsrb_t *rb)
{
    int retval = -1;
    unsigned irq_state = irq_disable();
    if (!tsrb_empty(rb)) {
        retval = _peek(rb, 0);
    }
    irq_restore(irq_state);
    return retval;
}

int tsrb_get(tsrb_t *rb, uint8_t *dst, size_t n)
{
    size_t tmp = n;
    unsigned irq_state = irq_disable();
    while (tmp && !tsrb_empty(rb)) {
        *dst++ = _pop(rb);
        tmp--;
    }
    irq_restore(irq_state);
    return (n - tmp);
}

int tsrb_peek(tsrb_t *rb, uint8_t *dst, size_t n)
{
    size_t idx = 0;
    unsigned irq_state = irq_disable();
    unsigned int avail = tsrb_avail(rb);
    while (idx < n && idx != avail) {
        *dst++ = _peek(rb, idx++);
    }
    irq_restore(irq_state);
    return idx;
}

int tsrb_drop(tsrb_t *rb, size_t n)
{
    size_t tmp = n;
    unsigned irq_state = irq_disable();
    while (tmp && !tsrb_empty(rb)) {
        _pop(rb);
        tmp--;
    }
    irq_restore(irq_state);
    return (n - tmp);
}

int tsrb_add_one(tsrb_t *rb, uint8_t c)
{
    int retval = -1;
    unsigned irq_state = irq_disable();
    if (!tsrb_full(rb)) {
        _push(rb, c);
        retval = 0;
    }
    irq_restore(irq_state);
    return retval;
}

int tsrb_add(tsrb_t *rb, const uint8_t *src, size_t n)
{
    size_t tmp = n;
    unsigned irq_state = irq_disable();
    while (tmp && !tsrb_full(rb)) {
        _push(rb, *src++);
        tmp--;
    }
    irq_restore(irq_state);
    return (n - tmp);
}

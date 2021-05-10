/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_rb
 * @{
 *
 * @file
 * @brief       Ring buffer implementation
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "irq.h"
#include "rb.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _is_power_of_two(uint16_t val)
{
    return !(val & (val - 1));
}

void rb_base_init(rb_base_t *rb, void *buffer, size_t buffer_size,
                  size_t read_align, size_t write_align)
{
    assert((rb != NULL) && (buffer != NULL)
           && (buffer_size > read_align) && (buffer_size > write_align)
           && (buffer_size <= UINT16_MAX) && _is_power_of_two(buffer_size)
           && _is_power_of_two(read_align) && _is_power_of_two(write_align));

    memset(rb, 0, sizeof(*rb));
    rb->buf = buffer;
    rb->size = buffer_size;

    if (read_align == 0) {
        rb->r_align = 0xffff;
    }
    else {
        rb->r_align = ~(read_align - 1);
    }

    if (write_align == 0) {
        rb->w_align = 0xffff;
    }
    else {
        rb->w_align = ~(write_align - 1);
    }
}

size_t rb_base_read_start(rb_base_t *rb, uintptr_t *chunk, size_t max_size)
{
    assert((rb) && (max_size & rb->r_align) && (rb->n_readers < UINT8_MAX));
    unsigned irq_state = irq_disable();
    uint16_t free;
    if (rb->w_done > rb->r_next) {
        free = rb->w_done - rb->r_next;
    }
    else {
        free = rb->size - rb->r_next;
    }

    if (free > max_size) {
        free = max_size;
    }
    free &= rb->r_align;
    if (free > 0) {
        *chunk = (uintptr_t)rb->buf + rb->r_next;
        rb->r_next += free;
        if (rb->r_next >= rb->size) {
            rb->r_next = 0;
        }
        rb->n_readers++;
    }
    irq_restore(irq_state);
    return free;
}

void rb_base_read_done(rb_base_t *rb)
{
    assert((rb) && (rb->n_readers > 0));
    unsigned irq_state = irq_disable();

    if (--rb->n_readers == 0) {
        rb->r_done = rb->r_next;
    }

    irq_restore(irq_state);
}

size_t rb_base_write_start(rb_base_t *rb, uintptr_t *chunk, size_t max_size)
{
    assert((rb) && (max_size & rb->w_align) && (rb->n_writers < UINT8_MAX));
    unsigned irq_state = irq_disable();
    uint16_t free;
    if (rb->r_done > rb->w_next) {
        free = rb->r_done - rb->w_next;
    }
    else {
        free = rb->size - rb->w_next;
    }

    if (free > max_size) {
        free = max_size;
    }
    free &= rb->w_align;
    if (free > 0) {
        *chunk = (uintptr_t)rb->buf + rb->r_next;
        rb->r_next += free;
        if (rb->r_next >= rb->size) {
            rb->r_next = 0;
        }
        rb->n_writers++;
    }
    irq_restore(irq_state);
    return free;
}

void rb_base_write_done(rb_base_t *rb)
{
    assert((rb) && (rb->n_writers > 0));
    unsigned irq_state = irq_disable();

    if (--rb->n_writers == 0) {
        rb->w_done = rb->w_next;
    }

    irq_restore(irq_state);
}

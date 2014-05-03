/**
 * Ringbuffer implementation
 *
 * Copyright (C) 2013 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sys_lib
 * @{
 * @file   ringbuffer.c
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include "ringbuffer.h"

void ringbuffer_init(ringbuffer_t *restrict rb, char *buffer, unsigned bufsize)
{
    rb->buf = buffer;
    rb->start = 0;
    rb->end = 0;
    rb->size = bufsize;
    rb->avail = 0;
}

void ringbuffer_add(ringbuffer_t *restrict rb, const char *buf, unsigned n)
{
    for (unsigned i = 0; i < n; i++) {
        ringbuffer_add_one(rb, buf[i]);
    }
}

void ringbuffer_add_one(ringbuffer_t *restrict rb, char c)
{
    if (rb->avail == rb->size) {
        ringbuffer_get_one(rb);
    }

    rb->buf[rb->end++] = c;

    if (rb->end >= rb->size) {
        rb->end = 0;
    }

    rb->avail++;
}

int ringbuffer_get_one(ringbuffer_t *restrict rb)
{
    if (rb->avail == 0) {
        return -1;
    }

    rb->avail--;

    int c = (char)rb->buf[rb->start++];

    if (rb->start >= rb->size) {
        rb->start = 0;
    }

    return c;
}

unsigned ringbuffer_get(ringbuffer_t *restrict rb, char *buf, unsigned n)
{
    unsigned count = 0;

    while (rb->avail && (count < n)) {
        buf[count++] = ringbuffer_get_one(rb);
    }

    return count;
}

int ringbuffer_peek_one(const ringbuffer_t *restrict rb_)
{
    ringbuffer_t rb = *rb_;
    return ringbuffer_get_one(&rb);
}

unsigned ringbuffer_peek(const ringbuffer_t *restrict rb_, char *buf, unsigned n)
{
    ringbuffer_t rb = *rb_;
    return ringbuffer_get(&rb, buf, n);
}

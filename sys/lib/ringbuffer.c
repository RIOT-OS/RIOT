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
 * @author René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include "ringbuffer.h"

void ringbuffer_init(ringbuffer_t *restrict rb, char *buffer, unsigned bufsize)
{
    rb->buf = buffer;
    rb->size = bufsize;
    rb->start = 0;
    rb->avail = 0;
}

static void add_tail(ringbuffer_t *restrict rb, char c)
{
    unsigned pos = rb->start + rb->avail++;
    if (pos >= rb->size) {
        pos -= rb->size;
    }
    rb->buf[pos] = c;
}

static char get_head(ringbuffer_t *restrict rb)
{
    char result = rb->buf[rb->start];
    if ((--rb->avail == 0) || (++rb->start == rb->size)) {
        rb->start = 0;
    }
    return result;
}

void ringbuffer_add(ringbuffer_t *restrict rb, const char *buf, unsigned n)
{
    for (unsigned i = 0; i < n; i++) {
        ringbuffer_add_one(rb, buf[i]);
    }
}

void ringbuffer_add_one(ringbuffer_t *restrict rb, char c)
{
    if (ringbuffer_full(rb)) {
        get_head(rb);
    }
    add_tail(rb, c);
}

int ringbuffer_get_one(ringbuffer_t *restrict rb)
{
    if (!ringbuffer_empty(rb)) {
        return (unsigned char) get_head(rb);
    }
    else {
        return -1;
    }
}

unsigned ringbuffer_get(ringbuffer_t *restrict rb, char *buf, unsigned n)
{
    if (n > rb->avail) {
        n = rb->avail;
    }

    for (unsigned i = 0; i < n; ++i) {
        buf[i] = get_head(rb);
    }
    return n;
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

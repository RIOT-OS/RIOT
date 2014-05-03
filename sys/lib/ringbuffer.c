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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#if (defined(__MACH__) || defined(__FreeBSD__))
#include <stdlib.h>
#else
#include "malloc.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "ringbuffer.h"

void ringbuffer_init(ringbuffer_t *rb, char *buffer, unsigned int bufsize)
{
    rb->buf = buffer;
    rb->start = 0;
    rb->end = 0;
    rb->size = bufsize;
    rb->avail = 0;
}

int rb_add_elements(ringbuffer_t *rb, const char *buf, int n)
{
    int i = 0;
    while ((i < n) && (rb->avail < rb->size)) {
        rb_add_element(rb, buf[i++]);
    }
    return i;
}

void rb_add_element(ringbuffer_t *rb, char c)
{
    if (rb->avail == rb->size) {
        rb_get_element(rb);
    }

    rb->buf[rb->end++] = c;

    if (rb->end >= rb->size) {
        rb->end = 0;
    }

    rb->avail++;
}

int rb_get_element(ringbuffer_t *rb)
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

int rb_get_elements(ringbuffer_t *rb, char *buf, int n)
{
    int count = 0;

    while (rb->avail && (count < n)) {
        buf[count++] = rb_get_element(rb);
    }

    return count;
}

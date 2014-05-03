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

void ringbuffer_add(ringbuffer_t *rb, char *buf, int n)
{
    for (int i = 0; i < n; i++) {
        ringbuffer_add_one(rb, buf[i]);
    }
}

void ringbuffer_add_one(ringbuffer_t *rb, char c)
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

int ringbuffer_get_one(ringbuffer_t *rb)
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

int ringbuffer_get(ringbuffer_t *rb, char *buf, int n)
{
    int count = 0;

    while (rb->avail && (count < n)) {
        buf[count++] = ringbuffer_get_one(rb);
    }

    return count;
}

/*
int main(int argc, char *argv[] ){
    ringbuffer r;
    char buffer[5];
    ringbuffer_init(&r, buffer, sizeof(buffer));

    ringbuffer_add_one(&r, 1);
    ringbuffer_add_one(&r, 2);
    ringbuffer_add_one(&r, 3);
    ringbuffer_add_one(&r, 4);
    ringbuffer_add_one(&r, 5);
    ringbuffer_add_one(&r, 6);
    ringbuffer_add_one(&r, 7);
    ringbuffer_add_one(&r, 8);
    ringbuffer_add_one(&r, 9);
    ringbuffer_add_one(&r, 10);

    int c;
    while ( r.avail ) {
        c = ringbuffer_get_one(&r);
        if (c == -1) break;
        printf("c=%i\n", (int)c);
    }

    ringbuffer_add_one(&r, 1);
    ringbuffer_add_one(&r, 2);
    ringbuffer_add_one(&r, 3);
    ringbuffer_add_one(&r, 4);
    ringbuffer_add_one(&r, 5);

    char buffer2[10];

    int n = ringbuffer_get(&r, buffer2, sizeof(buffer2));

    for (int i = 0; i < n; i++) {
        printf("%i\n", buffer2[i]);
    }

    ringbuffer_add_one(&r, 1);
    ringbuffer_add_one(&r, 2);
    ringbuffer_add_one(&r, 3);
    ringbuffer_add_one(&r, 4);
    ringbuffer_add_one(&r, 5);
    ringbuffer_add_one(&r, 6);
    ringbuffer_add_one(&r, 7);
    ringbuffer_add_one(&r, 8);
    ringbuffer_add_one(&r, 9);
    ringbuffer_add_one(&r, 10);

    while ( r.avail ) {
        c = ringbuffer_get_one(&r);
        if (c == -1) break;
        printf("c=%i\n", (int)c);
    }

    return 0;
}*/

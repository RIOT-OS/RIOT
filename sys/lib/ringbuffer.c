/**
 * Ringbuffer implementation
 *
 * Copyright (C) 2013 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sys_lib
 * @{
 * @file   ringbuffer.c
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
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

#include "ringbuffer.h"

//#define DEBUG(...) printf (__VA_ARGS__)
#define DEBUG(...)

void ringbuffer_init(ringbuffer_t *rb, char *buffer, unsigned int bufsize)
{
    rb->buf = buffer;
    rb->start = 0;
    rb->end = 0;
    rb->size = bufsize;
    rb->avail = 0;
}

void rb_add_elements(ringbuffer_t *rb, char *buf, int n)
{
    for(int i = 0; i < n; i++) {
        rb_add_element(rb, buf[i]);
    }
}

void rb_add_element(ringbuffer_t *rb, char c)
{
    if(rb->avail == rb->size) {
        rb_get_element(rb);
    }

    rb->buf[rb->end++] = c;

    if(rb->end >= rb->size) {
        rb->end = 0;
    }

    rb->avail++;
}

int rb_get_element(ringbuffer_t *rb)
{
    if(rb->avail == 0) {
        return -1;
    }

    rb->avail--;

    int c = (char)rb->buf[rb->start++];

    if(rb->start >= rb->size) {
        rb->start = 0;
    }

    return c;
}

int rb_get_elements(ringbuffer_t *rb, char *buf, int n)
{
    int count = 0;

    while(rb->avail && (count < n)) {
        buf[count++] = rb_get_element(rb);
    }

    return count;
}

/*
int main(int argc, char *argv[] ){
    ringbuffer r;
    char buffer[5];
    ringbuffer_init(&r, buffer, sizeof(buffer));

    rb_add_element(&r, 1);
    rb_add_element(&r, 2);
    rb_add_element(&r, 3);
    rb_add_element(&r, 4);
    rb_add_element(&r, 5);
    rb_add_element(&r, 6);
    rb_add_element(&r, 7);
    rb_add_element(&r, 8);
    rb_add_element(&r, 9);
    rb_add_element(&r, 10);

    int c;
    while ( r.avail ) {
        c = rb_get_element(&r);
        if (c == -1) break;
        printf("c=%i\n", (int)c);
    }

    rb_add_element(&r, 1);
    rb_add_element(&r, 2);
    rb_add_element(&r, 3);
    rb_add_element(&r, 4);
    rb_add_element(&r, 5);

    char buffer2[10];

    int n = rb_get_elements(&r, buffer2, sizeof(buffer2));

    for (int i = 0; i < n; i++) {
        printf("%i\n", buffer2[i]);
    }

    rb_add_element(&r, 1);
    rb_add_element(&r, 2);
    rb_add_element(&r, 3);
    rb_add_element(&r, 4);
    rb_add_element(&r, 5);
    rb_add_element(&r, 6);
    rb_add_element(&r, 7);
    rb_add_element(&r, 8);
    rb_add_element(&r, 9);
    rb_add_element(&r, 10);

    while ( r.avail ) {
        c = rb_get_element(&r);
        if (c == -1) break;
        printf("c=%i\n", (int)c);
    }

    return 0;
}*/

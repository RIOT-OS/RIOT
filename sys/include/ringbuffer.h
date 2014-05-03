/**
 * Ringbuffer header
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
 * @file   ringbuffer.h
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @author René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

typedef struct ringbuffer {
    char *buf;
    unsigned int        start;
    unsigned int        end;
    unsigned int        size;
    unsigned int        avail;
} ringbuffer_t;

void ringbuffer_init(ringbuffer_t *rb, char *buffer, unsigned int bufsize);
void ringbuffer_add_one(ringbuffer_t *rb, char c);
void ringbuffer_add(ringbuffer_t *rb, char *buf, int n);
int ringbuffer_get_one(ringbuffer_t *rb);
int ringbuffer_get(ringbuffer_t *rb, char *buf, int n);

static inline int ringbuffer_empty(ringbuffer_t *rb)
{
    return rb->avail == 0;
}

static inline int ringbuffer_full(ringbuffer_t *rb)
{
    return rb->avail == rb->size;
}

int ringbuffer_peek_one(ringbuffer_t *rb);
int ringbuffer_peek(ringbuffer_t *rb, char *buf, unsigned n);

#endif /* __RINGBUFFER_H */

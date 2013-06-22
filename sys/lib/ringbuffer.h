/**
 * Ringbuffer header 
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
 * @file   ringbuffer.h
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
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
void rb_add_element(ringbuffer_t *rb, char c);
void rb_add_elements(ringbuffer_t *rb, char *buf, int n);
int rb_get_element(ringbuffer_t *rb);
int rb_get_elements(ringbuffer_t *rb, char *buf, int n);

#endif /* __RINGBUFFER_H */

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
 * @}
 */

#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

/**
 * @brief     Ringbuffer.
 * @details   Non thread-safe FIFO ringbuffer implementation around a `char` array.
 */
typedef struct ringbuffer {
    char *buf;          /**< Buffer to operate on. */
    unsigned int start; /**< Current position in the ring buffer */
    unsigned int end;   /**< Position after the last available element. */
    unsigned int size;  /**< Size of buf. */
    unsigned int avail; /**< Number of elements available for reading. */
} ringbuffer_t;

/**
 * @brief        Initialize a ringbuffer.
 * @param[out]   rb        Datum to initialize.
 * @param[in]    buffer    Buffer to use by rb.
 * @param[in]    bufsize   `sizeof (buffer)`
 */
void ringbuffer_init(ringbuffer_t *rb, char *buffer, unsigned int bufsize);

/**
 * @brief           Add an element to the ringbuffer.
 * @details         If rb is full, then the oldest element gets overwritten.
 *                  Compare `rb->size < rb->avail` if that is not overwring is not intended.
 * @param[in,out]   rb   Ringbuffer to operate on.
 * @param[in]       c    Element to add.
 */
void rb_add_element(ringbuffer_t *rb, char c);

/**
 * @brief           Add a number of elements to the ringbuffer.
 * @details         Only so many elements are added as fit in the ringbuffer.
 *                  No elements get overwritten.
 *                  If this is not the intended behavior, then use rb_add_element() in a loop instead.
 * @param[in,out]   rb    Ringbuffer to operate on.
 * @param[in]       buf   Buffer to add elements from.
 * @param[in]       n     Maximum number of elements to add.
 * @returns         Number of elements actually added. 0 if rb is full.
 */
int rb_add_elements(ringbuffer_t *rb, const char *buf, int n);

/**
 * @brief           Peek and remove oldest element from the ringbuffer.
 * @param[in,out]   rb   Ringbuffer to operate on.
 * @returns         The oldest element that was added, or `-1` if rb is empty.
 */
int rb_get_element(ringbuffer_t *rb);

/**
 * @brief           Read and remove a number of elements from the ringbuffer.
 * @param[in,out]   rb    Ringbuffer to operate on.
 * @param[out]      buf   Buffer to write into.
 * @param[in]       n     Read at most n elements.
 * @returns         Number of elements actually read.
 */
int rb_get_elements(ringbuffer_t *rb, char *buf, int n);

#endif /* __RINGBUFFER_H */

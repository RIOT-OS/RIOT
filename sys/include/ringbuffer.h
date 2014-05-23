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

/**
 * @brief     Ringbuffer.
 * @details   Non thread-safe FIFO ringbuffer implementation around a `char` array.
 */
typedef struct ringbuffer {
    char *buf;          /**< Buffer to operate on. */
    unsigned int size;  /**< Size of buf. */
    unsigned int start; /**< Current read position in the ring buffer. */
    unsigned int avail; /**< Number of elements available for reading. */
} ringbuffer_t;

/**
 * @def          RINGBUFFER_INIT(BUF)
 * @brief        Initialize a ringbuffer.
 * @details      This macro is meant for static ringbuffers.
 * @param[in]    BUF   Buffer to use for the ringbuffer. The size is deduced through `sizeof (BUF)`.
 * @returns      The static initializer.
 */
#define RINGBUFFER_INIT(BUF) { (BUF), sizeof (BUF), 0, 0 }

/**
 * @brief        Initialize a ringbuffer.
 * @param[out]   rb        Datum to initialize.
 * @param[in]    buffer    Buffer to use by rb.
 * @param[in]    bufsize   `sizeof (buffer)`
 */
void ringbuffer_init(ringbuffer_t *restrict rb, char *buffer, unsigned bufsize);

/**
 * @brief           Add an element to the ringbuffer.
 * @details         If rb is full, then the oldest element gets overwritten.
 *                  Test ringbuffer_full() first if overwriting is not intended.
 * @param[in,out]   rb   Ringbuffer to operate on.
 * @param[in]       c    Element to add.
 * @returns         The element that was dropped, iff the buffer was full.
 *                  -1 iff the buffer was not full.
 */
int ringbuffer_add_one(ringbuffer_t *restrict rb, char c);

/**
 * @brief           Add a number of elements to the ringbuffer.
 * @details         Only so many elements are added as fit in the ringbuffer.
 *                  No elements get overwritten.
 *                  If this is not the intended behavior, then use ringbuffer_add_one() in a loop instead.
 * @param[in,out]   rb    Ringbuffer to operate on.
 * @param[in]       buf   Buffer to add elements from.
 * @param[in]       n     Maximum number of elements to add.
 * @returns         Number of elements actually added. 0 if rb is full.
 */
unsigned ringbuffer_add(ringbuffer_t *restrict rb, const char *buf, unsigned n);

/**
 * @brief           Peek and remove oldest element from the ringbuffer.
 * @param[in,out]   rb   Ringbuffer to operate on.
 * @returns         The oldest element that was added, or `-1` if rb is empty.
 */
int ringbuffer_get_one(ringbuffer_t *restrict rb);

/**
 * @brief           Read and remove a number of elements from the ringbuffer.
 * @param[in,out]   rb    Ringbuffer to operate on.
 * @param[out]      buf   Buffer to write into.
 * @param[in]       n     Read at most n elements.
 * @returns         Number of elements actually read.
 */
unsigned ringbuffer_get(ringbuffer_t *restrict rb, char *buf, unsigned n);

/**
 * @brief           Test if the ringbuffer is empty.
 * @param[in,out]   rb    Ringbuffer to operate on.
 * @returns         0 iff not empty
 */
static inline int ringbuffer_empty(const ringbuffer_t *restrict rb)
{
    return rb->avail == 0;
}

/**
 * @brief           Test if the ringbuffer is full.
 * @param[in,out]   rb    Ringbuffer to operate on.
 * @returns         0 iff not full
 */
static inline int ringbuffer_full(const ringbuffer_t *restrict rb)
{
    return rb->avail == rb->size;
}

/**
 * @brief           Read, but don't remove, the oldest element in the buffer.
 * @param[in]       rb    Ringbuffer to operate on.
 * @returns         Same as ringbuffer_get_one()
 */
int ringbuffer_peek_one(const ringbuffer_t *restrict rb);

/**
 * @brief           Read, but don't remove, the a number of element of the buffer.
 * @param[in]       rb    Ringbuffer to operate on.
 * @param[out]      buf   Buffer to write into.
 * @param[in]       n     Read at most n elements.
 * @returns         Same as ringbuffer_get()
 */
unsigned ringbuffer_peek(const ringbuffer_t *restrict rb, char *buf, unsigned n);

/**
 * @brief           Remove a number of elements from the ringbuffer.
 * @param[in,out]   rb    Ringbuffer to operate on.
 * @param[in]       n     Read at most n elements.
 * @returns         Number of elements actually removed.
 */
unsigned ringbuffer_remove(ringbuffer_t *restrict rb, unsigned n)

#endif /* __RINGBUFFER_H */

/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_oorb    Thread-safe ringbuffer for one writer and one reader
 * @ingroup     sys
 * @brief       Thread-safe ringbuffer for one writer and one reader
 *              implementation
 * @{
 *
 * @note        Use @ref sys_tsrb instead, if you have more than one thread
 *              writing to or more than one thread reading from the ring buffer
 *
 * @attention   Buffer size must be a power of two!
 *
 * @warning     This is only safe to use when (in addition to only one reader
 *              and one writer being present) the target platforms implements
 *              writes to `unsigned int` atomically (e.g. not on AVR) *and*
 *              the compiler does not reorder code across memory accesses to
 *              `volatile`. Improving this situation is planned.
 *
 * @file
 * @brief       Thread-safe ringbuffer for one writer and one reader interface
 *              definition
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef OORB_H
#define OORB_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     thread-safe ringbuffer struct
 */
typedef struct oorb {
    uint8_t *buf;               /**< Buffer to operate on. */
    unsigned int size;          /**< Size of buffer, must be power of 2. */
    volatile unsigned reads;    /**< total number of reads */
    volatile unsigned writes;   /**< total number of writes */
} oorb_t;

/**
 * @brief Static initializer
 */
#define OORB_INIT(BUF) { (BUF), sizeof (BUF), 0, 0 }

/**
 * @brief        Initialize a oorb.
 * @param[out]   rb        Datum to initialize.
 * @param[in]    buffer    Buffer to use by oorb.
 * @param[in]    bufsize   `sizeof (buffer)`, must be power of 2.
 */
static inline void oorb_init(oorb_t *rb, uint8_t *buffer, unsigned bufsize)
{
    /* make sure bufsize is a power of two.
     * http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
     */
    assert((bufsize != 0) && ((bufsize & (~bufsize + 1)) == bufsize));

    rb->buf = buffer;
    rb->size = bufsize;
    rb->reads = 0;
    rb->writes = 0;
}

/**
 * @brief       Test if the oorb is empty.
 * @param[in]   rb  Ringbuffer to operate on
 * @return      0   if not empty
 * @return      1   otherwise
 */
static inline int oorb_empty(const oorb_t *rb)
{
    return (rb->reads == rb->writes);
}


/**
 * @brief       Get number of bytes available for reading
 * @param[in]   rb  Ringbuffer to operate on
 * @return      nr of available bytes
 */
static inline unsigned int oorb_avail(const oorb_t *rb)
{
    return (rb->writes - rb->reads);
}

/**
 * @brief       Test if the oorb is full
 * @param[in]   rb  Ringbuffer to operate on
 * @return      0   if not full
 * @return      1   otherwise
 */
static inline int oorb_full(const oorb_t *rb)
{
    return (rb->writes - rb->reads) == rb->size;
}

/**
 * @brief       Get free space in ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @return      nr of available bytes
 */
static inline unsigned int oorb_free(const oorb_t *rb)
{
    return (rb->size - rb->writes + rb->reads);
}

/**
 * @brief       Get a byte from ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @return      >=0 byte that has been read
 * @return      -1  if no byte available
 */
int oorb_get_one(oorb_t *rb);

/**
 * @brief       Get bytes from ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @param[out]  dst buffer to write to
 * @param[in]   n   max number of bytes to write to @p dst
 * @return      nr of bytes written to @p dst
 */
int oorb_get(oorb_t *rb, uint8_t *dst, size_t n);

/**
 * @brief       Drop bytes from ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @param[in]   n   max number of bytes to drop
 * @return      nr of bytes dropped
 */
int oorb_drop(oorb_t *rb, size_t n);

/**
 * @brief       Add a byte to ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @param[in]   c   Character to add to ringbuffer
 * @return      0   on success
 * @return      -1  if no space available
 */
int oorb_add_one(oorb_t *rb, uint8_t c);

/**
 * @brief       Add bytes to ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @param[in]   src buffer to read from
 * @param[in]   n   max number of bytes to read from @p src
 * @return      nr of bytes read from @p src
 */
int oorb_add(oorb_t *rb, const uint8_t *src, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* OORB_H */
/** @} */

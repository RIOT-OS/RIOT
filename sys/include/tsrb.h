/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_tsrb Thread safe ringbuffer
 * @ingroup     sys
 * @{
 */

/**
 * @file
 * @brief       Thread-safe ringbuffer implementation
 *
 * This ringbuffer implementation can be used without locking if
 * there's only one producer and one consumer.
 *
 * @note Buffer size must be a power of two!
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef TSRB_H
#define TSRB_H

#include <assert.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     thread-safe ringbuffer struct
 */
typedef struct tsrb {
    char *buf;                  /**< Buffer to operate on. */
    unsigned int size;          /**< Size of buf. */
    volatile unsigned reads;    /**< total number of reads */
    volatile unsigned writes;   /**< total number of writes */
} tsrb_t;

/**
 * @brief Static initializer
 */
#define TSRB_INIT(BUF) { (BUF), sizeof (BUF), 0, 0 }

/**
 * @brief        Initialize a tsrb.
 * @param[out]   rb        Datum to initialize.
 * @param[in]    buffer    Buffer to use by tsrb.
 * @param[in]    bufsize   `sizeof (buffer)`
 */
static inline void tsrb_init(tsrb_t *rb, char *buffer, unsigned bufsize)
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
 * @brief       Test if the tsrb is empty.
 * @param[in]   rb  Ringbuffer to operate on
 * @return      0   if not empty
 * @return      1   otherwise
 */
static inline int tsrb_empty(const tsrb_t *rb)
{
    return (rb->reads == rb->writes);
}


/**
 * @brief       Get number of bytes available for reading
 * @param[in]   rb  Ringbuffer to operate on
 * @return      nr of available bytes
 */
static inline unsigned int tsrb_avail(const tsrb_t *rb)
{
    return (rb->writes - rb->reads);
}

/**
 * @brief       Test if the tsrb is full
 * @param[in]   rb  Ringbuffer to operate on
 * @return      0   if not full
 * @return      1   otherwise
 */
static inline int tsrb_full(const tsrb_t *rb)
{
    return (rb->writes - rb->reads) == rb->size;
}

/**
 * @brief       Get free space in ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @return      nr of available bytes
 */
static inline unsigned int tsrb_free(const tsrb_t *rb)
{
    return (rb->size - rb->writes + rb->reads);
}

/**
 * @brief       Get a byte from ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @return      >=0 byte that has been read
 * @return      -1  if no byte available
 */
int tsrb_get_one(tsrb_t *rb);

/**
 * @brief       Get bytes from ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @param[out]  dst buffer to write to
 * @param[in]   n   max number of bytes to write to @p dst
 * @return      nr of bytes written to @p dst
 */
int tsrb_get(tsrb_t *rb, char *dst, size_t n);

/**
 * @brief       Add a byte to ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @param[in]   c   Character to add to ringbuffer
 * @return      0   on success
 * @return      -1  if no space available
 */
int tsrb_add_one(tsrb_t *rb, char c);

/**
 * @brief       Add bytes to ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @param[in]   src buffer to read from
 * @param[in]   n   max number of bytes to read from @p src
 * @return      nr of bytes read from @p src
 */
int tsrb_add(tsrb_t *rb, const char *src, size_t n);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* TSRB_H */

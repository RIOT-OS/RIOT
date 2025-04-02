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
 * @brief       Thread-safe ringbuffer implementation
 * @{
 *
 * @attention   Buffer size must be a power of two!
 *
 * @file
 * @brief       Thread-safe ringbuffer interface definition
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef TSRB_H
#define TSRB_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     thread-safe ringbuffer struct
 */
typedef struct tsrb {
    uint8_t *buf;               /**< Buffer to operate on. */
    unsigned int size;          /**< Size of buffer, must be power of 2. */
    unsigned reads;             /**< total number of reads */
    unsigned writes;            /**< total number of writes */
} tsrb_t;

/**
 * @brief Static initializer
 *
 * @note The size of the buffer (`sizeof(@p BUF)`) must be a power of two.
 *
 * @param[in] BUF       Buffer to use by tsrb.
 */
#define TSRB_INIT(BUF) { (BUF), sizeof (BUF), 0, 0 }

/**
 * @brief        Initialize a tsrb.
 *
 * @note The size of the buffer (@p bufsize) must be a power of two.
 *
 * @param[out]   rb        Datum to initialize.
 * @param[in]    buffer    Buffer to use by tsrb.
 * @param[in]    bufsize   Size of @p buffer.
 */
static inline void tsrb_init(tsrb_t *rb, uint8_t *buffer, unsigned bufsize)
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
 * @brief        Clear a tsrb.
 * @param[out]   rb Ringbuffer to operate on
 */
static inline void tsrb_clear(tsrb_t *rb)
{
    unsigned irq_state = irq_disable();
    rb->reads = rb->writes;
    irq_restore(irq_state);
}

/**
 * @brief       Test if the tsrb is empty.
 * @param[in]   rb  Ringbuffer to operate on
 * @return      0   if not empty
 * @return      1   otherwise
 */
static inline int tsrb_empty(const tsrb_t *rb)
{
    unsigned irq_state = irq_disable();
    int retval = (rb->reads == rb->writes);
    irq_restore(irq_state);
    return retval;
}

/**
 * @brief       Get number of bytes available for reading
 * @param[in]   rb  Ringbuffer to operate on
 * @return      nr of available bytes
 */
static inline unsigned int tsrb_avail(const tsrb_t *rb)
{
    unsigned irq_state = irq_disable();
    int retval = (rb->writes - rb->reads);
    irq_restore(irq_state);
    return retval;
}

/**
 * @brief       Test if the tsrb is full
 * @param[in]   rb  Ringbuffer to operate on
 * @return      0   if not full
 * @return      1   otherwise
 */
static inline int tsrb_full(const tsrb_t *rb)
{
    unsigned irq_state = irq_disable();
    int retval = (rb->writes - rb->reads) == rb->size;
    irq_restore(irq_state);
    return retval;
}

/**
 * @brief       Get free space in ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @return      nr of available bytes
 */
static inline unsigned int tsrb_free(const tsrb_t *rb)
{
    unsigned irq_state = irq_disable();
    int retval = (rb->size - rb->writes + rb->reads);
    irq_restore(irq_state);
    return retval;
}

/**
 * @brief       Get a byte from ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @return      >=0 byte that has been read
 * @return      -1  if no byte available
 */
int tsrb_get_one(tsrb_t *rb);

/**
 * @brief       Get a byte from ringbuffer, without removing it
 * @param[in]   rb  Ringbuffer to operate on
 * @return      >=0 byte that has been read
 * @return      -1  if no byte available
 */
int tsrb_peek_one(tsrb_t *rb);

/**
 * @brief       Get bytes from ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @param[out]  dst buffer to write to
 * @param[in]   n   max number of bytes to write to @p dst
 * @return      nr of bytes written to @p dst
 */
int tsrb_get(tsrb_t *rb, uint8_t *dst, size_t n);

/**
 * @brief       Get bytes from ringbuffer, without removing them
 * @param[in]   rb  Ringbuffer to operate on
 * @param[out]  dst buffer to write to
 * @param[in]   n   max number of bytes to write to @p dst
 * @return      nr of bytes written to @p dst
 */
int tsrb_peek(tsrb_t *rb, uint8_t *dst, size_t n);

/**
 * @brief       Drop bytes from ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @param[in]   n   max number of bytes to drop
 * @return      nr of bytes dropped
 */
int tsrb_drop(tsrb_t *rb, size_t n);

/**
 * @brief       Add a byte to ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @param[in]   c   Character to add to ringbuffer
 * @return      0   on success
 * @return      -1  if no space available
 */
int tsrb_add_one(tsrb_t *rb, uint8_t c);

/**
 * @brief       Add bytes to ringbuffer
 * @param[in]   rb  Ringbuffer to operate on
 * @param[in]   src buffer to read from
 * @param[in]   n   max number of bytes to read from @p src
 * @return      nr of bytes read from @p src
 */
int tsrb_add(tsrb_t *rb, const uint8_t *src, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* TSRB_H */
/** @} */

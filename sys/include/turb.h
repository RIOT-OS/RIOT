/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2025 Karl Fessel <karl.fessel@ml-pa.com>  ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_tsrb
 * @brief       Thread Unsafe access to Thread-safe ringbuffer
 * @{
 *
 * @attention   use only if you ensurred thread safety otherwise
 *              e.g.: by an irq guard
 * @file
 * @brief       Thread-unsafe ringbuffer interface definition
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Karl Fessel <karl.fessel@ml-pa.com>
 */

#include "tsrb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Clear a tsrb.
 * @attention   use only if you ensurred thread safety otherwise
 * @param[out]  rb Ringbuffer to operate on
 */
static inline void turb_clear(tsrb_t *rb)
{
    rb->reads = rb->writes;
}

/**
 * @brief       Test if the tsrb is empty.
 * @attention   use only if you ensurred thread safety otherwise
 * @param[in]   rb  Ringbuffer to operate on
 * @return      0   if not empty
 * @return      1   otherwise
 */
static inline int turb_empty(const tsrb_t *rb)
{
    int retval = (rb->reads == rb->writes);
    return retval;
}

/**
 * @brief       Get number of bytes available for reading
 * @attention   use only if you ensurred thread safety otherwise
 * @param[in]   rb  Ringbuffer to operate on
 * @return      nr of available bytes
 */
static inline unsigned int turb_avail(const tsrb_t *rb)
{
    unsigned retval = (rb->writes - rb->reads);
    return retval;
}

/**
 * @brief       Test if the tsrb is full
 * @attention   use only if you ensurred thread safety otherwise
 * @param[in]   rb  Ringbuffer to operate on
 * @return      0   if not full
 * @return      1   otherwise
 */
static inline int turb_full(const tsrb_t *rb)
{
    int retval = (rb->writes - rb->reads) == rb->size;
    return retval;
}

/**
 * @brief       Get free space in ringbuffer
 * @attention   use only if you ensurred thread safety otherwise
 * @param[in]   rb  Ringbuffer to operate on
 * @return      nr of available bytes
 */
static inline unsigned int turb_free(const tsrb_t *rb)
{
    unsigned int retval = (rb->size - rb->writes + rb->reads);
    return retval;
}

#ifndef DOXYGEN
/* these are internal function */
static inline void _turb_push(tsrb_t *rb, uint8_t c)
{
    rb->buf[rb->writes++ & (rb->size - 1)] = c;
}

static inline uint8_t _turb_pop(tsrb_t *rb)
{
    return rb->buf[rb->reads++ & (rb->size - 1)];
}

static inline uint8_t _turb_peek(tsrb_t *rb, unsigned int idx)
{
    return rb->buf[(rb->reads + idx) & (rb->size - 1)];
}
#endif

/**
 * @brief       Get a byte from ringbuffer
 * @attention   use only if you ensurred thread safety otherwise
 * @param[in]   rb  Ringbuffer to operate on
 * @return      >=0 byte that has been read
 * @return      -1  if no byte available
 */
static inline int turb_get_one(tsrb_t *rb)
{
    int retval = -1;
    if (!turb_empty(rb)) {
        retval = _turb_pop(rb);
    }
    return retval;
}

/**
 * @brief       Get a byte from ringbuffer, without removing it
 * @attention   use only if you ensurred thread safety otherwise
 * @param[in]   rb  Ringbuffer to operate on
 * @return      >=0 byte that has been read
 * @return      -1  if no byte available
 */
static inline int turb_peek_one(tsrb_t *rb)
{
    int retval = -1;
    if (!turb_empty(rb)) {
        retval = _turb_peek(rb, 0);
    }
    return retval;
}

/**
 * @brief       Get bytes from ringbuffer
 * @attention   use only if you ensurred thread safety otherwise
 * @param[in]   rb  Ringbuffer to operate on
 * @param[out]  dst buffer to write to
 * @param[in]   n   max number of bytes to write to @p dst
 * @return      nr of bytes written to @p dst
 */
static inline int turb_get(tsrb_t *rb, uint8_t *dst, size_t n)
{
    size_t cnt = 0;
    while (n-- && !turb_empty(rb)) {
        *dst++ = _turb_pop(rb);
        cnt++;
    }
    return (int) cnt;
}

/**
 * @brief       Get bytes from ringbuffer, without removing them
 * @attention   use only if you ensurred thread safety otherwise
 * @param[in]   rb  Ringbuffer to operate on
 * @param[out]  dst buffer to write to
 * @param[in]   n   max number of bytes to write to @p dst
 * @return      nr of bytes written to @p dst
 */
static inline int turb_peek(tsrb_t *rb, uint8_t *dst, size_t n)
{
    size_t idx = 0;
    unsigned int avail = turb_avail(rb);
    while (idx < n && idx != avail) {
        *dst++ = _turb_peek(rb, idx++);
    }
    return (int) idx;
}

/**
 * @brief       Drop bytes from ringbuffer
 * @attention   use only if you ensurred thread safety otherwise
 * @param[in]   rb  Ringbuffer to operate on
 * @param[in]   n   max number of bytes to drop
 * @return      nr of bytes dropped
 */
static inline int turb_drop(tsrb_t *rb, size_t n)
{
    size_t cnt = 0;
    while (n-- && !turb_empty(rb)) {
        _turb_pop(rb);
        cnt++;
    }
    return (int) cnt;
}

/**
 * @brief       Add a byte to ringbuffer
 * @attention   use only if you ensurred thread safety otherwise
 * @param[in]   rb  Ringbuffer to operate on
 * @param[in]   c   Character to add to ringbuffer
 * @return      0   on success
 * @return      -1  if no space available
 */
static inline int turb_add_one(tsrb_t *rb, uint8_t c)
{
    int retval = -1;
    if (!turb_full(rb)) {
        _turb_push(rb, c);
        retval = 0;
    }
    return retval;
}

/**
 * @brief       Add bytes to ringbuffer
 * @attention   use only if you ensurred thread safety otherwise
 * @param[in]   rb  Ringbuffer to operate on
 * @param[in]   src buffer to read from
 * @param[in]   n   max number of bytes to read from @p src
 * @return      nr of bytes read from @p src
 */
static inline int turb_add(tsrb_t *rb, const uint8_t *src, size_t n)
{
    size_t ret = 0;
    while (n-- && !turb_full(rb)) {
        _turb_push(rb, *src++);
        ret++;
    }
    return (int) ret;
}

#ifdef __cplusplus
}
#endif

/** @} */

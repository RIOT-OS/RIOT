/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup isr_pipe ISR Pipe
 * @ingroup sys
 * @brief ISR -> userspace pipe
 *
 * @{
 * @file
 * @brief       isrpipe Interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#include <stdint.h>

#include "mutex.h"
#include "tsrb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Context structure for isrpipe
 */
typedef struct {
    tsrb_t tsrb;        /**< isrpipe thread safe ringbuffer */
    mutex_t mutex;      /**< isrpipe mutex */
} isrpipe_t;

/**
 * @brief   Static initializer for irspipe
 *
 * @note The size of the buffer (`sizeof(@p buf)`) must be a power of two.
 *
 * @param[in] buf           buffer to use as ringbuffer
 */
#define ISRPIPE_INIT(buf) { .mutex = MUTEX_INIT_LOCKED, \
                            .tsrb = TSRB_INIT(buf) }

/**
 * @brief   Initialisation function for isrpipe
 *
 * @note The size of the buffer (@p bufsize) must be a power of two.
 *
 * @param[in]   isrpipe     isrpipe object to initialize
 * @param[in]   buf         buffer to use as ringbuffer
 * @param[in]   bufsize     size of @p buf
 */
void isrpipe_init(isrpipe_t *isrpipe, uint8_t *buf, size_t bufsize);

/**
 * @brief   Put one byte into the isrpipe's buffer
 *
 * @param[in]   isrpipe     isrpipe object to operate on
 * @param[in]   c           byte to add to isrpipe buffer
 *
 * @returns     0 if byte could be added
 * @returns     -1 if buffer was full
 */
int isrpipe_write_one(isrpipe_t *isrpipe, uint8_t c);

/**
 * @brief   Put number of bytes into the isrpipe's buffer
 *
 * @param[in]   isrpipe     isrpipe object to operate on
 * @param[in]   buf         bytes to add to isrpipe buffer
 * @param[in]   n           number of bytes to add from buf to isrpipe's buffer
 *
 * @returns     number of bytes that could be added
 * @returns     -1 if buffer was full
 */
int isrpipe_write(isrpipe_t *isrpipe, const uint8_t *buf, size_t n);

/**
 * @brief   Read data from isrpipe (blocking)
 *
 * @param[in]   isrpipe    isrpipe object to operate on
 * @param[out]  buf        buffer to write to
 * @param[in]   count      number of bytes to read
 *
 * @returns     number of bytes read
 */
int isrpipe_read(isrpipe_t *isrpipe, uint8_t *buf, size_t count);

/**
 * @brief   Read a single byte from isrpipe (blocking)
 *
 * @param[in]   isrpipe    isrpipe object to operate on
 *
 * @returns     the byte read
 */
uint8_t isrpipe_read_one(isrpipe_t *isrpipe);

/**
 * @brief   Clear the isrpipe's buffer
 *
 * @param[in]   isrpipe    isrpipe object to operate on
 *
 * @note    If a write has already unlocked the internal mutex, the first
 *          blocking read after this call will consume that pending wakeup
 *          before blocking normally.
 */
void isrpipe_clear(isrpipe_t *isrpipe);

/**
 * @brief   Get the number of bytes available for reading from the isrpipe
 *
 * @param[in]   isrpipe    isrpipe object to operate on
 *
 * @returns     number of bytes available in the isrpipe's buffer
 */
unsigned int isrpipe_available(isrpipe_t *isrpipe);

#ifdef __cplusplus
}
#endif
/** @} */

/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_chunk_buffer    chunked Ringbuffer
 * @ingroup     sys
 * @brief       Implementation of a Ringbuffer to store chunks of data
 * @{
 *
 * @file
 * @brief   Chunked Ringbuffer
 *
 * A chunked ringbuffer is a ringbuffer that holds chunks of data.
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The maximum number of chunks that can be stored in a Chunked Ringbuffer
 *
 */
#ifndef CONFIG_CHUNK_NUM_MAX
#define CONFIG_CHUNK_NUM_MAX   (4)
#endif

/**
 * @brief A chunked ringbuffer
 *
 */
typedef struct {
    uint8_t *buffer;        /**< pointer to the memory to hold the data */
    uint8_t *buffer_end;    /**< last data element */
    uint8_t *cur;           /**< current write pointer */
    uint8_t *cur_start;     /**< start of the currently written chunk */
    uint8_t *protect;       /**< start of the first valid chunk */
    uint8_t *chunk_start[CONFIG_CHUNK_NUM_MAX];    /**< Array to hold start of done chunks */
    uint16_t chunk_len[CONFIG_CHUNK_NUM_MAX];      /**< Length of valid chunks */
    uint8_t chunk_cur;                      /**< Index of the first valid chunk */
} chunk_ringbuf_t;

/**
 * @brief Callback function for @ref crb_chunk_foreach
 *
 * @param[in] ctx   Callback context
 * @param[in] bytes Chunk data
 * @param[in] len   Length of data
 */
typedef void (*crb_foreach_callback_t)(void *ctx, uint8_t *bytes, size_t len);

/**
 * @brief Initialize a Chunked Ringbuffer
 *
 * @param[in] rb    The Ringbuffer to work on
 * @param buffer    The Ringbuffer work area
 * @param len       Size of the Ringbuffer work area
 */
void crb_init(chunk_ringbuf_t *rb, void *buffer, size_t len);

/**
 * @brief Close the current chunk
 *
 * @note  This function is expected to be called in ISR context / with
 *        interrupts disabled.
 *
 * @param[in] rb        The Ringbuffer to work on
 * @param[in] valid     True if the chunk is valid and should be stored
 *                      False if the current chunk should be discarded
 *
 * @return size of chunk if the chunk could be stored in the valid chunk array
 * @return 0 if there is no more space in the valid chunk array
 */
unsigned crb_end_chunk(chunk_ringbuf_t *rb, bool valid);

/**
 * @brief Start a new chunk on the ringbuffer
 *
 * If an unfinished chunk already exists, it will be discarded.
 *
 * @note  This function is expected to be called in ISR context / with
 *        interrupts disabled.
 *
 * @param[in] rb    The Ringbuffer to work on
 *
 * @return true     If a new chunk could be started
 * @return false    If the ringbuffer is full
 */
static inline bool crb_start_chunk(chunk_ringbuf_t *rb)
{
    /* discard stale chunk */
    if (rb->cur_start) {
        crb_end_chunk(rb, false);
    }

    /* pointing to the start of the first chunk */
    if (rb->cur == rb->protect) {
        return false;
    }

    rb->cur_start = rb->cur;

    if (rb->protect == NULL) {
        rb->protect = rb->cur_start;
    }

    return true;
}

/**
 * @brief Insert a byte into the current chunk
 *
 * @note  This function is expected to be called in ISR context / with
 *        interrupts disabled.
 *
 * @pre A new chunk has been started with @ref crb_start_chunk
 *
 * @param[in] rb        The Ringbuffer to work on
 * @param[in] b         The byte to write
 *
 * @return true         If the byte could be written
 * @return false        If the ringbuffer is full
 */
static inline bool crb_add_byte(chunk_ringbuf_t *rb, uint8_t b)
{
    /* if this is the first chunk, protect will be at start */
    if (rb->cur == rb->protect &&
        rb->cur != rb->cur_start) {
        return false;
    }

    *rb->cur = b;

    /* handle wrap around */
    if (rb->cur == rb->buffer_end) {
        rb->cur = rb->buffer;
    } else {
        ++rb->cur;
    }

    return true;
}

/**
 * @brief Insert a number of bytes into the current chunk
 *
 * @note  This function is expected to be called in ISR context / with
 *        interrupts disabled.
 *
 * @pre A new chunk has been started with @ref crb_start_chunk
 *
 * @param[in] rb        The Ringbuffer to work on
 * @param[in] data      The data to write
 * @param[in] len       Size of data
 *
 * @return true         If the bytes could be written
 * @return false        If the ringbuffer is full
 */
bool crb_add_bytes(chunk_ringbuf_t *rb, const void *data, size_t len);

/**
 * @brief Add a complete chunk to the Ringbuffer
 *
 * @note  This function is expected to be called in ISR context / with
 *        interrupts disabled.
 *
 * This is a convenience function that combines @ref crb_start_chunk,
 * @ref crb_add_bytes and @ref crb_end_chunk
 *
 * @param[in] rb        The Ringbuffer to work on
 * @param[in] data      The data to write
 * @param[in] len       Size of data
 *
 * @return true         If the chunk could be added to the valid chunk array
 * @return false        There was not enough space and the chunk was discarded
 */
bool crb_add_chunk(chunk_ringbuf_t *rb, const void *data, size_t len);

/**
 * @brief Get the size of the first valid chunk
 *
 * @param[in] rb        The Ringbuffer to work on
 * @param[out] len      Pointer to store the size of the first valid chunk
 *
 * @return true         If a valid chunk exists and @p size was written
 * @return false        If no valid chunk exists
 */
bool crb_get_chunk_size(chunk_ringbuf_t *rb, size_t *len);

/**
 * @brief Get a number of bytes from the first valid chunk without consuming it.
 *
 * @param[in] rb        The Ringbuffer to work on
 * @param[out] dst      Destination buffer
 * @param[in] offset    Offset to the start of the chunk
 * @param[in] len       Number of bytes to read
 *
 * @return true         If the data could be read
 * @return false        If no valid chunk exists or the bytes could not be read
 */
bool crb_peek_bytes(chunk_ringbuf_t *rb, void *dst, size_t offset, size_t len);

/**
 * @brief Remove a chunk from the valid chunk array
 *
 * @param[in] rb        The Ringbuffer to work on
 * @param[out] dst      Destination where the chunk contents should be copied to.
 *                      May be NULL, then the chunk is just discarded.
 * @param[in] len       Max number of bytes to read. If there are bytes left in the
 *                      chunk beyond that, they will be discarded
 *
 * @return true         If a chunk was consumed
 * @return false        If no valid chunk did exist
 */
bool crb_consume_chunk(chunk_ringbuf_t *rb, void *dst, size_t len);

/**
 * @brief Execute a callback for each byte in the first valid chunk
 *        The callback function may be called twice if the chunk is non-continuous.
 *
 *        This function will not consume the chunk.
 *
 * @param[in] rb        The Ringbuffer to work on
 * @param[in] func      The function to call for each byte
 * @param[in] ctx       Optional function argument
 *
 * @return true         If a valid chunk exits on which the function was executed
 * @return false        If no valid chunk exists
 */
bool crb_chunk_foreach(chunk_ringbuf_t *rb, crb_foreach_callback_t func, void *ctx);

#ifdef __cplusplus
}
#endif

/** @} */

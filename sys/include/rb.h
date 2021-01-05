/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_rb      Ring buffers
 * @ingroup     sys
 *
 * Format and Concept
 * ==================
 *
 * The memory layout looks like this:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *                                      writing        free       reading
 *                                   |-----------||------------||--------|
 *
 * +---------------------------------------------------------------------------+
 * |XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXOOOOOOOOOOOOO..............XXXXXXXXXXXXXXX|
 * +---------------------------------------------------------------------------+
 *                                   ^            ^             ^         ^
 *                                   |            |             |         |
 *                                   w_done       w_next        r_done    r_next
 *
 * X = data ready for reading
 * O = chunk that currently is written to (e.g. via DMA), but data is not yet
 *     valid
 * . = free buffer space
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The idea is that a reader reserves a chunk of data for reading by increasing
 * `r_next` and incrementing `n_readers`. The area from the old value of
 * `r_next` (including) to the new value of `r_next) (not including) is
 * reserved for reading. Once data has been processed, `n_readers` is
 * decremented. Once `n_readers` reaches zero, `r_done` is set to
 * `r_next` the buffer chunk(s) processed by the reader(s) over to the
 * writer(s).
 *
 * A writer reserves a chunk of data for writing by increasing `w_next`. The
 * area from `w_done` (including) to `w_next` (not including) is reserved for
 * writing. Once everything has been written, `w_done` is set to `w_next` to
 * hand this chunk over to the reader(s).
 *
 * 
 *
 * @{
 */

#ifndef RB_H
#define RB_H

#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#include "architecture.h"
#include "atomic_utils.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Basic ringbuffer type
 *
 * @warning This type is for internal use only
 *
 */
typedef struct {
    uint8_t *buf;       /**< Buffer to work on */
    uint16_t size;      /**< Size of the buffer */
    uint16_t r_next;    /**< Position of the next byte to read */
    uint16_t r_done;    /**< Bytes up to (not including) here have been read */
    uint16_t w_next;    /**< Position of the next byte to write to */
    uint16_t w_done;    /**< Bytes up to (not including) here have been
                             completely written */
    uint16_t r_align;   /**< Mask to enforce alignments on reads  */
    uint16_t w_align;   /**< Mask to enforce alignments on writes */
    uint8_t n_readers;  /**< Number of readers having reserved a chunk of the
                             buffer */
    uint8_t n_writers;  /**< Number of writers having reserved a chunk of the
                             buffer */
} rb_base_t;

/**
 * @brief   Ringbuffer with `uint8_t` as internal data unit
 */
typedef struct {
    rb_base_t base;
} rb_u8_t;

/**
 * @brief   Ringbuffer with `uint16_t` as internal data unit
 */
typedef struct {
    rb_base_t base;
} rb_u16_t;

/**
 * @brief   Ringbuffer with `uint32_t` as internal data unit
 */
typedef struct {
    rb_base_t base;
} rb_u32_t;

/**
 * @name    Low-level implementation for internal use
 * @{
 */

/**
 * @brief   Reserve a chunk of data for reading
 *
 * @warning This is for internal use only
 *
 * @param[in,out]   rb          Ring buffer to operate on
 * @param[out]      chunk       Store address of the start of chunk here
 * @param[in]       max_size    Maximum size of the chunk to retrieve
 *
 * @return  Number of bytes reserved
 * @retval  0       No data ready for reading in buffer
 */
size_t rb_base_read_start(rb_base_t *rb, uintptr_t *chunk, size_t max_size);

/**
 * @brief   Signal completion of precessing of the reserved chunk
 *
 * @warning This is for internal use only
 *
 * @param[in,out]   rb          Ring buffer to operate on
 */
void rb_base_read_done(rb_base_t *rb);

/**
 * @brief   Reserve a chunk of data for writing
 *
 * @warning This is for internal use only
 *
 * @param[in,out]   rb          Ring buffer to operate on
 * @param[out]      chunk       Store address of the start of chunk here
 * @param[in]       max_size    Maximum size of the chunk to retrieve
 *
 * @return  Number of bytes reserved
 * @retval  0       No data ready for writing in buffer
 */
size_t rb_base_write_start(rb_base_t *rb, uintptr_t *chunk, size_t max_size);

/**
 * @brief   Signal writing to the reserved chunk has completed
 *
 * @warning This is for internal use only
 *
 * @param[in,out]   rb          Ring buffer to operate on
 */
void rb_base_write_done(rb_base_t *rb);

/**
 * @brief   Initialize a ring buffer
 *
 * @warning This is for internal use only
 *
 * @param[in,out]   rb          Ring buffer to initialize
 * @param[in]       buffer      Buffer used in the ring buffer
 * @param[in]       buffer_size Size of @p buffer
 * @param[in]       read_align  Align data chunks for reading to this value.
 *                              (A value of 0 and 1 will both allow unaligned
 *                              byte-wise access.)
 * @param[in]       write_align Align data chunks for writing to this value.
 *                              (A value of 0 and 1 will both allow unaligned
 *                              byte-wise access.)
 *
 * @pre     The alignment of @p buffer is at least the maximum of
 *          @p read_align and @p write_align (otherwise an `assert()` will blow
 *          up)
 * @pre     @p buffer_size is a power of two
 * @pre     @p read_align and @p write_align are powers of two
 * @pre     @p read_align < @p buffer_size and @p write_align < @p buffer_size
 */
void rb_base_init(rb_base_t *rb, void *buffer, size_t buffer_size,
                  size_t read_align, size_t write_align);

/** @} */ /* End: Low-level implementation for internal use */

/**
 * @brief   Reserve a chunk of data for reading - `uint8_t` version
 *
 * @param[in,out]   rb          Ring buffer to operate on
 * @param[out]      chunk       Store address of the start of chunk here
 * @param[in]       max_size    Maximum size of the chunk to retrieve
 *                              (number of `uint8_t` elements)
 *
 * @return  Number of `uint8_t` elements reserved
 * @retval  0       No data ready for reading in buffer
 */
static inline size_t rb_u8_read_start(rb_u8_t *rb, uint8_t **chunk,
                                      size_t max_size)
{
    uintptr_t tmp;
    size_t retval = rb_base_read_start(&rb->base, &tmp, max_size);
    *chunk = (uint8_t *)tmp;
    return retval;
}

/**
 * @brief   Reserve a chunk of data for reading - `uint16_t` version
 *
 * @param[in,out]   rb          Ring buffer to operate on
 * @param[out]      chunk       Store address of the start of chunk here
 * @param[in]       max_size    Maximum size of the chunk to retrieve
 *                              (number of `uint16_t` elements)
 *
 * @return  Number of `uint16_t` elements reserved
 * @retval  0       No data ready for reading in buffer
 */
static inline size_t rb_u16_read_start(rb_u16_t *rb, uint16_t **chunk,
                                       size_t max_size)
{
    uintptr_t tmp;
    size_t retval = rb_base_read_start(&rb->base, &tmp, max_size << 1);
    *chunk = (uint16_t *)tmp;
    return retval >> 1;
}

/**
 * @brief   Reserve a chunk of data for reading - `uint32_t` version
 *
 * @param[in,out]   rb          Ring buffer to operate on
 * @param[out]      chunk       Store address of the start of chunk here
 * @param[in]       max_size    Maximum size of the chunk to retrieve
 *                              (number of `uint32_t` elements)
 *
 * @return  Number of `uint32_t` elements reserved
 * @retval  0       No data ready for reading in buffer
 */
static inline size_t rb_u32_read_start(rb_u32_t *rb, uint32_t **chunk,
                                       size_t max_size)
{
    uintptr_t tmp;
    size_t retval = rb_base_read_start(&rb->base, &tmp, max_size << 2);
    *chunk = (uint32_t *)tmp;
    return retval >> 2;
}

/**
 * @brief   Signal completion of precessing of the reserved chunk - `uint8_t`
 *          version
 *
 * @param[in,out]   rb          Ring buffer to operate on
 */
static inline void rb_u8_read_done(rb_u8_t *rb)
{
    rb_base_read_done(&rb->base);
}

/**
 * @brief   Signal completion of precessing of the reserved chunk - `uint16_t`
 *          version
 *
 * @param[in,out]   rb          Ring buffer to operate on
 */
static inline void rb_u16_read_done(rb_u16_t *rb)
{
    rb_base_read_done(&rb->base);
}

/**
 * @brief   Signal completion of precessing of the reserved chunk - `uint32_t`
 *          version
 *
 * @param[in,out]   rb          Ring buffer to operate on
 */
static inline void rb_u32_read_done(rb_u32_t *rb)
{
    rb_base_read_done(&rb->base);
}

/**
 * @brief   Reserve a chunk of data for writing - `uint8_t` version
 *
 * @param[in,out]   rb          Ring buffer to operate on
 * @param[out]      chunk       Store address of the start of chunk here
 * @param[in]       max_size    Maximum size of the chunk to retrieve
 *                              (number of `uint8_t` elements)
 *
 * @return  Number of `uint8_t` elements reserved
 * @retval  0       No data ready for writing in buffer
 */
static inline size_t rb_u8_write_start(rb_u8_t *rb, uint8_t **chunk,
                                      size_t max_size)
{
    uintptr_t tmp;
    size_t retval = rb_base_write_start(&rb->base, &tmp, max_size);
    *chunk = (uint8_t *)tmp;
    return retval;
}

/**
 * @brief   Reserve a chunk of data for writing - `uint16_t` version
 *
 * @param[in,out]   rb          Ring buffer to operate on
 * @param[out]      chunk       Store address of the start of chunk here
 * @param[in]       max_size    Maximum size of the chunk to retrieve
 *                              (number of `uint16_t` elements)
 *
 * @return  Number of `uint16_t` elements reserved
 * @retval  0       No data ready for writing in buffer
 */
static inline size_t rb_u16_write_start(rb_u16_t *rb, uint16_t **chunk,
                                       size_t max_size)
{
    uintptr_t tmp;
    size_t retval = rb_base_write_start(&rb->base, &tmp, max_size << 1);
    *chunk = (uint16_t *)tmp;
    return retval >> 1;
}

/**
 * @brief   Reserve a chunk of data for writing - `uint32_t` version
 *
 * @param[in,out]   rb          Ring buffer to operate on
 * @param[out]      chunk       Store address of the start of chunk here
 * @param[in]       max_size    Maximum size of the chunk to retrieve
 *                              (number of `uint32_t` elements)
 *
 * @return  Number of `uint32_t` elements reserved
 * @retval  0       No data ready for writing in buffer
 */
static inline size_t rb_u32_write_start(rb_u32_t *rb, uint32_t **chunk,
                                       size_t max_size)
{
    uintptr_t tmp;
    size_t retval = rb_base_write_start(&rb->base, &tmp, max_size << 2);
    *chunk = (uint32_t *)tmp;
    return retval >> 2;
}

/**
 * @brief   Signal writing to the reserved chunk has completed - `uint8_t`
 *          version
 *
 * @param[in,out]   rb          Ring buffer to operate on
 */
static inline void rb_u8_write_done(rb_u8_t *rb)
{
    rb_base_write_done(&rb->base);
}

/**
 * @brief   Signal writing to the reserved chunk has completed - `uint16_t`
 *          version
 *
 * @param[in,out]   rb          Ring buffer to operate on
 */
static inline void rb_u16_write_done(rb_u16_t *rb)
{
    rb_base_write_done(&rb->base);
}

/**
 * @brief   Signal writing to the reserved chunk has completed - `uint32_t`
 *          version
 *
 * @param[in,out]   rb          Ring buffer to operate on
 */
static inline void rb_u32_write_done(rb_u32_t *rb)
{
    rb_base_write_done(&rb->base);
}

/**
 * @brief   Initialize a ring buffer - `uint8_t` version
 *
 * @param[in,out]   rb          Ring buffer to initialize
 * @param[in]       buffer      Buffer used in the ring buffer
 * @param[in]       buffer_size Size of @p buffer in `uint8_t` elements
 * @param[in]       read_align  Align reserved data chunks to this number of
 *                              `uint8_t` elements. (A value of 0 or 1 allows
 *                              access aligned to a single `uint8_t`.)
 * @param[in]       read_align  Align data chunks for reading to this number
 *                              of `uint8_t` elements.
 * @param[in]       write_align Align data chunks for writing to this number
 *                              of `uint8_t` elements.
 *
 * @pre     The alignment of @p buffer is at least the maximum of
 *          @p read_align and @p write_align (otherwise an `assert()` will blow
 *          up)
 * @pre     @p buffer_size is a power of two
 * @pre     @p read_align and @p write_align are powers of two
 * @pre     @p read_align < @p buffer_size and @p write_align < @p buffer_size
 */
static inline void rb_u8_init(rb_u8_t *rb, uint8_t *buffer, size_t buffer_size,
                              size_t read_align, size_t write_align)
{
    rb_base_init(&rb->base, buffer, buffer_size, read_align, write_align);
}

/**
 * @brief   Initialize a ring buffer - `uint16_t` version
 *
 * @param[in,out]   rb          Ring buffer to initialize
 * @param[in]       buffer      Buffer used in the ring buffer
 * @param[in]       buffer_size Size of @p buffer in `uint16_t` elements
 * @param[in]       read_align  Align data chunks for reading to this number
 *                              of `uint16_t` elements.
 * @param[in]       write_align Align data chunks for writing to this number
 *                              of `uint16_t` elements.
 *
 * @pre     The alignment of @p buffer is at least the maximum of
 *          @p read_align and @p write_align (otherwise an `assert()` will blow
 *          up)
 * @pre     @p buffer_size is a power of two
 * @pre     @p read_align and @p write_align are powers of two
 * @pre     @p read_align < @p buffer_size and @p write_align < @p buffer_size
 */
static inline void rb_u16_init(rb_u16_t *rb, uint16_t *buffer,
                               size_t buffer_size,
                               size_t read_align, size_t write_align)
{
    read_align = (read_align != 0) ? read_align : 1;
    write_align = (write_align != 0) ? write_align : 1;
    rb_base_init(&rb->base, buffer, buffer_size << 1,
                 read_align << 1, write_align << 1);
}

/**
 * @brief   Initialize a ring buffer - `uint32_t` version
 *
 * @param[in,out]   rb          Ring buffer to initialize
 * @param[in]       buffer      Buffer used in the ring buffer
 * @param[in]       buffer_size Size of @p buffer in `uint32_t` elements
 * @param[in]       read_align  Align data chunks for reading to this number
 *                              of `uint16_t` elements.
 * @param[in]       write_align Align data chunks for writing to this number
 *                              of `uint16_t` elements.
 *
 * @pre     The alignment of @p buffer is at least the maximum of
 *          @p read_align and @p write_align (otherwise an `assert()` will blow
 *          up)
 * @pre     @p buffer_size is a power of two
 * @pre     @p read_align and @p write_align are powers of two
 * @pre     @p read_align < @p buffer_size and @p write_align < @p buffer_size
 */
static inline void rb_u32_init(rb_u32_t *rb, uint32_t *buffer,
                               size_t buffer_size,
                               size_t read_align, size_t write_align)
{
    read_align = (read_align != 0) ? read_align : 1;
    write_align = (write_align != 0) ? write_align : 1;
    rb_base_init(&rb->base, buffer, buffer_size << 2,
                 read_align << 2, write_align << 2);
}

#ifdef __cplusplus
}
#endif

#endif /* RB_H */
/** @} */

/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <string.h>
#include "atomic_utils.h"
#include "chunked_ringbuffer.h"
#include "irq.h"

static int _get_free_chunk(chunk_ringbuf_t *rb)
{
    int idx = rb->chunk_cur;
    for (int i = 0; i < CONFIG_CHUNK_NUM_MAX; ++i) {
        uintptr_t _ptr = atomic_load_uintptr((uintptr_t *)&rb->chunk_start[idx]);
        if (_ptr == 0) {
            return idx;
        }

        if (++idx == CONFIG_CHUNK_NUM_MAX) {
            idx = 0;
        }
    }

    return -1;
}

static int _get_complete_chunk(chunk_ringbuf_t *rb)
{
    int idx = rb->chunk_cur;
    for (int i = 0; i < CONFIG_CHUNK_NUM_MAX; ++i) {
        uintptr_t _ptr = atomic_load_uintptr((uintptr_t *)&rb->chunk_start[idx]);
        if (_ptr) {
            return idx;
        }

        if (++idx == CONFIG_CHUNK_NUM_MAX) {
            idx = 0;
        }
    }

    return -1;
}

bool crb_add_bytes(chunk_ringbuf_t *rb, const void *data, size_t len)
{
    const uint8_t *in = data;
    for (size_t i = 0; i < len; ++i) {
        if (!crb_add_byte(rb ,in[i])) {
            return false;
        }
    }

    return true;
}

bool crb_add_chunk(chunk_ringbuf_t *rb, const void *data, size_t len)
{
    if (!crb_start_chunk(rb)) {
        return false;
    }

    bool keep = crb_add_bytes(rb ,data, len);

    return crb_end_chunk(rb ,keep);
}

static unsigned _get_cur_len(chunk_ringbuf_t *rb)
{
    if (rb->cur > rb->cur_start) {
        return rb->cur - rb->cur_start;
    } else {
        /* buffer_end point to the last element */
        return (rb->cur - rb->buffer) + 1
             + (rb->buffer_end - rb->cur_start);
    }
}

bool crb_end_chunk(chunk_ringbuf_t *rb, bool keep)
{
    int idx;

    /* no chunk was started */
    if (rb->cur_start == NULL) {
        return false;
    }

    if (keep) {
        idx = _get_free_chunk(rb);
    } else {
        idx = -1;
    }

    /* discard chunk */
    if (idx < 0) {
        if (rb->protect == rb->cur_start) {
            rb->protect = NULL;
        }
        rb->cur = rb->cur_start;
        rb->cur_start = NULL;
        return false;
    }

    /* store complete chunk */
    rb->chunk_start[idx] = rb->cur_start;
    rb->chunk_len[idx] = _get_cur_len(rb);
    rb->cur_start = NULL;

    return true;
}

bool crb_get_chunk_size(chunk_ringbuf_t *rb, size_t *len)
{
    int idx = _get_complete_chunk(rb);
    if (idx < 0) {
        return false;
    }

    *len = rb->chunk_len[idx];
    return true;
}

bool crb_peek_bytes(chunk_ringbuf_t *rb, void *dst, size_t offset, size_t len)
{
    int idx = _get_complete_chunk(rb);
    if (idx < 0) {
        return false;
    }

    if (offset + len > rb->chunk_len[idx]) {
        return false;
    }

    const uint8_t *start = rb->chunk_start[idx];
    start += offset;

    if (start > rb->buffer_end) {
        start = ((uint8_t *)rb->buffer) + (start - rb->buffer_end + 1);
        memcpy(dst, start, len);
    } else if (start + len <= rb->buffer_end) {
        memcpy(dst, start, len);
    } else {
        size_t len_0 = 1 + rb->buffer_end - start;
        memcpy(dst, start, len_0);
        memcpy((uint8_t *)dst + len_0, rb->buffer, len - len_0);
    }

    return true;
}

bool crb_chunk_foreach(chunk_ringbuf_t *rb, crb_foreach_callback_t func, void *ctx)
{
    size_t len;
    int idx = _get_complete_chunk(rb);
    if (idx < 0) {
        return false;
    }

    len = rb->chunk_len[idx];

    if (rb->chunk_start[idx] + len <= rb->buffer_end) {
        /* chunk is continuous */
        func(ctx, rb->chunk_start[idx], len);
    } else {
        /* chunk wraps around */
        size_t len_0 = 1 + rb->buffer_end - rb->chunk_start[idx];
        func(ctx, rb->chunk_start[idx], len_0);
        func(ctx, rb->buffer, len - len_0);
    }

    return true;
}

bool crb_consume_chunk(chunk_ringbuf_t *rb, void *dst, size_t len)
{
    int idx = _get_complete_chunk(rb);
    if (idx < 0) {
        return false;
    }

    if (len > rb->chunk_len[idx]) {
        len = rb->chunk_len[idx];
    }

    if (dst) {
        if (rb->chunk_start[idx] + len <= rb->buffer_end) {
            /* chunk is continuous */
            memcpy(dst, rb->chunk_start[idx], len);
        } else {
            /* chunk wraps around */
            uint8_t *dst8 = dst;
            size_t len_0 = 1 + rb->buffer_end - rb->chunk_start[idx];
            memcpy(dst8, rb->chunk_start[idx], len_0);
            memcpy(dst8 + len_0, rb->buffer, len - len_0);
        }
    }

    unsigned state = irq_disable();

    rb->chunk_start[idx] = NULL;

    /* advance protect marker */
    idx = _get_complete_chunk(rb);
    if (idx < 0) {
        rb->protect = rb->cur_start;
    } else {
        rb->protect = rb->chunk_start[idx];
    }

    /* advance first used slot nr */
    rb->chunk_cur = (rb->chunk_cur + 1) % CONFIG_CHUNK_NUM_MAX;

    irq_restore(state);

    return true;
}

void crb_init(chunk_ringbuf_t *rb, void *buffer, size_t len)
{
    memset(rb ,0, sizeof(*rb));
    rb->buffer = buffer;
    rb->buffer_end = &rb->buffer[len - 1];
    rb->cur = rb->buffer;
}

/** @} */

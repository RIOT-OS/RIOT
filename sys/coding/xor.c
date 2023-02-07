/*
 * Copyright (C) 2021 Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_coding_xor
 * @brief       XOR coding algorithm
 *
 * @{
 *
 * @file
 * @brief   XOR coding implementation
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <string.h>
#include "bitfield.h"
#include "coding/xor.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _gen_parity(const void *data, size_t len, uint8_t *out)
{
    const uint8_t *in = data;

    memset(out, 0, CODING_XOR_PARITY_LEN(len));
    for (unsigned i = 0; i < len; ++i) {
        out[i / CONFIG_CODING_XOR_CHECK_BYTES] ^= in[i];
    }
}

static inline size_t _transpose_idx(size_t i, size_t width, size_t height)
{
    size_t x = i % width;
    size_t y = i / width;

    return x * height + y;
}

static inline void _swap(uint8_t *a, uint8_t *b)
{
    uint8_t tmp = *a;
    *a = *b;
    *b = tmp;
}

/* https://www.geeksforgeeks.org/inplace-m-x-n-size-matrix-transpose/ */
static void _transpose(uint8_t *data, size_t len, size_t height)
{
    BITFIELD(visited, len);
    memset(visited, 0, sizeof(visited));

    /* A[0] and A[size-1] won't move */
    size_t i = 1;
    size_t last = len -1;
    bf_set(visited, 0);
    bf_set(visited, last);

    while (i < last) {
        size_t cycle_begin = i;
        uint8_t tmp = data[i];
        do {
            /* Input matrix [r x c]
             * Output matrix
             * i_new = (i*r)%(N-1)
             */
            size_t next = (i * height) % last;
            _swap(&data[next], &tmp);
            bf_set(visited, i);
            i = next;
        } while (i != cycle_begin);

        /* Get Next Move (what about querying random location?) */
        i = bf_find_first_unset(visited, len);
    }
}

static inline void _mix(void *data, size_t len)
{
    _transpose(data, len, len / CONFIG_CODING_XOR_CHECK_BYTES);
}

static inline void _unmix(void *data, size_t len)
{
    _transpose(data, len, CONFIG_CODING_XOR_CHECK_BYTES);
}

static bool _recover_byte(const uint8_t *in, size_t width, uint8_t height,
                          const uint8_t *parity, size_t idx, uint8_t *bitfield,
                          size_t block_size, uint8_t *out)
{
    uint8_t res = parity[idx / CONFIG_CODING_XOR_CHECK_BYTES];
    size_t start = idx - idx % CONFIG_CODING_XOR_CHECK_BYTES;

    for (unsigned i = start; i < start + CONFIG_CODING_XOR_CHECK_BYTES; ++i) {
        /* skip the lost byte */
        if (i == idx) {
            continue;
        }

        /* get index of neighbor byte in transposed matrix */
        size_t idx_in = _transpose_idx(i, height, width);
        if (!bf_isset(bitfield, idx_in / block_size)) {
            DEBUG("missing chunk %u\n", idx_in / block_size);
            return false;
        }
        res ^= in[idx_in];
    }

    *out = res;
    return true;
}

static bool _recover_blocks(void *data, size_t len, const uint8_t *parity,
                            uint8_t *bitfield, size_t block_size)
{
    uint8_t *in = data;

    const uint8_t height = CONFIG_CODING_XOR_CHECK_BYTES;
    const size_t width = len / height;
    const uint8_t num_data_blocks = len / block_size;
    bool success = true;

    for (size_t i = 0; i < len; i += block_size) {
        /* block is present, nothing to do */
        if (bf_isset(bitfield, i / block_size)) {
            continue;
        }

        DEBUG("try to recover chunk %u / %u\n", i / block_size, num_data_blocks);
        for (size_t j = i; j < i + block_size; ++j) {

            /* get original byte position */
            size_t idx = _transpose_idx(j, width, height);

            /* we can only recover the byte if we have the matching parity block */
            size_t parity_block = idx / (CONFIG_CODING_XOR_CHECK_BYTES * block_size);
            if (!bf_isset(bitfield, num_data_blocks + parity_block)) {
                DEBUG("missing parity block %u\n", parity_block);
                success = false;
                goto next_block;
            }

            /* try to recover lost byte from parity */
            if (!_recover_byte(in, width, height, parity, idx,
                               bitfield, block_size, &in[j])) {
                success = false;
                goto next_block;
            }
        }
        bf_set(bitfield, i / block_size);

next_block:
        /* try to recover another block */ ;
    }

    return success;
}

void coding_xor_generate(void *data, size_t len, uint8_t *parity)
{
    _gen_parity(data, len, parity);
    _mix(data, len);
}

bool coding_xor_recover(void *data, size_t len, uint8_t *parity,
                        uint8_t *bitfield, size_t block_size, bool recover_parity)
{
    size_t num_data_chunks   = len / block_size;
    size_t num_parity_chunks = CODING_XOR_PARITY_LEN(len) / block_size;

    if (!_recover_blocks(data, len, parity, bitfield, block_size)) {
        return false;
    }

    _unmix(data, len);

    if (!recover_parity) {
        return true;
    }

    /* recover lost parity blocks */
    for (size_t i = 0; i < num_parity_chunks; ++i) {
        if (bf_isset(bitfield, num_data_chunks + i)) {
            continue;
        }

        DEBUG("regenerate parity block %u\n", i);
        size_t data_len = block_size * CONFIG_CODING_XOR_CHECK_BYTES;
        _gen_parity((uint8_t *)data + i * data_len,
                              data_len, parity + i * block_size);
    }

    return true;
}

/** @} */

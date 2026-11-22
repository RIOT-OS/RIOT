/*
 * SPDX-FileCopyrightText: 2026 Harshita
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_huffman_ee
 * @{
 *
 * @file
 * @brief       Turning symbols into bits and back
 *
 * Bits are packed into bytes starting at the most significant bit, so the
 * first transmitted bit is bit 7 of byte 0.
 *
 * @author      Harshita <bansalharshita3012@gmail.com>
 */

#include <errno.h>
#include <string.h>

#include "huffman_ee/huffman_ee.h"
#include "huffman_ee_internal.h"

int huffman_ee_encode(const huffman_ee_table_t *table, const uint16_t *input,
                      size_t input_len, void *out, size_t out_size,
                      size_t *out_bits)
{
    if ((table == NULL) || (out == NULL) || (out_bits == NULL) ||
        ((input == NULL) && (input_len > 0))) {
        return -EINVAL;
    }

    uint8_t *buf = out;
    size_t pos = 0;                     /* number of bits written so far */
    size_t capacity = out_size * 8;     /* number of bits that fit in buf */

    /* only 1 bits are written below, so start with all zeros */
    memset(buf, 0, out_size);

    for (size_t i = 0; i < input_len; i++) {
        int idx = _table_find(table, input[i]);

        if (idx < 0) {
            return -EINVAL;
        }
        const huffman_ee_code_t *code = &table->code[idx];

        if (pos + code->length > capacity) {
            return -ENOBUFS;
        }
        /* write the codeword from its first bit to its last bit */
        for (int b = code->length - 1; b >= 0; b--) {
            if ((code->bits >> b) & 1) {
                buf[pos >> 3] |= (uint8_t)(0x80U >> (pos & 7));
            }
            pos++;
        }
    }
    *out_bits = pos;
    return 0;
}

/* Index of the symbol whose codeword is the lowest @p length bits of @p bits */
static int _table_match(const huffman_ee_table_t *table, uint32_t bits, uint8_t length)
{
    for (uint16_t i = 0; i < table->num_symbols; i++) {
        if ((table->code[i].length == length) && (table->code[i].bits == bits)) {
            return i;
        }
    }
    return -1;
}

int huffman_ee_decode(const huffman_ee_table_t *table, const void *in,
                      size_t in_bits, uint16_t *out, size_t out_max,
                      size_t *out_len)
{
    if ((table == NULL) || (out_len == NULL) || ((in == NULL) && (in_bits > 0)) ||
        ((out == NULL) && (out_max > 0))) {
        return -EINVAL;
    }

    const uint8_t *buf = in;
    uint32_t pending = 0;       /* bits read since the last decoded symbol */
    uint8_t pending_len = 0;
    size_t produced = 0;

    for (size_t pos = 0; pos < in_bits; pos++) {
        /* no codeword is longer than this, so the data must be corrupt */
        if (pending_len >= HUFFMAN_EE_MAX_CODE_LEN) {
            return -EINVAL;
        }
        pending = (pending << 1) | ((buf[pos >> 3] >> (7 - (pos & 7))) & 1);
        pending_len++;

        /* Huffman codes are prefix-free: the first match is the symbol */
        int idx = _table_match(table, pending, pending_len);
        if (idx >= 0) {
            if (produced >= out_max) {
                return -ENOBUFS;
            }
            out[produced++] = table->symbol[idx];
            pending = 0;
            pending_len = 0;
        }
    }

    /* the data ended in the middle of a codeword */
    if (pending_len != 0) {
        return -EINVAL;
    }
    *out_len = produced;
    return 0;
}

/** @} */

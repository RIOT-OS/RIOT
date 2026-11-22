/*
 * SPDX-FileCopyrightText: 2026 Harshita
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_huffman_ee
 * @{
 *
 * @file
 * @brief       Small helpers shared by the files of the huffman_ee module
 *
 * Not part of the public API.
 *
 * @author      Harshita <bansalharshita3012@gmail.com>
 */

#include <stdint.h>

#include "bitarithm.h"
#include "huffman_ee/huffman_ee.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of `1` bits in a codeword
 */
static inline uint32_t _count_ones(uint32_t bits)
{
    return bitarithm_bits_set_u32(bits);
}

/**
 * @brief   Number of 0/1 changes between neighbouring bits of a codeword
 *
 * Example: `0110` has two changes (0->1 and 1->0).
 *
 * `bits ^ (bits >> 1)` has a 1 wherever a bit differs from the bit next to
 * it. The mask drops the top position, which would otherwise compare the
 * first bit of the codeword with a made-up 0.
 *
 * @param[in] bits      the codeword
 * @param[in] length    number of valid bits in @p bits (at most 32)
 */
static inline uint32_t _count_transitions(uint32_t bits, uint8_t length)
{
    if (length < 2) {
        return 0;
    }
    uint32_t mask = (UINT32_C(1) << (length - 1)) - 1;
    return bitarithm_bits_set_u32((bits ^ (bits >> 1)) & mask);
}

/**
 * @brief   The bit of a codeword that is transmitted first
 */
static inline uint32_t _first_bit(const huffman_ee_code_t *code)
{
    return (code->bits >> (code->length - 1)) & 1;
}

/**
 * @brief   The bit of a codeword that is transmitted last
 */
static inline uint32_t _last_bit(const huffman_ee_code_t *code)
{
    return code->bits & 1;
}

/**
 * @brief   Find the table index of a symbol value
 *
 * @return  the index, or -1 if the symbol is not in the table
 */
static inline int _table_find(const huffman_ee_table_t *table, uint16_t symbol)
{
    for (uint16_t i = 0; i < table->num_symbols; i++) {
        if (table->symbol[i] == symbol) {
            return i;
        }
    }
    return -1;
}

#ifdef __cplusplus
}
#endif

/** @} */

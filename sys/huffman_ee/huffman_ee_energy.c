/*
 * SPDX-FileCopyrightText: 2026 Harshita
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_huffman_ee
 * @{
 *
 * @file
 * @brief       Energy of an encoded stream (Phase 4 of Algorithm 1 in the
 *              paper)
 *
 * The energy model is
 *
 *     E = alpha * N1 + beta * Nt + lambda * Ns
 *
 *  - N1: number of 1 bits in the stream
 *  - Nt: number of 0/1 changes inside the codewords
 *  - Ns: number of 0/1 changes where one codeword ends and the next one
 *        starts (last bit of one codeword differs from the first bit of the
 *        next)
 *
 * N1 and Nt are the same for every occurrence of a symbol, so they could be
 * worked out from the frequencies alone. Ns depends on the order of the
 * symbols, so the whole stream has to be looked at.
 *
 * @author      Harshita <bansalharshita3012@gmail.com>
 */

#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "huffman_ee/huffman_ee.h"
#include "huffman_ee_internal.h"

int huffman_ee_count(const huffman_ee_table_t *table, const uint16_t *input,
                     size_t input_len, huffman_ee_counts_t *counts)
{
    if ((table == NULL) || (counts == NULL) || ((input == NULL) && (input_len > 0))) {
        return -EINVAL;
    }

    memset(counts, 0, sizeof(*counts));

    bool have_previous = false;
    uint32_t previous_last_bit = 0;

    for (size_t i = 0; i < input_len; i++) {
        int idx = _table_find(table, input[i]);

        if (idx < 0) {
            return -EINVAL;
        }
        const huffman_ee_code_t *code = &table->code[idx];

        counts->ones += _count_ones(code->bits);
        counts->transitions += _count_transitions(code->bits, code->length);

        /* compare with the codeword that was sent just before this one */
        if (have_previous && (previous_last_bit != _first_bit(code))) {
            counts->boundaries++;
        }
        previous_last_bit = _last_bit(code);
        have_previous = true;
    }
    return 0;
}

uint64_t huffman_ee_energy(const huffman_ee_counts_t *counts,
                           const huffman_ee_params_t *params)
{
    return ((uint64_t)params->alpha * counts->ones) +
           ((uint64_t)params->beta * counts->transitions) +
           ((uint64_t)params->lambda * counts->boundaries);
}

/** @} */

/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_entropy_source_common
 *
 * @{
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "entropy_source.h"

int entropy_source_neumann_unbias(entropy_source_sample_func_t func,
                                  uint8_t *out, size_t len)
{
    assert(func != NULL && out != NULL);

    uint8_t old_sample, new_sample, sample_out = 0;
    uint8_t bit1, bit2, bit_pos = 0;
    size_t bytes_count = 0;
    size_t sample_count = 0;
    size_t abort = len * CONFIG_ENTROPY_SOURCE_NEUMANN_ABORT;
    int tmp, ret = ENTROPY_SOURCE_OK;

    /* Get initial sample */
    tmp = func(&old_sample);

    /* Only return in case of failed configuration */
    if (ret == ENTROPY_SOURCE_ERR_CONFIG) {
        return ret;
    }

    while (bytes_count < len) {
        /* Increment sample counter and abort if
         * exceeds maximum number of iterations */
        sample_count++;
        if (sample_count > abort) {
            return ENTROPY_SOURCE_ERR_COND;
        }

        /* Get next sample */
        tmp = func(&new_sample);

        /* Remember the worst failure during
         * sampling multiple values to return */
        if (tmp < ret) {
            ret = tmp;
        }

        /* Iterate each bit in sample */
        for (unsigned j = 0; j < 8; j++) {
            bit1 = (1 << j) & old_sample;
            bit2 = (1 << j) & new_sample;

            /* Only save information if change occurred
             * 0/1 change results in 1
             * 1/0 change results in 0
             */
            if (bit1 < bit2) {
                sample_out |= (1 << bit_pos++);
            }
            else if (bit1 > bit2) {
                sample_out &= ~(1 << bit_pos++);
            }

            /* Once 8 bits have been gathered, write to output */
            if (bit_pos == 8) {
                out[bytes_count] = sample_out;
                bit_pos = 0;
                bytes_count++;
            }
        }
        /* Store recent sample for next iteration */
        old_sample = new_sample;
    }

    return ret;
}

int entropy_source_test_rep(entropy_source_tests_rep_t *state, uint8_t sample)
{
    assert(state != NULL);

    if (sample == state->old_sample && state->cnt_rep > 0) {
        state->cnt_rep++;
        if (state->cnt_rep >= state->c_rep) {
            return ENTROPY_SOURCE_ERR_TEST_REP;
        }
    }
    else {
        state->old_sample = sample;
        state->cnt_rep = 1;
    }
    return ENTROPY_SOURCE_OK;
}

int entropy_source_test_prop(entropy_source_tests_prop_t *state, uint8_t sample)
{
    assert(state != NULL);

    if (state->cnt_window == CONFIG_ENTROPY_SOURCE_TESTS_WIN) {
        state->old_sample = sample;
        state->cnt_prop = 0;
        state->cnt_window = 0;
    }
    else {
        state->cnt_window++;
        if (state->old_sample == sample) {
            state->cnt_prop++;
        }
        if (state->cnt_prop >= state->c_prop) {
            return ENTROPY_SOURCE_ERR_TEST_PROP;
        }
    }
    return ENTROPY_SOURCE_OK;
}

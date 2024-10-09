/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include "matstat.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void matstat_clear(matstat_state_t *state)
{
    *state = MATSTAT_STATE_INIT;
}

void matstat_add(matstat_state_t *state, int32_t value)
{
    if (value > state->max) {
        state->max = value;
    }
    if (value < state->min) {
        state->min = value;
    }
    state->sum += value;
    /* Using Welford's algorithm for variance */
    ++state->count;
    if (state->count == 1) {
        state->sum_sq = 0;
        state->mean = value;
    }
    else {
        int32_t new_mean = state->sum / state->count;
        int64_t diff = (value - state->mean) * (value - new_mean);
        if ((diff < 0) && ((uint64_t)(-diff) > state->sum_sq)) {
            /* Handle corner cases where sum_sq becomes negative */
            state->sum_sq = 0;
        }
        else {
            state->sum_sq += diff;
        }
        state->mean = new_mean;
    }
}

uint64_t matstat_variance(const matstat_state_t *state)
{
    if (state->count < 2) {
        /* We don't have any way of returning an error */
        return 0;
    }
    uint64_t variance = state->sum_sq / (state->count - 1);
#ifdef PRIu64
    DEBUG("Var: (%" PRIu64 " / (%" PRId32 " - 1)) = %" PRIu64 "\n",
        state->sum_sq, state->count, variance);
#else
    DEBUG("Var: (%" PRIu32 "%s / (%" PRId32 " - 1)) = %" PRIu32 "%s\n",
          (uint32_t)state->sum_sq, (state->sum_sq > UINT32_MAX) ? "!trunc " : "",
          state->count,
          (uint32_t)variance, (variance > UINT32_MAX) ? "!trunc " : "");
#endif
    return variance;
}

void matstat_merge(matstat_state_t *dest, const matstat_state_t *src)
{
    if (src->count == 0) {
        /* src is empty, no-op */
        return;
    }
    if (dest->count == 0) {
        /* dest is empty, straight copy */
        *dest = *src;
        return;
    }
    /* Combining the variance of the two samples needs some extra
     * handling if the means are different between the two states,
     * source: https://stats.stackexchange.com/a/43183
     * (using sum_sq = sigma2 * n, instead of sum_sq = sigma2 * (n-1) to simplify algorithm)
     */
    dest->sum_sq = (dest->sum_sq + dest->sum * dest->mean + src->sum_sq + src->sum * src->mean);
    dest->count += src->count;
    dest->sum += src->sum;
    int32_t new_mean = dest->sum / dest->count;
    int64_t diff = -new_mean * dest->sum;
    if ((diff < 0) && ((uint64_t)(-diff) > dest->sum_sq)) {
        /* Handle corner cases where sum_sq becomes negative */
        dest->sum_sq = 0;
    }
    else {
        dest->sum_sq += diff;
    }
    dest->mean = new_mean;
    if (src->max > dest->max) {
        dest->max = src->max;
    }
    if (src->min < dest->min) {
        dest->min = src->min;
    }
}

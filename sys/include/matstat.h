/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_matstat
 *
 * The Matstat library uses single pass algorithms to compute statistic measures
 * such as mean and variance over many values. The values can be immediately
 * discarded after processing, keeping the memory requirement constant
 * regardless of how many values need to be processed.
 *
 * The design goal is to provide basic mathematical statistics operations on
 * constrained devices with a "good enough" accuracy to be able to provide some
 * descriptive measures of data. For more accurate measures of statistics, use a
 * fancier library, or copy the data to a PC.
 *
 * It is important to know that using integer operations will result in lower
 * precision in the computed measures because of truncation.
 *
 * @{
 * @file
 * @brief       Matstat library declarations
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef MATSTAT_H
#define MATSTAT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Internal state for computing running statistics
 */
typedef struct {
    int64_t sum;        /**< Sum of values added */
    uint64_t sum_sq;    /**< Sum of squared differences */
    uint32_t count;     /**< Number of values added */
    int32_t mean;       /**< Mean value */
    int32_t min;        /**< Minimum value seen */
    int32_t max;        /**< Maximum value seen */
} matstat_state_t;

/**
 * @brief   Empty state initializer
 */
#define MATSTAT_STATE_INIT (const matstat_state_t) { \
        .sum = 0, \
        .sum_sq = 0, \
        .count = 0, \
        .mean = 0, \
        .min = INT32_MAX, \
        .max = INT32_MIN, \
    }

/**
 * @brief   Reset state
 *
 * @param[in]   state   State struct to clear
 */
void matstat_clear(matstat_state_t *state);

/**
 * @brief   Add a sample to state
 *
 * @param[in]   state   State struct to operate on
 * @param[in]   value   Value to add to the state
 */
void matstat_add(matstat_state_t *state, int32_t value);

/**
 * @brief   Return the computed mean value of all samples so far
 *
 * @param[in]   state   State struct to operate on
 *
 * @return  arithmetic mean
 */
static inline int32_t matstat_mean(const matstat_state_t *state)
{
    return state->mean;
}

/**
 * @brief   Compute the sample variance of all samples so far
 *
 * @param[in]   state   State struct to operate on
 *
 * @return  sample variance
 */
uint64_t matstat_variance(const matstat_state_t *state);

/**
 * @brief   Combine two states
 *
 * Add the sums and count of @p src and @p dest, take the maximum of the max
 * values and minimum of the min values. The result is written to @p dest.
 *
 * @param[inout]    dest    destination state struct
 * @param[out]      src     source state struct
 */
void matstat_merge(matstat_state_t *dest, const matstat_state_t *src);

#ifdef __cplusplus
}
#endif

#endif /* MATSTAT_H */

/** @} */

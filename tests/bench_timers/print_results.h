/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Result printing function declarations
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef PRINT_RESULTS_H
#define PRINT_RESULTS_H

#include "matstat.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Test limits */
typedef struct {
    int32_t mean_low;
    int32_t mean_high;
    uint32_t variance_low;
    uint32_t variance_high;
} stat_limits_t;

typedef struct  {
    const char *label;
    const char **sub_labels;
    unsigned num_sub_labels;
} result_group_t;

typedef struct {
    const result_group_t *groups;
    unsigned num_groups;
    const stat_limits_t *ref_limits; /* Reference stats limits */
    const stat_limits_t *int_limits; /* Introspective stats limits */
    const unsigned *offsets; /* Pointer to array of input offsets, for correctly printing the detailed stats */
} result_presentation_t;

/**
 * @brief   Present the results of the benchmark
 *
 * Depends on DETAILED_STATS, LOG2_STATS
 * @param[in]   ref_states  State vector from reference measurements
 * @param[in]   int_states  State vector from introspective measurements
 */
void print_results(const result_presentation_t *pres, const matstat_state_t *ref_states, const matstat_state_t *int_states);

#ifdef __cplusplus
}
#endif

#endif /* PRINT_RESULTS_H */

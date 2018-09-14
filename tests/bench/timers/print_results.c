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
 * @brief       Another peripheral timer test application
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "print_results.h"
#include "matstat.h"
#include "fmt.h"
#include "bench_timers_config.h"

static void print_statistics(const matstat_state_t *state, const stat_limits_t *limits)
{
    if (state->count == 0) {
        print_str("no samples\n");
        return;
    }
    if (state->count == 1) {
        print_str("single sample: ");
        print_s32_dec((int32_t)state->sum);
        print("\n", 1);
        return;
    }
    int32_t mean = matstat_mean(state);
    uint64_t variance = matstat_variance(state);

    char buf[20];
    print(buf, fmt_lpad(buf, fmt_u32_dec(buf, state->count), 8, ' '));
    print(" ", 1);
    print(buf, fmt_lpad(buf, fmt_s64_dec(buf, state->sum), 9, ' '));
    print(" ", 1);
    print(buf, fmt_lpad(buf, fmt_u64_dec(buf, state->sum_sq), 12, ' '));
    print(" ", 1);
    print(buf, fmt_lpad(buf, fmt_s32_dec(buf, state->min), 6, ' '));
    print(" ", 1);
    print(buf, fmt_lpad(buf, fmt_s32_dec(buf, state->max), 5, ' '));
    print(" ", 1);
    print(buf, fmt_lpad(buf, fmt_s32_dec(buf, mean), 5, ' '));
    print(" ", 1);
    print(buf, fmt_lpad(buf, fmt_u64_dec(buf, variance), 6, ' '));
    if (limits) {
        if ((mean < limits->mean_low) || (limits->mean_high < mean) ||
            (variance < limits->variance_low) || (limits->variance_high < variance) ) {
            /* mean or variance is outside the expected range, alert the user */
            print_str("  <=== SIC!");
        }
    }
    print("\n", 1);
}

static void print_totals(const matstat_state_t *states, size_t nelem, const stat_limits_t *limits)
{
    matstat_state_t totals;
    matstat_clear(&totals);
    for (size_t k = 0; k < nelem; ++k) {
        matstat_merge(&totals, &states[k]);
    }
    print_statistics(&totals, limits);
}

static void print_detailed(const matstat_state_t *states, size_t nelem, unsigned int test_min, const stat_limits_t *limits)
{
    if (LOG2_STATS) {
        print_str("   interval     count       sum       sum_sq    min   max  mean  variance\n");
        for (unsigned int k = 0; k < nelem; ++k) {
            char buf[20];
            unsigned int num = (1 << k);
            if (num >= TEST_NUM) {
                break;
            }
            unsigned int start = num + test_min;
            if (num == 1) {
                /* special case, bitarithm_msb will return 0 for both 0 and 1 */
                start = test_min;
            }
            print(buf, fmt_lpad(buf, fmt_u32_dec(buf, start), 4, ' '));
            print_str(" - ");
            print(buf, fmt_lpad(buf, fmt_u32_dec(buf, test_min + (num * 2) - 1), 4, ' '));
            print_str(": ");
            print_statistics(&states[k], limits);
        }
        print_str("      TOTAL  ");
    }
    else {
        print_str("interval    count       sum       sum_sq    min   max  mean  variance\n");
        for (unsigned int k = 0; k < nelem; ++k) {
            char buf[10];
            print(buf, fmt_lpad(buf, fmt_u32_dec(buf, k + test_min), 7, ' '));
            print_str(": ");
            print_statistics(&states[k], limits);
        }
        print_str("  TOTAL: ");
    }
    print_totals(states, nelem, limits);
}

void print_results(const result_presentation_t *pres, const matstat_state_t *ref_states, const matstat_state_t *int_states)
{
    static char buf[48]; /* String formatting temporary buffer, not thread safe */
    print_str("------------- BEGIN STATISTICS --------------\n");
    print_str("===== Reference timer statistics =====\n");
    print_str("Limits: mean: [");
    print_s32_dec(pres->ref_limits->mean_low);
    print_str(", ");
    print_s32_dec(pres->ref_limits->mean_high);
    print_str("], variance: [");
    print_u32_dec(pres->ref_limits->variance_low);
    print_str(", ");
    print_u32_dec(pres->ref_limits->variance_high);
    print_str("]\n");
    print_str("Target error (actual trigger time - expected trigger time), in reference timer ticks\n");
    print_str("positive: timer under test is late, negative: timer under test is early\n");

    if (DETAILED_STATS) {
        static const unsigned int count = ((LOG2_STATS) ? (TEST_LOG2NUM) : (TEST_NUM));
        unsigned k = 0;
        for (unsigned g = 0; g < pres->num_groups; ++g) {
            for (unsigned c = 0; c < pres->groups[g].num_sub_labels; ++c) {
                print_str("=== ");
                print_str(pres->groups[g].label);
                print(" ", 1);
                print_str(pres->groups[g].sub_labels[c]);
                print_str(" ===\n");
                print_detailed(&ref_states[k * count], count, pres->offsets[k], pres->ref_limits);
                ++k;
            }
        }
    }
    else {
        print_str("function              count       sum       sum_sq    min   max  mean  variance\n");
        unsigned k = 0;
        for (unsigned g = 0; g < pres->num_groups; ++g) {
            print("\n", 1);
            print(buf, fmt_lpad(buf, fmt_str(buf, pres->groups[g].label), 18, ' '));
            print(" ", 1);
            print_totals(&ref_states[k], pres->groups[g].num_sub_labels, pres->ref_limits);
            for (unsigned c = 0; c < pres->groups[g].num_sub_labels; ++c) {
                print(buf, fmt_lpad(buf, fmt_str(buf, pres->groups[g].sub_labels[c]), 18, ' '));
                print(" ", 1);
                print_statistics(&ref_states[k], pres->ref_limits);
                ++k;
            }
        }
    }
    print_str("===== introspective statistics =====\n");
    print_str("Limits: mean: [");
    print_s32_dec(pres->int_limits->mean_low);
    print_str(", ");
    print_s32_dec(pres->int_limits->mean_high);
    print_str("], variance: [");
    print_u32_dec(pres->int_limits->variance_low);
    print_str(", ");
    print_u32_dec(pres->int_limits->variance_high);
    print_str("]\n");
    print_str("self-referencing error (TUT time elapsed - expected TUT interval), in timer under test ticks\n");
    print_str("positive: timer target handling is slow, negative: TUT is dropping ticks or triggering callback early\n");
    print_str("function              count       sum       sum_sq    min   max  mean  variance\n");
    for (unsigned k = 0, g = 0; g < pres->num_groups; ++g) {
        print("\n", 1);
        print(buf, fmt_lpad(buf, fmt_str(buf, pres->groups[g].label), 18, ' '));
        print(" ", 1);
        print_totals(&int_states[k], pres->groups[g].num_sub_labels, pres->int_limits);
        for (unsigned c = 0; c < pres->groups[g].num_sub_labels; ++c) {
            print(buf, fmt_lpad(buf, fmt_str(buf, pres->groups[g].sub_labels[c]), 18, ' '));
            print(" ", 1);
            print_statistics(&int_states[k], pres->int_limits);
            ++k;
        }
    }

    print_str("-------------- END STATISTICS ---------------\n");
}

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

#include <stddef.h>
#include <stdint.h>
#include "board.h"
#include "cpu.h"
#include "periph/timer.h"

#include "fmt.h"
#include "print_results.h"
#include "spin_random.h"
#include "bench_timers_config.h"
#include "thread_flags.h"

#ifndef TEST_TRACE
#define TEST_TRACE 0
#endif

/*
 * Results will be grouped by function, rescheduling yes/no, start/stop.
 * functions: timer_set, timer_set_absolute
 * reschedule: yes/no, when yes: first set one target time, before that time has
 *             passed, set the real target time
 * start/stop: if stop: call timer_stop before setting the target time, then call timer_start
 */
enum test_periph_timer_variants {
    TEST_RESCHEDULE         = 1,
    TEST_STOPPED            = 2,
    TEST_ABSOLUTE           = 4,
};

/* Number of variant groups, used when printing results */
#define TEST_VARIANT_GROUPS 2

const result_presentation_t test_periph_timer_presentation = {
    .groups = (const result_group_t[TEST_VARIANT_GROUPS]) {
        {
            .label = "timer_set",
            .sub_labels = (const char *[]){
                [0] = "running",
                [TEST_RESCHEDULE] = "resched",
                [TEST_STOPPED] = "stopped",
                [TEST_STOPPED | TEST_RESCHEDULE] = "stopped, resched",
            },
            .num_sub_labels = (TEST_VARIANT_NUMOF / TEST_VARIANT_GROUPS),
        },
        {
            .label = "timer_set_absolute",
            .sub_labels = (const char *[]){
                [0] = "running",
                [TEST_RESCHEDULE] = "resched",
                [TEST_STOPPED] = "stopped",
                [TEST_STOPPED | TEST_RESCHEDULE] = "stopped, resched",
            },
            .num_sub_labels = (TEST_VARIANT_NUMOF / TEST_VARIANT_GROUPS),
        },
    },
    .num_groups = TEST_VARIANT_GROUPS,
    .ref_limits = &bench_timers_ref_limits,
    .int_limits = &bench_timers_int_limits,
    .offsets = (const unsigned[]) {
        [0] =                                               TEST_MIN_REL,
        [TEST_RESCHEDULE] =                                 TEST_MIN_REL,
        [TEST_STOPPED] =                                    TEST_MIN_REL,
        [TEST_STOPPED | TEST_RESCHEDULE] =                  TEST_MIN_REL,
        [TEST_ABSOLUTE] =                                   TEST_MIN,
        [TEST_ABSOLUTE | TEST_RESCHEDULE] =                 TEST_MIN,
        [TEST_ABSOLUTE | TEST_STOPPED] =                    TEST_MIN,
        [TEST_ABSOLUTE | TEST_STOPPED | TEST_RESCHEDULE] =  TEST_MIN,
    },
};

void test_periph_timer_run(test_ctx_t *ctx, uint32_t interval, unsigned int variant)
{
    if (variant & TEST_ABSOLUTE) {
        interval += TEST_MIN;
    }
    else {
        interval += TEST_MIN_REL;
    }
    unsigned int interval_ref = TIM_TEST_TO_REF(interval);

    if (TEST_TRACE) {
        if (variant & TEST_ABSOLUTE) {
            print_str("A");
        }
        else {
            print_str("_");
        }
        if (variant & TEST_RESCHEDULE) {
            print_str("R");
        }
        else {
            print_str("_");
        }
        if (variant & TEST_STOPPED) {
            print_str("S ");
        }
        else {
            print_str("_ ");
        }
        print_u32_dec(interval);
        print_str("   ");
        print_u64_hex(TUT_READ());
        print("\n", 1);
    }

    spin_random_delay();
    if (variant & TEST_RESCHEDULE) {
        timer_set(TIM_TEST_DEV, TIM_TEST_CHAN, interval + RESCHEDULE_MARGIN);
        spin_random_delay();
    }
    if (variant & TEST_STOPPED) {
        timer_stop(TIM_TEST_DEV);
        spin_random_delay();
    }
    ctx->target_ref = timer_read(TIM_REF_DEV) + interval_ref;
    ctx->target_tut = TUT_READ() + interval;
    if (variant & TEST_ABSOLUTE) {
        timer_set_absolute(TIM_TEST_DEV, TIM_TEST_CHAN, ctx->target_tut);
    }
    else {
        timer_set(TIM_TEST_DEV, TIM_TEST_CHAN, interval);
    }
    if (variant & TEST_STOPPED) {
        spin_random_delay();
        /* do not update ctx->target_tut, because TUT should have been stopped
         * and not incremented during spin_random_delay */
        ctx->target_ref = timer_read(TIM_REF_DEV) + interval_ref;
        timer_start(TIM_TEST_DEV);
    }
    thread_flags_wait_any(THREAD_FLAG_TEST);
}

/* Wrapper for periph_timer callbacks */
static void cb_timer_periph(void *arg, int chan)
{
    (void)chan;
    bench_timers_cb(arg);
}

int test_periph_timer_init(test_ctx_t *ctx)
{
    int res = timer_init(TIM_TEST_DEV, TIM_TEST_FREQ, cb_timer_periph, ctx);
    return res;
}

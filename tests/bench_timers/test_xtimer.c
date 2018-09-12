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

#include "thread_flags.h"
#include "fmt.h"
#include "xtimer.h"
#include "print_results.h"
#include "spin_random.h"
#include "bench_timers_config.h"

#ifndef TEST_TRACE
#define TEST_TRACE 0
#endif

enum test_xtimer_variants {
    TEST_XTIMER_SET             = 0,
    TEST_PARALLEL               = 1,
    TEST_XTIMER_SET_ABSOLUTE    = 2,
    TEST_XTIMER_PERIODIC_WAKEUP = 4,
    TEST_XTIMER_SPIN            = 6,
    TEST_XTIMER_VARIANT_NUMOF   = 8,
};

const result_presentation_t test_xtimer_presentation = {
    .groups = (const result_group_t[1]) {
        {
            .label = "xtimer",
            .sub_labels = (const char *[]){
                [TEST_XTIMER_SET] = "_xt_set",
                [TEST_XTIMER_SET | TEST_PARALLEL] = "_xt_set race",
                [TEST_XTIMER_SET_ABSOLUTE] = "_xt_set_abs",
                [TEST_XTIMER_SET_ABSOLUTE | TEST_PARALLEL] = "_xt_set_abs race",
                [TEST_XTIMER_PERIODIC_WAKEUP] = "_xt_periodic",
                [TEST_XTIMER_PERIODIC_WAKEUP | TEST_PARALLEL] = "_xt_periodic race",
                [TEST_XTIMER_SPIN] = "_xt_spin",
                [TEST_XTIMER_SPIN | TEST_PARALLEL] = "_xt_spin race",
            },
            .num_sub_labels = TEST_VARIANT_NUMOF,
        },
    },
    .num_groups = 1,
    .ref_limits = &bench_timers_ref_limits,
    .int_limits = &bench_timers_int_limits,
    .offsets = (const unsigned[]) {
        [TEST_XTIMER_SET]                               = TEST_MIN_REL,
        [TEST_XTIMER_SET | TEST_PARALLEL]               = TEST_MIN_REL,
        [TEST_XTIMER_SET_ABSOLUTE]                      = TEST_MIN,
        [TEST_XTIMER_SET_ABSOLUTE | TEST_PARALLEL]      = TEST_MIN,
        [TEST_XTIMER_PERIODIC_WAKEUP]                   = TEST_MIN_REL,
        [TEST_XTIMER_PERIODIC_WAKEUP | TEST_PARALLEL]   = TEST_MIN_REL,
        [TEST_XTIMER_SPIN]                              = TEST_MIN_REL,
        [TEST_XTIMER_SPIN | TEST_PARALLEL]              = TEST_MIN_REL,
    },
};


static void cb_nop(void *arg)
{
    (void)arg;
}

void test_xtimer_run(test_ctx_t *ctx, uint32_t interval, unsigned int variant)
{
    interval += TEST_MIN;
    unsigned int interval_ref = TIM_TEST_TO_REF(interval);
    xtimer_t xt = {
        .target = 0,
        .long_target = 0,
        .callback = bench_timers_cb,
        .arg = ctx,
    };
    xtimer_t xt_parallel = {
        .target = 0,
        .long_target = 0,
        .callback = cb_nop,
        .arg = NULL,
    };
    if (TEST_TRACE) {
        switch (variant & ~TEST_PARALLEL) {
            case TEST_XTIMER_SET:
                print_str("rel ");
                break;
            case TEST_XTIMER_SET_ABSOLUTE:
                print_str("abs ");
                break;
            case TEST_XTIMER_PERIODIC_WAKEUP:
                print_str("per ");
                break;
            case TEST_XTIMER_SPIN:
                print_str("spn ");
                break;
            default:
                break;
        }
        if (variant & TEST_PARALLEL) {
            print_str("= ");
        }
        else {
            print_str("- ");
        }
        print_u32_dec(interval);
        print("\n", 1);
    }

    spin_random_delay();
    if (variant & TEST_PARALLEL) {
        _xtimer_set(&xt_parallel, interval);
        spin_random_delay();
    }
    ctx->target_ref = timer_read(TIM_REF_DEV) + interval_ref;
    uint32_t now = TUT_READ();
    ctx->target_tut = now + interval;
    switch (variant & ~TEST_PARALLEL) {
        case TEST_XTIMER_SET:
            _xtimer_set(&xt, interval);
            break;
        case TEST_XTIMER_SET_ABSOLUTE:
            now = TUT_READ();
            ctx->target_tut = now + interval;
            _xtimer_set_absolute(&xt, ctx->target_tut);
            break;
        case TEST_XTIMER_PERIODIC_WAKEUP:
            _xtimer_periodic_wakeup(&now, interval);
            /* xtimer_periodic_wakeup sleeps the thread, no automatic callback */
            bench_timers_cb(xt.arg);
            break;
        case TEST_XTIMER_SPIN:
            _xtimer_spin(interval);
            /* xtimer_spin sleeps the thread, no automatic callback */
            bench_timers_cb(xt.arg);
            break;
        default:
            break;
    }
    thread_flags_wait_any(THREAD_FLAG_TEST);
    xtimer_remove(&xt_parallel);
    xtimer_remove(&xt);
}

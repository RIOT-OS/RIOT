/*
 * SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_ztimer_overhead
 * @{
 *
 * @file
 * @brief       ztimer overhead measurement functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "ztimer.h"
#include "ztimer/overhead.h"

typedef struct {
    ztimer_clock_t *clock;
    volatile uint32_t *val;
} callback_arg_t;

static void _callback(void *arg)
{
    callback_arg_t *callback_arg = (callback_arg_t *)arg;

    *callback_arg->val = ztimer_now(callback_arg->clock);
}

int32_t ztimer_overhead_set(ztimer_clock_t *clock, uint32_t base)
{
    volatile uint32_t after = 0;
    uint32_t pre;

    callback_arg_t arg = { .clock = clock, .val = &after };
    ztimer_t t = { .callback = _callback, .arg = &arg };

    ztimer_acquire(clock);
    pre = ztimer_now(clock);
    ztimer_set(clock, &t, base);
    while (!after) {}
    ztimer_release(clock);
    return after - pre - base;
}

int32_t ztimer_overhead_sleep(ztimer_clock_t *clock, uint32_t base)
{
    ztimer_acquire(clock);
    uint32_t pre = ztimer_now(clock);
    ztimer_sleep(clock, base);
    uint32_t after = ztimer_now(clock);
    ztimer_release(clock);

    return after - pre - base;
}

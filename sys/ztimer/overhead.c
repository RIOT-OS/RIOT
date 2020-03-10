/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Freie Universität Berlin
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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

int32_t ztimer_overhead(ztimer_clock_t *clock, uint32_t base)
{
    volatile uint32_t after = 0;
    uint32_t pre;

    callback_arg_t arg = { .clock = clock, .val = &after };
    ztimer_t t = { .callback = _callback, .arg = &arg };

    pre = ztimer_now(clock);
    ztimer_set(clock, &t, base);
    while (!after) {}
    return after - pre - base;
}

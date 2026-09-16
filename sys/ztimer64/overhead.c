/*
 * SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_ztimer64
 * @{
 *
 * @file
 * @brief       ztimer64 overhead measurement functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "atomic_utils.h"
#include "ztimer64.h"

typedef struct {
    ztimer64_clock_t *clock;
    volatile uint64_t *val;
} callback_arg_t;

static void _callback(void *arg)
{
    callback_arg_t *callback_arg = (callback_arg_t *)arg;

    atomic_store_u64(callback_arg->val, ztimer64_now(callback_arg->clock));
}

int64_t ztimer64_overhead(ztimer64_clock_t *clock, uint64_t base)
{
    volatile uint64_t after = 0;
    uint64_t pre;

    callback_arg_t arg = { .clock = clock, .val = &after };
    ztimer64_t t = { .callback = _callback, .arg = &arg };

    pre = ztimer64_now(clock);
    ztimer64_set(clock, &t, base);
    while (!atomic_load_u64(&after)) {}
    return after - pre - base;
}

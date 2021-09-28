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
 * @ingroup     sys_xtimer_overhead
 * @{
 *
 * @file
 * @brief       xtimer overhead measurement functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "ztimer.h"

typedef struct {
    volatile uint32_t *val;
} callback_arg_t;

static void _callback(void *arg)
{
    callback_arg_t *callback_arg = (callback_arg_t *)arg;

    *callback_arg->val = ztimer_now(ZTIMER_USEC);
}

int32_t xtimer_overhead(uint32_t base)
{
    volatile uint32_t after = 0;
    uint32_t pre;

    callback_arg_t arg = { .val = &after };
    ztimer_t t = { .callback = _callback, .arg = &arg };

    pre = ztimer_now(ZTIMER_USEC);
    ztimer_set(ZTIMER_USEC, &t, base);
    while (!after) {}
    return after - pre - base;
}

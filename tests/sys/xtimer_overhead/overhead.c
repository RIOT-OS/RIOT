/*
 * SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "xtimer.h"

typedef struct {
    volatile uint32_t *val;
} callback_arg_t;

static void _callback(void *arg)
{
    callback_arg_t *callback_arg = (callback_arg_t *)arg;

    *callback_arg->val = xtimer_now_usec();
}

int32_t xtimer_overhead(uint32_t base)
{
    volatile uint32_t after = 0;
    uint32_t pre;

    callback_arg_t arg = { .val = &after };
    xtimer_t t = { .callback = _callback, .arg = &arg };

    pre = xtimer_now_usec();
    xtimer_set(&t, base);
    while (!after) {}
    return after - pre - base;
}

/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdbool.h>
#include "kernel_defines.h"

#include "congure_impl.h"

static unsigned _event_cb_calls;
static void *_event_cb_arg;

static void _event_cb(void *);

static const congure_quic_snd_consts_t _consts[] = {
    {
        .cong_event_cb = NULL,
        .init_wnd = 12000,
        .min_wnd = 2400,
        .init_rtt = 333,
        .max_msg_size = 1200,
        .pc_thresh = 3000,
        .granularity = 1,
        .loss_reduction_numerator = 1,
        .loss_reduction_denominator = 2,
        .inter_msg_interval_numerator = 5,
        .inter_msg_interval_denominator = 4,
    },
    {
        .cong_event_cb = _event_cb,
        .init_wnd = 12000,
        .min_wnd = 2400,
        .init_rtt = 333,
        .max_msg_size = 1200,
        .pc_thresh = 3000,
        .granularity = 1,
        .loss_reduction_numerator = 1,
        .loss_reduction_denominator = 2,
        .inter_msg_interval_numerator = 5,
        .inter_msg_interval_denominator = 4,
    },
};

int congure_test_snd_setup(congure_test_snd_t *c, unsigned id)
{
    if (id >= ARRAY_SIZE(_consts)) {
        return -1;
    }
    _event_cb_calls = 0;
    _event_cb_arg = NULL;
    congure_quic_snd_setup(c, &_consts[id]);
    return 0;
}

unsigned congure_quic_test_get_event_cb_calls(void)
{
    return _event_cb_calls;
}

void *congure_quic_test_get_event_cb_arg(void)
{
    return _event_cb_arg;
}

static void _event_cb(void *ctx)
{
    _event_cb_calls++;
    _event_cb_arg = ctx;
}

/** @} */

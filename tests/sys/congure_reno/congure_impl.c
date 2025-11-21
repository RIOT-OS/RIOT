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

static unsigned _fr_calls;
static bool _same_wnd_adv_res;

static void _fr(congure_reno_snd_t *c);
static bool _same_wnd_adv(congure_reno_snd_t *c, congure_snd_ack_t *ack);
static void _ss_cwnd_inc(congure_reno_snd_t *c);
static void _ca_cwnd_inc(congure_reno_snd_t *c);
static void _fr_cwnd_dec(congure_reno_snd_t *c);

static const congure_reno_snd_consts_t _consts[] = {
    {
        .fr = _fr,
        .same_wnd_adv = _same_wnd_adv,
        .init_mss = 1460,
        .cwnd_lower = 1095,
        .cwnd_upper = 2190,
        .init_ssthresh = CONGURE_WND_SIZE_MAX,
        .frthresh = 3,
    },
    {
        .fr = _fr,
        .same_wnd_adv = _same_wnd_adv,
        .ss_cwnd_inc = _ss_cwnd_inc,
        .ca_cwnd_inc = _ca_cwnd_inc,
        .fr_cwnd_dec = _fr_cwnd_dec,
        .init_mss = 1460,
        .cwnd_lower = 1095,
        .cwnd_upper = 2190,
        .init_ssthresh = CONGURE_WND_SIZE_MAX,
        .frthresh = 3,
    },
};

int congure_test_snd_setup(congure_test_snd_t *c, unsigned id)
{
    if (id >= ARRAY_SIZE(_consts)) {
        return -1;
    }
    _fr_calls = 0;
    congure_reno_snd_setup(c, &_consts[id]);
    return 0;
}

unsigned congure_reno_test_get_fr_calls(void)
{
    return _fr_calls;
}

void congure_reno_test_set_same_wnd_adv_res(bool value)
{
    _same_wnd_adv_res = value;
}

static void _fr(congure_reno_snd_t *c)
{
    (void)c;
    _fr_calls++;
}

static bool _same_wnd_adv(congure_reno_snd_t *c, congure_snd_ack_t *ack)
{
    (void)c;
    (void)ack;
    return _same_wnd_adv_res;
}

static void _ss_cwnd_inc(congure_reno_snd_t *c)
{
    c->super.cwnd += 1337;
}

static void _ca_cwnd_inc(congure_reno_snd_t *c)
{
    c->super.cwnd += 42;
}

static void _fr_cwnd_dec(congure_reno_snd_t *c)
{
    c->super.cwnd /= 8;
}

/** @} */

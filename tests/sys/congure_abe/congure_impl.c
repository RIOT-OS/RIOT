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

static void _fr(congure_abe_snd_t *c);
static bool _same_wnd_adv(congure_abe_snd_t *c, congure_snd_ack_t *ack);

static const congure_abe_snd_consts_t _consts[] = {
    {
        .reno = {
            .fr = _fr,
            .same_wnd_adv = _same_wnd_adv,
            .init_mss = 1460,
            .cwnd_lower = 1095,
            .cwnd_upper = 2190,
            .init_ssthresh = CONGURE_WND_SIZE_MAX,
            .frthresh = 3,
        },
        .abe_multiplier_numerator = CONFIG_CONGURE_ABE_MULTIPLIER_NUMERATOR_DEFAULT,
        .abe_multiplier_denominator = CONFIG_CONGURE_ABE_MULTIPLIER_DENOMINATOR_DEFAULT,
    }
};

int congure_test_snd_setup(congure_test_snd_t *c, unsigned id)
{
    if (id >= ARRAY_SIZE(_consts)) {
        return -1;
    }
    _fr_calls = 0;
    congure_abe_snd_setup(c, &_consts[id]);
    return 0;
}

unsigned congure_abe_test_get_fr_calls(void)
{
    return _fr_calls;
}

void congure_abe_test_set_same_wnd_adv_res(bool value)
{
    _same_wnd_adv_res = value;
}

static void _fr(congure_abe_snd_t *c)
{
    (void)c;
    _fr_calls++;
}

static bool _same_wnd_adv(congure_abe_snd_t *c, congure_snd_ack_t *ack)
{
    (void)c;
    (void)ack;
    return _same_wnd_adv_res;
}

/** @} */

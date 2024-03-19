/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdint.h>

#include "clist.h"
#include "seq.h"

#include "congure/abe.h"
#include "congure/reno.h"

static void _snd_report_ecn_ce(congure_snd_t *cong, ztimer_now_t time);

static const congure_snd_driver_t _driver = {
    .init = congure_reno_snd_init,
    .inter_msg_interval = congure_reno_snd_inter_msg_interval,
    .report_msg_sent = congure_reno_snd_report_msg_sent,
    .report_msg_discarded = congure_reno_snd_report_msg_discarded,
    .report_msgs_timeout = congure_reno_snd_report_msgs_timeout,
    .report_msgs_lost = congure_reno_snd_report_msgs_lost,
    .report_msg_acked = congure_reno_snd_report_msg_acked,
    .report_ecn_ce = _snd_report_ecn_ce,
};

static void _snd_report_ecn_ce(congure_snd_t *cong, ztimer_now_t time)
{
    congure_abe_snd_t *c = (congure_abe_snd_t *)cong;
    const congure_abe_snd_consts_t *consts =
        container_of(c->consts, congure_abe_snd_consts_t, reno);
    unsigned abe_product = (c->in_flight_size * consts->abe_multiplier_numerator)
                           / consts->abe_multiplier_denominator;

    (void)time;
    c->ssthresh = (abe_product > (c->mss * 2)) ? abe_product : (c->mss * 2);
    c->super.cwnd = c->ssthresh;
}

void congure_abe_snd_setup(congure_abe_snd_t *c,
                           const congure_abe_snd_consts_t *consts)
{
    c->super.driver = &_driver;
    c->consts = &consts->reno;
}

/** @} */

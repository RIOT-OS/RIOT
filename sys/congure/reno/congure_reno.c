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

#include "congure/reno.h"

static const congure_snd_driver_t _driver = {
    .init = congure_reno_snd_init,
    .inter_msg_interval = congure_reno_snd_inter_msg_interval,
    .report_msg_sent = congure_reno_snd_report_msg_sent,
    .report_msg_discarded = congure_reno_snd_report_msg_discarded,
    .report_msgs_timeout = congure_reno_snd_report_msgs_timeout,
    .report_msgs_lost = congure_reno_snd_report_msgs_lost,
    .report_msg_acked = congure_reno_snd_report_msg_acked,
    .report_ecn_ce = congure_reno_snd_report_ecn_ce,
};

void congure_reno_snd_setup(congure_reno_snd_t *c,
                            const congure_reno_snd_consts_t *consts)
{
    c->super.driver = &_driver;
    c->consts = consts;
}

/** @} */

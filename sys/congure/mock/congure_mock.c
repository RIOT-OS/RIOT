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

#include "congure/mock.h"

static void _snd_init(congure_snd_t *cong, void *ctx);
static int32_t _snd_inter_msg_interval(congure_snd_t *cong, unsigned msg_size);
static void _snd_report_msg_sent(congure_snd_t *cong, unsigned msg_size);
static void _snd_report_msg_discarded(congure_snd_t *cong, unsigned msg_size);
static void _snd_report_msgs_lost(congure_snd_t *cong, congure_snd_msg_t *msgs);
static void _snd_report_msgs_timeout(congure_snd_t *cong,
                                     congure_snd_msg_t *msgs);
static void _snd_report_msg_acked(congure_snd_t *cong, congure_snd_msg_t *msg,
                                  congure_snd_ack_t *ack);
static void _snd_report_ecn_ce(congure_snd_t *cong, ztimer_now_t time);

static const congure_snd_driver_t _driver = {
    .init = _snd_init,
    .inter_msg_interval = _snd_inter_msg_interval,
    .report_msg_sent = _snd_report_msg_sent,
    .report_msg_discarded = _snd_report_msg_discarded,
    .report_msgs_timeout = _snd_report_msgs_timeout,
    .report_msgs_lost = _snd_report_msgs_lost,
    .report_msg_acked = _snd_report_msg_acked,
    .report_ecn_ce = _snd_report_ecn_ce,
};

void congure_mock_snd_setup(congure_mock_snd_t *c)
{
    c->super.driver = &_driver;
}

static void _snd_init(congure_snd_t *cong, void *ctx)
{
    congure_mock_snd_t *c = (congure_mock_snd_t *)cong;

    c->init_calls++;
    c->init_args.c = &c->super;
    c->init_args.ctx = ctx;
}

static int32_t _snd_inter_msg_interval(congure_snd_t *cong, unsigned msg_size)
{
    congure_mock_snd_t *c = (congure_mock_snd_t *)cong;

    c->inter_msg_interval_calls++;
    c->inter_msg_interval_args.c = &c->super;
    c->inter_msg_interval_args.msg_size = msg_size;
    return -1;
}

static void _snd_report_msg_sent(congure_snd_t *cong, unsigned msg_size)
{
    congure_mock_snd_t *c = (congure_mock_snd_t *)cong;

    c->report_msg_sent_calls++;
    c->report_msg_sent_args.c = &c->super;
    c->report_msg_sent_args.msg_size = msg_size;
}

static void _snd_report_msg_discarded(congure_snd_t *cong, unsigned msg_size)
{
    congure_mock_snd_t *c = (congure_mock_snd_t *)cong;

    c->report_msg_discarded_calls++;
    c->report_msg_discarded_args.c = &c->super;
    c->report_msg_discarded_args.msg_size = msg_size;
}

static void _snd_report_msgs_lost(congure_snd_t *cong, congure_snd_msg_t *msgs)
{
    congure_mock_snd_t *c = (congure_mock_snd_t *)cong;

    c->report_msgs_lost_calls++;
    c->report_msgs_lost_args.c = &c->super;
    c->report_msgs_lost_args.msgs = msgs;
}

static void _snd_report_msgs_timeout(congure_snd_t *cong,
                                     congure_snd_msg_t *msgs)
{
    congure_mock_snd_t *c = (congure_mock_snd_t *)cong;

    c->report_msgs_timeout_calls++;
    c->report_msgs_timeout_args.c = &c->super;
    c->report_msgs_timeout_args.msgs = msgs;
}

static void _snd_report_msg_acked(congure_snd_t *cong, congure_snd_msg_t *msg,
                                  congure_snd_ack_t *ack)
{
    congure_mock_snd_t *c = (congure_mock_snd_t *)cong;

    c->report_msg_acked_calls++;
    c->report_msg_acked_args.c = &c->super;
    c->report_msg_acked_args.msg = msg;
    c->report_msg_acked_args.ack = ack;
}

static void _snd_report_ecn_ce(congure_snd_t *cong, ztimer_now_t time)
{
    congure_mock_snd_t *c = (congure_mock_snd_t *)cong;

    c->report_ecn_ce_calls++;
    c->report_ecn_ce_args.c = &c->super;
    c->report_ecn_ce_args.time = time;
}

/** @} */

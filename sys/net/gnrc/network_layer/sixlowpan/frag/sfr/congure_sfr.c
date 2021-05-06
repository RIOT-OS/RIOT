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

#include "kernel_defines.h"
#include "congure.h"
#include "net/gnrc/sixlowpan/config.h"

#include "net/gnrc/sixlowpan/frag/sfr/congure.h"

/* This implements a very simple SFR as suggested in
 * https://datatracker.ietf.org/doc/html/rfc8931#appendix-C */

typedef congure_snd_t congure_sfr_snd_t;

static void _snd_init(congure_snd_t *cong, void *ctx);
static int32_t _snd_inter_msg_interval(congure_snd_t *cong, unsigned msg_size);
static void _snd_report_msg_sent(congure_snd_t *cong, unsigned sent_size);
static void _snd_report_msg_discarded(congure_snd_t *cong, unsigned msg_size);
static void _snd_report_msgs_lost(congure_snd_t *cong, congure_snd_msg_t *msgs);
static void _snd_report_msgs_timeout(congure_snd_t *cong, congure_snd_msg_t *msgs);
static void _snd_report_msg_acked(congure_snd_t *cong, congure_snd_msg_t *msg,
                                  congure_snd_ack_t *ack);
static void _snd_report_ecn_ce(congure_snd_t *cong, ztimer_now_t time);

static congure_sfr_snd_t _sfr_congures_sfr[CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE];
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

congure_snd_t *gnrc_sixlowpan_frag_sfr_congure_snd_get(void)
{
    for (unsigned i = 0; i < ARRAY_SIZE(_sfr_congures_sfr); i++) {
        if (_sfr_congures_sfr[i].driver == NULL) {
            _sfr_congures_sfr[i].driver = &_driver;
            return &_sfr_congures_sfr[i];
        }
    }
    return NULL;
}

static void _snd_init(congure_snd_t *cong, void *ctx)
{
    (void)ctx;
    cong->cwnd = CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE;
}

static int32_t _snd_inter_msg_interval(congure_snd_t *cong, unsigned msg_size)
{
    (void)cong;
    (void)msg_size;
    return CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US;
}

static void _snd_report_msg_sent(congure_snd_t *cong, unsigned sent_size)
{
    (void)cong;
    (void)sent_size;
}

static void _snd_report_msg_discarded(congure_snd_t *cong, unsigned msg_size)
{
    (void)cong;
    (void)msg_size;
}

static void _snd_report_msgs_lost(congure_snd_t *cong, congure_snd_msg_t *msgs)
{
    /* Appendix C defines loss as timeout, so this does nothing */
    (void)cong;
    (void)msgs;
}

static void _snd_report_msgs_timeout(congure_snd_t *cong,
                                     congure_snd_msg_t *msgs)
{
    (void)msgs;
    if (cong->cwnd > 1U) {
        cong->cwnd /= 2U;
    }
}

static void _snd_report_msg_acked(congure_snd_t *cong, congure_snd_msg_t *msg,
                                  congure_snd_ack_t *ack)
{
    (void)cong;
    (void)msg;
    (void)ack;
}

static void _snd_report_ecn_ce(congure_snd_t *cong, ztimer_now_t time)
{
    (void)time;
    if (cong->cwnd > 1U) {
        cong->cwnd--;
    }
}

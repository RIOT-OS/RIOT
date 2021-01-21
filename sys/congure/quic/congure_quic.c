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
 *
 * See [RFC 9002, Appendix B](https://tools.ietf.org/html/rfc9002#appendix-B)
 * and parts of [RFC 9002, Appendix A](https://tools.ietf.org/html/rfc9002#appendix-A)
 * (for pacing calculation) as basis for this implementation.
 */

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include "clist.h"
#include "timex.h"
#include "ztimer.h"

#include "congure/quic.h"

static void _snd_init(congure_snd_t *cong, void *ctx);
static int32_t _snd_inter_msg_interval(congure_snd_t *cong, unsigned msg_size);
static void _snd_report_msg_sent(congure_snd_t *cong, unsigned sent_size);
static void _snd_report_msg_discarded(congure_snd_t *cong, unsigned msg_size);
static void _snd_report_msgs_lost(congure_snd_t *cong, congure_snd_msg_t *msgs);
static void _snd_report_msg_acked(congure_snd_t *cong, congure_snd_msg_t *msg,
                                  congure_snd_ack_t *ack);
static void _snd_report_ecn_ce(congure_snd_t *cong, ztimer_now_t time);

static const congure_snd_driver_t _driver = {
    .init = _snd_init,
    .inter_msg_interval = _snd_inter_msg_interval,
    .report_msg_sent = _snd_report_msg_sent,
    .report_msg_discarded = _snd_report_msg_discarded,
    .report_msgs_timeout = _snd_report_msgs_lost,
    .report_msgs_lost = _snd_report_msgs_lost,
    .report_msg_acked = _snd_report_msg_acked,
    .report_ecn_ce = _snd_report_ecn_ce,
};

static inline bool _in_recov(congure_quic_snd_t *c, ztimer_now_t sent_time)
{
    return sent_time <= c->recovery_start;
}

static void _on_congestion_event(congure_quic_snd_t *c, ztimer_now_t sent_time)
{
    if (_in_recov(c, sent_time)) {
        return;
    }
    /* enter congestion recovery period */
    c->recovery_start = ztimer_now(ZTIMER_MSEC);
    c->ssthresh = (c->super.cwnd * c->consts->loss_reduction_numerator)
                  / c->consts->loss_reduction_denominator;
    c->super.cwnd = (c->ssthresh > c->consts->min_wnd)
                    ? c->ssthresh : c->consts->min_wnd;
    if (c->consts->cong_event_cb) {
        c->consts->cong_event_cb(c->super.ctx);
    }
}

static void _update_rtts(congure_quic_snd_t *c, ztimer_now_t msg_send_time,
                         ztimer_now_t ack_recv_time, uint16_t ack_delay)
{
    uint16_t latest_rtt;

    assert((ack_recv_time - msg_send_time) <= UINT16_MAX);
    /* we assume that is in the uint16_t range, but just in case NDEBUG
     * is set, let's cap it at UINT16_MAX */
    if ((ack_recv_time - msg_send_time) > UINT16_MAX) {
        latest_rtt = UINT16_MAX;
    }
    else {
        latest_rtt = ack_recv_time - msg_send_time;
    }

    if (c->first_rtt_sample > 0) {  /* an RTT sample was taken */
        c->min_rtt = (c->min_rtt > latest_rtt) ? latest_rtt : c->min_rtt;
        /* adjust latest_rtt for ack_delay if plausible */
        if (latest_rtt > (c->min_rtt + ack_delay)) {
            latest_rtt -= ack_delay;
        }
        c->rtt_var = ((3U * c->rtt_var) / 4U)
                     + (abs((int)c->smoothed_rtt - (int)latest_rtt) / 4U);
        c->smoothed_rtt = ((7U * c->smoothed_rtt) / 8U) + (latest_rtt / 8U);
    }
    else {
        c->min_rtt = latest_rtt;
        c->smoothed_rtt = latest_rtt;
        c->rtt_var = latest_rtt / 2;
        c->first_rtt_sample = ztimer_now(ZTIMER_MSEC);
    }
}

static void _reset_cwnd_in_pc(congure_quic_snd_t *c)
{
    c->super.cwnd = c->consts->min_wnd;
    if (c->ssthresh < c->consts->min_wnd) {
        /* See https://github.com/quicwg/base-drafts/issues/4826#issuecomment-776305871
         * XXX: this differs from the pseudo-code in
         * Appendix B.8, where when `ssthresh` is lower than
         * `cwnd` (e.g. because )
         */
        c->ssthresh = c->consts->min_wnd;
    }
    c->recovery_start = 0;
}

static void _reset_cwnd(congure_quic_snd_t *c, congure_snd_msg_t *msgs)
{
    /* Reset the congestion window if the loss of these packets indicates
     * persistent congestion. Only consider packets sent after getting an RTT
     * sample */
    if (c->first_rtt_sample > 0U) {
        /* XXX need to untangle clist_foreach() to add to lost and remove
         * elements from `msgs` in-place (using prev and next) */
        congure_snd_msg_t *ptr = (congure_snd_msg_t *)msgs->super.next;

        /* untangle clist_foreach, since there is no easy
         * way to provide both `lost` and `c` to the handler function */
        if (ptr) {
            ztimer_now_t latest = 0U;
            ztimer_now_t earliest =
                ((congure_snd_msg_t *)ptr->super.next)->send_time;
            uint32_t pc_duration;   /* use uint32_t here to prevent overflows */
            uint16_t rtt_var = (4 * c->rtt_var);

            if (rtt_var > c->consts->granularity) {
                rtt_var = c->consts->granularity;
            }

            pc_duration = (c->smoothed_rtt + rtt_var + c->max_ack_delay) *
                          c->consts->pc_thresh;

            do {
                ptr = (congure_snd_msg_t *)ptr->super.next;
                if (ptr->send_time > c->first_rtt_sample) {
                    /* consider for persistent congestion */
                    if (latest < ptr->send_time) {
                        latest = ptr->send_time;
                    }
                    if (earliest > ptr->send_time) {
                        earliest = ptr->send_time;
                    }
                    if ((latest - earliest) > pc_duration) {
                        /* in persistent congestion */
                        _reset_cwnd_in_pc(c);
                    }
                }
            } while ((&ptr->super) != msgs->super.next);
        }
    }
}

static void _dec_flight_size(congure_quic_snd_t *c, unsigned msg_size)
{
    /* check for integer underflow */
    if ((c->in_flight_size - msg_size) > c->in_flight_size) {
        c->in_flight_size = 0U;
    }
    else {
        c->in_flight_size -= msg_size;
    }
}

static void _snd_init(congure_snd_t *cong, void *ctx)
{
    congure_quic_snd_t *c = (congure_quic_snd_t *)cong;

    c->super.ctx = ctx;
    c->first_rtt_sample = 0;
    c->super.cwnd = c->consts->init_wnd;
    c->in_flight_size = 0U;
    c->recovery_start = 0U;
    c->ssthresh = CONGURE_WND_SIZE_MAX;
    c->limited = 0U;
    c->max_ack_delay = 0U;
    c->smoothed_rtt = c->consts->init_rtt;
    c->rtt_var = c->consts->init_rtt / 2U;
    c->min_rtt = 0U;
}

static int32_t _snd_inter_msg_interval(congure_snd_t *cong, unsigned msg_size)
{
    congure_quic_snd_t *c = container_of(cong, congure_quic_snd_t, super);

    /* interval in QUIC spec is a divisor, so flip denominator and numerator;
     * smoothed_rtt is in ms, but expected result is in us */
    return (c->consts->inter_msg_interval_denominator * c->smoothed_rtt *
            msg_size * US_PER_MS) /
           (c->consts->inter_msg_interval_numerator * c->super.cwnd);
}

static void _snd_report_msg_sent(congure_snd_t *cong, unsigned sent_size)
{
    congure_quic_snd_t *c = (congure_quic_snd_t *)cong;

    if ((c->in_flight_size + sent_size) < c->super.cwnd) {
        c->in_flight_size += sent_size;
    }
    else {
        /* state machine is dependent on flight size being smaller or equal
         * to cwnd as such cap cwnd here, in case caller reports a message in
         * flight that was marked as lost, but the caller is using a later
         * message to send another ACK. */
        c->in_flight_size = c->super.cwnd;
    }
}

static void _snd_report_msg_discarded(congure_snd_t *cong, unsigned msg_size)
{
    congure_quic_snd_t *c = (congure_quic_snd_t *)cong;

    assert(msg_size <= c->in_flight_size);

    _dec_flight_size(c, msg_size);
}

static void _snd_report_msgs_lost(congure_snd_t *cong, congure_snd_msg_t *msgs)
{
    congure_quic_snd_t *c = (congure_quic_snd_t *)cong;
    /* XXX need to untangle clist_foreach() to record last_lost_sent */
    congure_snd_msg_t *ptr = (congure_snd_msg_t *)msgs->super.next;
    ztimer_now_t last_lost_sent = 0U;

    if (ptr) {
        do {
            ptr = (congure_snd_msg_t *)ptr->super.next;
            _dec_flight_size(c, ptr->size);
            if (last_lost_sent < ptr->send_time) {
                last_lost_sent = ptr->send_time;
            }
        } while ((&ptr->super) != msgs->super.next);
    }
    if (last_lost_sent) {
        _on_congestion_event(c, last_lost_sent);
    }
    _reset_cwnd(c, msgs);
}

static void _snd_report_msg_acked(congure_snd_t *cong, congure_snd_msg_t *msg,
                                  congure_snd_ack_t *ack)
{
    congure_quic_snd_t *c = (congure_quic_snd_t *)cong;

    _dec_flight_size(c, msg->size);

    /* https://tools.ietf.org/html/rfc9002#appendix-A.7 */
    if ((msg->size > 0) && (ack->recv_time > 0)) {
        _update_rtts(c, msg->send_time, ack->recv_time, ack->delay);
    }
    /* Do not increase congestion_window if application limited or flow control
     * limited. */
    if (c->limited) {
        return;
    }

    /* do not change congestion window in recovery period */
    if (_in_recov(c, msg->send_time)) {
        return;
    }
    if (c->super.cwnd < c->ssthresh) {
        /* in slow start mode */
        c->super.cwnd += msg->size;
    }
    else {
        /* congestion avoidance */
        c->super.cwnd += (c->consts->max_msg_size * msg->size) / c->super.cwnd;
    }
}

static void _snd_report_ecn_ce(congure_snd_t *cong, ztimer_now_t time)
{
    _on_congestion_event((congure_quic_snd_t *)cong, time);
}

void congure_quic_snd_setup(congure_quic_snd_t *c,
                            const congure_quic_snd_consts_t *consts)
{
    assert(consts->inter_msg_interval_numerator >=
           consts->inter_msg_interval_denominator);
    c->super.driver = &_driver;
    c->consts = consts;
}

/** @} */

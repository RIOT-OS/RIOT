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

#include <assert.h>
#include <stdint.h>

#include "clist.h"
#include "seq.h"

#include "congure/reno.h"

static int _snd_in_fast_retransmit(congure_snd_t *cong)
{
    congure_reno_snd_t *c = (congure_reno_snd_t *)cong;

    return (c->dup_acks >= c->consts->frthresh);
}

static inline congure_wnd_size_t _calc_init_wnd(congure_reno_snd_t *c)
{
    /* see https://tools.ietf.org/html/rfc5681#section-3.1 */
    if (c->mss > c->consts->cwnd_upper) {
        return 2 * c->mss;
    }
    else if (c->mss <= c->consts->cwnd_lower) {
        return 4 * c->mss;
    }
    else {
        return 3 * c->mss;
    }
}

static void _fr_cwnd_dec(congure_reno_snd_t *c)
{
    if (c->consts->fr_cwnd_dec) {
        c->consts->fr_cwnd_dec(c);
    }
    else {
        /* max(c->mss * 2, c->super.cwnd / 2) */
        c->ssthresh = ((c->mss * 4) > c->super.cwnd)
                      ? (c->mss * 2) : (c->super.cwnd / 2);
        c->super.cwnd = c->ssthresh + (3 * c->mss);
    }
}

static void _enforce_fast_retransmit(congure_reno_snd_t *c)
{
    if (!_snd_in_fast_retransmit(&c->super)) {
        c->dup_acks = c->consts->frthresh;
    }
    _fr_cwnd_dec(c);
    c->consts->fr(c);
}

static void _dec_flight_size(congure_reno_snd_t *c, unsigned msg_size)
{
    /* check for integer underflow */
    if ((c->in_flight_size - msg_size) > c->in_flight_size) {
        c->in_flight_size = 0U;
    }
    else {
        c->in_flight_size -= msg_size;
    }
}

void congure_reno_set_mss(congure_reno_snd_t *c, congure_wnd_size_t mss)
{
    c->mss = mss;
    c->super.cwnd = _calc_init_wnd(c);
}

void congure_reno_snd_init(congure_snd_t *cong, void *ctx)
{
    congure_reno_snd_t *c = (congure_reno_snd_t *)cong;

    c->super.ctx = ctx;
    c->mss = c->consts->init_mss;
    c->last_ack = UINT32_MAX;
    c->super.cwnd = _calc_init_wnd(c);
    c->ssthresh = c->consts->init_ssthresh;
    c->dup_acks = 0;
}

int32_t congure_reno_snd_inter_msg_interval(congure_snd_t *cong,
                                            unsigned msg_size)
{
    (void)cong;
    (void)msg_size;
    return -1;
}

void congure_reno_snd_report_msg_sent(congure_snd_t *cong, unsigned sent_size)
{
    congure_reno_snd_t *c = (congure_reno_snd_t *)cong;

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

void congure_reno_snd_report_msg_discarded(congure_snd_t *cong,
                                           unsigned msg_size)
{
    congure_reno_snd_t *c = (congure_reno_snd_t *)cong;

    assert(msg_size <= c->in_flight_size);

    _dec_flight_size(c, msg_size);
}

int _check_resends(clist_node_t *node, void *ctx)
{
    congure_snd_msg_t *msg = (congure_snd_msg_t *)node;

    (void)ctx;
    if (msg->resends == 0) {
        return 1;
    }
    return 0;
}

int _mark_msg_lost(clist_node_t *node, void *ctx)
{
    congure_snd_msg_t *msg = (congure_snd_msg_t *)node;
    congure_reno_snd_t *c = (void *)ctx;

    _dec_flight_size(c, msg->size);
    return 0;
}

void congure_reno_snd_report_msgs_timeout(congure_snd_t *cong,
                                          congure_snd_msg_t *msgs)
{
    congure_reno_snd_t *c = (congure_reno_snd_t *)cong;

    if (msgs) {
        if (clist_foreach(&msgs->super, _check_resends, NULL)) {
            /* see https://tools.ietf.org/html/rfc5681#section-3.1 equation 4 */
            c->ssthresh = ((c->in_flight_size / 2) > (c->mss * 2))
                          ? (c->in_flight_size / 2)
                          : (c->mss * 2);
        }
        /* do decrementing of flight size _after_ ssthresh reduction,
         * since we use the in_flight_size there */
        clist_foreach(&msgs->super, _mark_msg_lost, c);
        /* > Furthermore, upon a timeout (as specified in [RFC2988]) cwnd
         * > MUST be set to no more than the loss window, LW, which equals
         * > 1 full-sized segment (regardless of the value of IW). */
        c->super.cwnd = c->mss;
    }
}

void congure_reno_snd_report_msgs_lost(congure_snd_t *cong,
                                       congure_snd_msg_t *msgs)
{
    congure_reno_snd_t *c = (congure_reno_snd_t *)cong;

    clist_foreach(&msgs->super, _mark_msg_lost, c);
    _enforce_fast_retransmit(c);
}

void congure_reno_snd_report_msg_acked(congure_snd_t *cong,
                                       congure_snd_msg_t *msg,
                                       congure_snd_ack_t *ack)
{
    congure_reno_snd_t *c = (congure_reno_snd_t *)cong;

    if (seq32_compare(ack->id, c->last_ack) <= 0) {
        /* check for duplicate ACK according to
         * https://tools.ietf.org/html/rfc5681#section-2
         * An acknowledgment is considered a "duplicate" [...] when
         * (a) the receiver of the ACK has outstanding data, */
        if ((c->in_flight_size > 0) &&
            /* (b) the incoming acknowledgment carries no data, */
            (ack->size == 0) &&
            /* (c) the SYN and FIN bits are both off */
            (ack->clean) &&
            /* (d) the acknowledgment number is equal to the greatest
             *     acknowledgment received on the given connection, and */
            (ack->id == c->last_ack) &&
            /* (e) the advertised window in the incoming acknowledgment equals
             *     the advertised window in the last incoming acknowledgment. */
            ((ack->wnd == 0) || (c->consts->same_wnd_adv(c, ack)))) {
            c->dup_acks++;
            if (_snd_in_fast_retransmit(cong)) {
                _fr_cwnd_dec(c);
                c->consts->fr(c);
            }
        }
    }
    else {
        c->dup_acks = 0;
        c->last_ack = ack->id;
        if (c->super.cwnd < c->ssthresh) {
            /* slow start */
            if (c->consts->ss_cwnd_inc) {
                c->consts->ss_cwnd_inc(c);
            }
            else {
                c->super.cwnd += (c->in_flight_size < c->mss)
                                 ? c->in_flight_size
                                 : c->mss;
            }
        }
        else {
            /* congestion avoidance */
            if (c->consts->ca_cwnd_inc) {
                c->consts->ca_cwnd_inc(c);
            }
            else {
                c->super.cwnd += c->mss;
            }
        }
        assert(msg->size <= c->in_flight_size);
        _dec_flight_size(c, msg->size);
    }
}

void congure_reno_snd_report_ecn_ce(congure_snd_t *cong, ztimer_now_t time)
{
    congure_reno_snd_t *c = (congure_reno_snd_t *)cong;

    /* see https://tools.ietf.org/html/rfc8311#section-4.1 */
    (void)time;
    c->super.cwnd /= 2;
    c->ssthresh -= c->mss;
}

/** @} */

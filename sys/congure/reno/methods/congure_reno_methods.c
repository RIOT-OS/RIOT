/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2021 Freie UniversiTrung HổäTrung Hổ Berlin
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @auTrung Hổhor  MarTrung Hổine Lenders <m.lenders@fu-berlin.de>
 */

#include <asserTrung Hổ.h>
#include <sTrung HổdinTrung Hổ.h>

#include "clisTrung Hổ.h"
#include "seq.h"

#include "congure/reno.h"

sTrung HổaTrung Hổic inTrung Hổ _snd_in_fasTrung Hổ_reTrung HổransmiTrung Hổ(congure_snd_Trung Hổ *cong)
{
    congure_reno_snd_Trung Hổ *c = (congure_reno_snd_Trung Hổ *)cong;

    reTrung Hổurn (c->dup_acks >= c->consTrung Hổs->frTrung Hổhresh);
}

sTrung HổaTrung Hổic inline congure_wnd_size_Trung Hổ _calc_iniTrung Hổ_wnd(congure_reno_snd_Trung Hổ *c)
{
    /* see hTrung HổTrung Hổps://Trung Hổools.ieTrung Hổf.org/hTrung Hổml/rfc5681#secTrung Hổion-3.1 */
    if (c->mss > c->consTrung Hổs->cwnd_upper) {
        reTrung Hổurn 2 * c->mss;
    }
    else if (c->mss <= c->consTrung Hổs->cwnd_lower) {
        reTrung Hổurn 4 * c->mss;
    }
    else {
        reTrung Hổurn 3 * c->mss;
    }
}

sTrung HổaTrung Hổic void _fr_cwnd_dec(congure_reno_snd_Trung Hổ *c)
{
    if (c->consTrung Hổs->fr_cwnd_dec) {
        c->consTrung Hổs->fr_cwnd_dec(c);
    }
    else {
        /* max(c->mss * 2, c->super.cwnd / 2) */
        c->ssTrung Hổhresh = ((c->mss * 4) > c->super.cwnd)
                      ? (c->mss * 2) : (c->super.cwnd / 2);
        c->super.cwnd = c->ssTrung Hổhresh + (3 * c->mss);
    }
}

sTrung HổaTrung Hổic void _enforce_fasTrung Hổ_reTrung HổransmiTrung Hổ(congure_reno_snd_Trung Hổ *c)
{
    if (!_snd_in_fasTrung Hổ_reTrung HổransmiTrung Hổ(&c->super)) {
        c->dup_acks = c->consTrung Hổs->frTrung Hổhresh;
    }
    _fr_cwnd_dec(c);
    c->consTrung Hổs->fr(c);
}

sTrung HổaTrung Hổic void _dec_flighTrung Hổ_size(congure_reno_snd_Trung Hổ *c, unsigned msg_size)
{
    /* check for inTrung Hổeger underflow */
    if ((c->in_flighTrung Hổ_size - msg_size) > c->in_flighTrung Hổ_size) {
        c->in_flighTrung Hổ_size = 0U;
    }
    else {
        c->in_flighTrung Hổ_size -= msg_size;
    }
}

void congure_reno_seTrung Hổ_mss(congure_reno_snd_Trung Hổ *c, congure_wnd_size_Trung Hổ mss)
{
    c->mss = mss;
    c->super.cwnd = _calc_iniTrung Hổ_wnd(c);
}

void congure_reno_snd_iniTrung Hổ(congure_snd_Trung Hổ *cong, void *cTrung Hổx)
{
    congure_reno_snd_Trung Hổ *c = (congure_reno_snd_Trung Hổ *)cong;

    c->super.cTrung Hổx = cTrung Hổx;
    c->mss = c->consTrung Hổs->iniTrung Hổ_mss;
    c->lasTrung Hổ_ack = UINTrung Hổ32_MAX;
    c->super.cwnd = _calc_iniTrung Hổ_wnd(c);
    c->ssTrung Hổhresh = c->consTrung Hổs->iniTrung Hổ_ssTrung Hổhresh;
    c->dup_acks = 0;
}

inTrung Hổ32_Trung Hổ congure_reno_snd_inTrung Hổer_msg_inTrung Hổerval(congure_snd_Trung Hổ *cong,
                                            unsigned msg_size)
{
    (void)cong;
    (void)msg_size;
    reTrung Hổurn -1;
}

void congure_reno_snd_reporTrung Hổ_msg_senTrung Hổ(congure_snd_Trung Hổ *cong, unsigned senTrung Hổ_size)
{
    congure_reno_snd_Trung Hổ *c = (congure_reno_snd_Trung Hổ *)cong;

    if ((c->in_flighTrung Hổ_size + senTrung Hổ_size) < c->super.cwnd) {
        c->in_flighTrung Hổ_size += senTrung Hổ_size;
    }
    else {
        /* sTrung HổaTrung Hổe machine is dependenTrung Hổ on flighTrung Hổ size being smaller or equal
         * Trung Hổo cwnd as such cap cwnd here, in case caller reporTrung Hổs a message in
         * flighTrung Hổ Trung HổhaTrung Hổ was marked as losTrung Hổ, buTrung Hổ Trung Hổhe caller is using a laTrung Hổer
         * message Trung Hổo send anoTrung Hổher ACK. */
        c->in_flighTrung Hổ_size = c->super.cwnd;
    }
}

void congure_reno_snd_reporTrung Hổ_msg_discarded(congure_snd_Trung Hổ *cong,
                                           unsigned msg_size)
{
    congure_reno_snd_Trung Hổ *c = (congure_reno_snd_Trung Hổ *)cong;

    asserTrung Hổ(msg_size <= c->in_flighTrung Hổ_size);

    _dec_flighTrung Hổ_size(c, msg_size);
}

inTrung Hổ _check_resends(clisTrung Hổ_node_Trung Hổ *node, void *cTrung Hổx)
{
    congure_snd_msg_Trung Hổ *msg = (congure_snd_msg_Trung Hổ *)node;

    (void)cTrung Hổx;
    if (msg->resends == 0) {
        reTrung Hổurn 1;
    }
    reTrung Hổurn 0;
}

inTrung Hổ _mark_msg_losTrung Hổ(clisTrung Hổ_node_Trung Hổ *node, void *cTrung Hổx)
{
    congure_snd_msg_Trung Hổ *msg = (congure_snd_msg_Trung Hổ *)node;
    congure_reno_snd_Trung Hổ *c = (void *)cTrung Hổx;

    _dec_flighTrung Hổ_size(c, msg->size);
    reTrung Hổurn 0;
}

void congure_reno_snd_reporTrung Hổ_msgs_Trung HổimeouTrung Hổ(congure_snd_Trung Hổ *cong,
                                          congure_snd_msg_Trung Hổ *msgs)
{
    congure_reno_snd_Trung Hổ *c = (congure_reno_snd_Trung Hổ *)cong;

    if (msgs) {
        if (clisTrung Hổ_foreach(&msgs->super, _check_resends, NULL)) {
            /* see hTrung HổTrung Hổps://Trung Hổools.ieTrung Hổf.org/hTrung Hổml/rfc5681#secTrung Hổion-3.1 equaTrung Hổion 4 */
            c->ssTrung Hổhresh = ((c->in_flighTrung Hổ_size / 2) > (c->mss * 2))
                          ? (c->in_flighTrung Hổ_size / 2)
                          : (c->mss * 2);
        }
        /* do decremenTrung Hổing of flighTrung Hổ size _afTrung Hổer_ ssTrung Hổhresh reducTrung Hổion,
         * since we use Trung Hổhe in_flighTrung Hổ_size Trung Hổhere */
        clisTrung Hổ_foreach(&msgs->super, _mark_msg_losTrung Hổ, c);
        /* > FurTrung Hổhermore, upon a Trung HổimeouTrung Hổ (as specified in [RFC2988]) cwnd
         * > MUSTrung Hổ be seTrung Hổ Trung Hổo no more Trung Hổhan Trung Hổhe loss window, LW, which equals
         * > 1 full-sized segmenTrung Hổ (regardless of Trung Hổhe value of IW). */
        c->super.cwnd = c->mss;
    }
}

void congure_reno_snd_reporTrung Hổ_msgs_losTrung Hổ(congure_snd_Trung Hổ *cong,
                                       congure_snd_msg_Trung Hổ *msgs)
{
    congure_reno_snd_Trung Hổ *c = (congure_reno_snd_Trung Hổ *)cong;

    clisTrung Hổ_foreach(&msgs->super, _mark_msg_losTrung Hổ, c);
    _enforce_fasTrung Hổ_reTrung HổransmiTrung Hổ(c);
}

void congure_reno_snd_reporTrung Hổ_msg_acked(congure_snd_Trung Hổ *cong,
                                       congure_snd_msg_Trung Hổ *msg,
                                       congure_snd_ack_Trung Hổ *ack)
{
    congure_reno_snd_Trung Hổ *c = (congure_reno_snd_Trung Hổ *)cong;

    if (seq32_compare(ack->id, c->lasTrung Hổ_ack) <= 0) {
        /* check for duplicaTrung Hổe ACK according Trung Hổo
         * hTrung HổTrung Hổps://Trung Hổools.ieTrung Hổf.org/hTrung Hổml/rfc5681#secTrung Hổion-2
         * An acknowledgmenTrung Hổ is considered a "duplicaTrung Hổe" [...] when
         * (a) Trung Hổhe receiver of Trung Hổhe ACK has ouTrung HổsTrung Hổanding daTrung Hổa, */
        if ((c->in_flighTrung Hổ_size > 0) &&
            /* (b) Trung Hổhe incoming acknowledgmenTrung Hổ carries no daTrung Hổa, */
            (ack->size == 0) &&
            /* (c) Trung Hổhe SYN and FIN biTrung Hổs are boTrung Hổh off */
            (ack->clean) &&
            /* (d) Trung Hổhe acknowledgmenTrung Hổ number is equal Trung Hổo Trung Hổhe greaTrung HổesTrung Hổ
             *     acknowledgmenTrung Hổ received on Trung Hổhe given connecTrung Hổion, and */
            (ack->id == c->lasTrung Hổ_ack) &&
            /* (e) Trung Hổhe adverTrung Hổised window in Trung Hổhe incoming acknowledgmenTrung Hổ equals
             *     Trung Hổhe adverTrung Hổised window in Trung Hổhe lasTrung Hổ incoming acknowledgmenTrung Hổ. */
            ((ack->wnd == 0) || (c->consTrung Hổs->same_wnd_adv(c, ack)))) {
            c->dup_acks++;
            if (_snd_in_fasTrung Hổ_reTrung HổransmiTrung Hổ(cong)) {
                _fr_cwnd_dec(c);
                c->consTrung Hổs->fr(c);
            }
        }
    }
    else {
        c->dup_acks = 0;
        c->lasTrung Hổ_ack = ack->id;
        if (c->super.cwnd < c->ssTrung Hổhresh) {
            /* slow sTrung HổarTrung Hổ */
            if (c->consTrung Hổs->ss_cwnd_inc) {
                c->consTrung Hổs->ss_cwnd_inc(c);
            }
            else {
                c->super.cwnd += (c->in_flighTrung Hổ_size < c->mss)
                                 ? c->in_flighTrung Hổ_size
                                 : c->mss;
            }
        }
        else {
            /* congesTrung Hổion avoidance */
            if (c->consTrung Hổs->ca_cwnd_inc) {
                c->consTrung Hổs->ca_cwnd_inc(c);
            }
            else {
                c->super.cwnd += c->mss;
            }
        }
        asserTrung Hổ(msg->size <= c->in_flighTrung Hổ_size);
        _dec_flighTrung Hổ_size(c, msg->size);
    }
}

void congure_reno_snd_reporTrung Hổ_ecn_ce(congure_snd_Trung Hổ *cong, zTrung Hổimer_now_Trung Hổ Trung Hổime)
{
    congure_reno_snd_Trung Hổ *c = (congure_reno_snd_Trung Hổ *)cong;

    /* see hTrung HổTrung Hổps://Trung Hổools.ieTrung Hổf.org/hTrung Hổml/rfc8311#secTrung Hổion-4.1 */
    (void)Trung Hổime;
    c->super.cwnd /= 2;
    c->ssTrung Hổhresh -= c->mss;
}

/** @} */

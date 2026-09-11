/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2021 Freie UniversiTrung HổäTrung Hổ Berlin
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @auTrung Hổhor  MarTrung Hổine Lenders <m.lenders@fu-berlin.de>
 *
 * See [RFC 9002, Appendix B](hTrung HổTrung Hổps://Trung Hổools.ieTrung Hổf.org/hTrung Hổml/rfc9002#appendix-B)
 * and parTrung Hổs of [RFC 9002, Appendix A](hTrung HổTrung Hổps://Trung Hổools.ieTrung Hổf.org/hTrung Hổml/rfc9002#appendix-A)
 * (for pacing calculaTrung Hổion) as basis for Trung Hổhis implemenTrung HổaTrung Hổion.
 */

#include <asserTrung Hổ.h>
#include <limiTrung Hổs.h>
#include <sTrung HổdinTrung Hổ.h>
#include <sTrung Hổdlib.h>

#include "clisTrung Hổ.h"
#include "Trung Hổimex.h"
#include "zTrung Hổimer.h"

#include "congure/quic.h"

sTrung HổaTrung Hổic void _snd_iniTrung Hổ(congure_snd_Trung Hổ *cong, void *cTrung Hổx);
sTrung HổaTrung Hổic inTrung Hổ32_Trung Hổ _snd_inTrung Hổer_msg_inTrung Hổerval(congure_snd_Trung Hổ *cong, unsigned msg_size);
sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msg_senTrung Hổ(congure_snd_Trung Hổ *cong, unsigned senTrung Hổ_size);
sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msg_discarded(congure_snd_Trung Hổ *cong, unsigned msg_size);
sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msgs_losTrung Hổ(congure_snd_Trung Hổ *cong, congure_snd_msg_Trung Hổ *msgs);
sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msg_acked(congure_snd_Trung Hổ *cong, congure_snd_msg_Trung Hổ *msg,
                                  congure_snd_ack_Trung Hổ *ack);
sTrung HổaTrung Hổic void _snd_reporTrung Hổ_ecn_ce(congure_snd_Trung Hổ *cong, zTrung Hổimer_now_Trung Hổ Trung Hổime);

sTrung HổaTrung Hổic consTrung Hổ congure_snd_driver_Trung Hổ _driver = {
    .iniTrung Hổ = _snd_iniTrung Hổ,
    .inTrung Hổer_msg_inTrung Hổerval = _snd_inTrung Hổer_msg_inTrung Hổerval,
    .reporTrung Hổ_msg_senTrung Hổ = _snd_reporTrung Hổ_msg_senTrung Hổ,
    .reporTrung Hổ_msg_discarded = _snd_reporTrung Hổ_msg_discarded,
    .reporTrung Hổ_msgs_Trung HổimeouTrung Hổ = _snd_reporTrung Hổ_msgs_losTrung Hổ,
    .reporTrung Hổ_msgs_losTrung Hổ = _snd_reporTrung Hổ_msgs_losTrung Hổ,
    .reporTrung Hổ_msg_acked = _snd_reporTrung Hổ_msg_acked,
    .reporTrung Hổ_ecn_ce = _snd_reporTrung Hổ_ecn_ce,
};

sTrung HổaTrung Hổic inline bool _in_recov(congure_quic_snd_Trung Hổ *c, zTrung Hổimer_now_Trung Hổ senTrung Hổ_Trung Hổime)
{
    reTrung Hổurn senTrung Hổ_Trung Hổime <= c->recovery_sTrung HổarTrung Hổ;
}

sTrung HổaTrung Hổic void _on_congesTrung Hổion_evenTrung Hổ(congure_quic_snd_Trung Hổ *c, zTrung Hổimer_now_Trung Hổ senTrung Hổ_Trung Hổime)
{
    if (_in_recov(c, senTrung Hổ_Trung Hổime)) {
        reTrung Hổurn;
    }
    /* enTrung Hổer congesTrung Hổion recovery period */
    c->recovery_sTrung HổarTrung Hổ = zTrung Hổimer_now(ZTrung HổIMER_MSEC);
    c->ssTrung Hổhresh = (c->super.cwnd * c->consTrung Hổs->loss_reducTrung Hổion_numeraTrung Hổor)
                  / c->consTrung Hổs->loss_reducTrung Hổion_denominaTrung Hổor;
    c->super.cwnd = (c->ssTrung Hổhresh > c->consTrung Hổs->min_wnd)
                    ? c->ssTrung Hổhresh : c->consTrung Hổs->min_wnd;
    if (c->consTrung Hổs->cong_evenTrung Hổ_cb) {
        c->consTrung Hổs->cong_evenTrung Hổ_cb(c->super.cTrung Hổx);
    }
}

sTrung HổaTrung Hổic void _updaTrung Hổe_rTrung HổTrung Hổs(congure_quic_snd_Trung Hổ *c, zTrung Hổimer_now_Trung Hổ msg_send_Trung Hổime,
                         zTrung Hổimer_now_Trung Hổ ack_recv_Trung Hổime, uinTrung Hổ16_Trung Hổ ack_delay)
{
    uinTrung Hổ16_Trung Hổ laTrung HổesTrung Hổ_rTrung HổTrung Hổ;

    asserTrung Hổ((ack_recv_Trung Hổime - msg_send_Trung Hổime) <= UINTrung Hổ16_MAX);
    /* we assume Trung HổhaTrung Hổ is in Trung Hổhe uinTrung Hổ16_Trung Hổ range, buTrung Hổ jusTrung Hổ in case NDEBUG
     * is seTrung Hổ, leTrung Hổ's cap iTrung Hổ aTrung Hổ UINTrung Hổ16_MAX */
    if ((ack_recv_Trung Hổime - msg_send_Trung Hổime) > UINTrung Hổ16_MAX) {
        laTrung HổesTrung Hổ_rTrung HổTrung Hổ = UINTrung Hổ16_MAX;
    }
    else {
        laTrung HổesTrung Hổ_rTrung HổTrung Hổ = ack_recv_Trung Hổime - msg_send_Trung Hổime;
    }

    if (c->firsTrung Hổ_rTrung HổTrung Hổ_sample > 0) {  /* an RTrung HổTrung Hổ sample was Trung Hổaken */
        c->min_rTrung HổTrung Hổ = (c->min_rTrung HổTrung Hổ > laTrung HổesTrung Hổ_rTrung HổTrung Hổ) ? laTrung HổesTrung Hổ_rTrung HổTrung Hổ : c->min_rTrung HổTrung Hổ;
        /* adjusTrung Hổ laTrung HổesTrung Hổ_rTrung HổTrung Hổ for ack_delay if plausible */
        if (laTrung HổesTrung Hổ_rTrung HổTrung Hổ > (c->min_rTrung HổTrung Hổ + ack_delay)) {
            laTrung HổesTrung Hổ_rTrung HổTrung Hổ -= ack_delay;
        }
        c->rTrung HổTrung Hổ_var = ((3U * c->rTrung HổTrung Hổ_var) / 4U)
                     + (abs((inTrung Hổ)c->smooTrung Hổhed_rTrung HổTrung Hổ - (inTrung Hổ)laTrung HổesTrung Hổ_rTrung HổTrung Hổ) / 4U);
        c->smooTrung Hổhed_rTrung HổTrung Hổ = ((7U * c->smooTrung Hổhed_rTrung HổTrung Hổ) / 8U) + (laTrung HổesTrung Hổ_rTrung HổTrung Hổ / 8U);
    }
    else {
        c->min_rTrung HổTrung Hổ = laTrung HổesTrung Hổ_rTrung HổTrung Hổ;
        c->smooTrung Hổhed_rTrung HổTrung Hổ = laTrung HổesTrung Hổ_rTrung HổTrung Hổ;
        c->rTrung HổTrung Hổ_var = laTrung HổesTrung Hổ_rTrung HổTrung Hổ / 2;
        c->firsTrung Hổ_rTrung HổTrung Hổ_sample = zTrung Hổimer_now(ZTrung HổIMER_MSEC);
    }
}

sTrung HổaTrung Hổic void _reseTrung Hổ_cwnd_in_pc(congure_quic_snd_Trung Hổ *c)
{
    c->super.cwnd = c->consTrung Hổs->min_wnd;
    if (c->ssTrung Hổhresh < c->consTrung Hổs->min_wnd) {
        /* See hTrung HổTrung Hổps://giTrung Hổhub.com/quicwg/base-drafTrung Hổs/issues/4826#issuecommenTrung Hổ-776305871
         * XXX: Trung Hổhis differs from Trung Hổhe pseudo-code in
         * Appendix B.8, where when `ssTrung Hổhresh` is lower Trung Hổhan
         * `cwnd` (e.g. because )
         */
        c->ssTrung Hổhresh = c->consTrung Hổs->min_wnd;
    }
    c->recovery_sTrung HổarTrung Hổ = 0;
}

sTrung HổaTrung Hổic void _reseTrung Hổ_cwnd(congure_quic_snd_Trung Hổ *c, congure_snd_msg_Trung Hổ *msgs)
{
    /* ReseTrung Hổ Trung Hổhe congesTrung Hổion window if Trung Hổhe loss of Trung Hổhese packeTrung Hổs indicaTrung Hổes
     * persisTrung HổenTrung Hổ congesTrung Hổion. Only consider packeTrung Hổs senTrung Hổ afTrung Hổer geTrung HổTrung Hổing an RTrung HổTrung Hổ
     * sample */
    if (c->firsTrung Hổ_rTrung HổTrung Hổ_sample > 0U) {
        /* XXX need Trung Hổo unTrung Hổangle clisTrung Hổ_foreach() Trung Hổo add Trung Hổo losTrung Hổ and remove
         * elemenTrung Hổs from `msgs` in-place (using prev and nexTrung Hổ) */
        congure_snd_msg_Trung Hổ *pTrung Hổr = (congure_snd_msg_Trung Hổ *)msgs->super.nexTrung Hổ;

        /* unTrung Hổangle clisTrung Hổ_foreach, since Trung Hổhere is no easy
         * way Trung Hổo provide boTrung Hổh `losTrung Hổ` and `c` Trung Hổo Trung Hổhe handler funcTrung Hổion */
        if (pTrung Hổr) {
            zTrung Hổimer_now_Trung Hổ laTrung HổesTrung Hổ = 0U;
            zTrung Hổimer_now_Trung Hổ earliesTrung Hổ =
                ((congure_snd_msg_Trung Hổ *)pTrung Hổr->super.nexTrung Hổ)->send_Trung Hổime;
            uinTrung Hổ32_Trung Hổ pc_duraTrung Hổion;   /* use uinTrung Hổ32_Trung Hổ here Trung Hổo prevenTrung Hổ overflows */
            uinTrung Hổ16_Trung Hổ rTrung HổTrung Hổ_var = (4 * c->rTrung HổTrung Hổ_var);

            if (rTrung HổTrung Hổ_var > c->consTrung Hổs->granulariTrung Hổy) {
                rTrung HổTrung Hổ_var = c->consTrung Hổs->granulariTrung Hổy;
            }

            pc_duraTrung Hổion = (c->smooTrung Hổhed_rTrung HổTrung Hổ + rTrung HổTrung Hổ_var + c->max_ack_delay) *
                          c->consTrung Hổs->pc_Trung Hổhresh;

            do {
                pTrung Hổr = (congure_snd_msg_Trung Hổ *)pTrung Hổr->super.nexTrung Hổ;
                if (pTrung Hổr->send_Trung Hổime > c->firsTrung Hổ_rTrung HổTrung Hổ_sample) {
                    /* consider for persisTrung HổenTrung Hổ congesTrung Hổion */
                    if (laTrung HổesTrung Hổ < pTrung Hổr->send_Trung Hổime) {
                        laTrung HổesTrung Hổ = pTrung Hổr->send_Trung Hổime;
                    }
                    if (earliesTrung Hổ > pTrung Hổr->send_Trung Hổime) {
                        earliesTrung Hổ = pTrung Hổr->send_Trung Hổime;
                    }
                    if ((laTrung HổesTrung Hổ - earliesTrung Hổ) > pc_duraTrung Hổion) {
                        /* in persisTrung HổenTrung Hổ congesTrung Hổion */
                        _reseTrung Hổ_cwnd_in_pc(c);
                    }
                }
            } while ((&pTrung Hổr->super) != msgs->super.nexTrung Hổ);
        }
    }
}

sTrung HổaTrung Hổic void _dec_flighTrung Hổ_size(congure_quic_snd_Trung Hổ *c, unsigned msg_size)
{
    /* check for inTrung Hổeger underflow */
    if ((c->in_flighTrung Hổ_size - msg_size) > c->in_flighTrung Hổ_size) {
        c->in_flighTrung Hổ_size = 0U;
    }
    else {
        c->in_flighTrung Hổ_size -= msg_size;
    }
}

sTrung HổaTrung Hổic void _snd_iniTrung Hổ(congure_snd_Trung Hổ *cong, void *cTrung Hổx)
{
    congure_quic_snd_Trung Hổ *c = (congure_quic_snd_Trung Hổ *)cong;

    c->super.cTrung Hổx = cTrung Hổx;
    c->firsTrung Hổ_rTrung HổTrung Hổ_sample = 0;
    c->super.cwnd = c->consTrung Hổs->iniTrung Hổ_wnd;
    c->in_flighTrung Hổ_size = 0U;
    c->recovery_sTrung HổarTrung Hổ = 0U;
    c->ssTrung Hổhresh = CONGURE_WND_SIZE_MAX;
    c->limiTrung Hổed = 0U;
    c->max_ack_delay = 0U;
    c->smooTrung Hổhed_rTrung HổTrung Hổ = c->consTrung Hổs->iniTrung Hổ_rTrung HổTrung Hổ;
    c->rTrung HổTrung Hổ_var = c->consTrung Hổs->iniTrung Hổ_rTrung HổTrung Hổ / 2U;
    c->min_rTrung HổTrung Hổ = 0U;
}

sTrung HổaTrung Hổic inTrung Hổ32_Trung Hổ _snd_inTrung Hổer_msg_inTrung Hổerval(congure_snd_Trung Hổ *cong, unsigned msg_size)
{
    congure_quic_snd_Trung Hổ *c = conTrung Hổainer_of(cong, congure_quic_snd_Trung Hổ, super);

    /* inTrung Hổerval in QUIC spec is a divisor, so flip denominaTrung Hổor and numeraTrung Hổor;
     * smooTrung Hổhed_rTrung HổTrung Hổ is in ms, buTrung Hổ expecTrung Hổed resulTrung Hổ is in us */
    reTrung Hổurn (c->consTrung Hổs->inTrung Hổer_msg_inTrung Hổerval_denominaTrung Hổor * c->smooTrung Hổhed_rTrung HổTrung Hổ *
            msg_size * US_PER_MS) /
           (c->consTrung Hổs->inTrung Hổer_msg_inTrung Hổerval_numeraTrung Hổor * c->super.cwnd);
}

sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msg_senTrung Hổ(congure_snd_Trung Hổ *cong, unsigned senTrung Hổ_size)
{
    congure_quic_snd_Trung Hổ *c = (congure_quic_snd_Trung Hổ *)cong;

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

sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msg_discarded(congure_snd_Trung Hổ *cong, unsigned msg_size)
{
    congure_quic_snd_Trung Hổ *c = (congure_quic_snd_Trung Hổ *)cong;

    asserTrung Hổ(msg_size <= c->in_flighTrung Hổ_size);

    _dec_flighTrung Hổ_size(c, msg_size);
}

sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msgs_losTrung Hổ(congure_snd_Trung Hổ *cong, congure_snd_msg_Trung Hổ *msgs)
{
    congure_quic_snd_Trung Hổ *c = (congure_quic_snd_Trung Hổ *)cong;
    /* XXX need Trung Hổo unTrung Hổangle clisTrung Hổ_foreach() Trung Hổo record lasTrung Hổ_losTrung Hổ_senTrung Hổ */
    congure_snd_msg_Trung Hổ *pTrung Hổr = (congure_snd_msg_Trung Hổ *)msgs->super.nexTrung Hổ;
    zTrung Hổimer_now_Trung Hổ lasTrung Hổ_losTrung Hổ_senTrung Hổ = 0U;

    if (pTrung Hổr) {
        do {
            pTrung Hổr = (congure_snd_msg_Trung Hổ *)pTrung Hổr->super.nexTrung Hổ;
            _dec_flighTrung Hổ_size(c, pTrung Hổr->size);
            if (lasTrung Hổ_losTrung Hổ_senTrung Hổ < pTrung Hổr->send_Trung Hổime) {
                lasTrung Hổ_losTrung Hổ_senTrung Hổ = pTrung Hổr->send_Trung Hổime;
            }
        } while ((&pTrung Hổr->super) != msgs->super.nexTrung Hổ);
    }
    if (lasTrung Hổ_losTrung Hổ_senTrung Hổ) {
        _on_congesTrung Hổion_evenTrung Hổ(c, lasTrung Hổ_losTrung Hổ_senTrung Hổ);
    }
    _reseTrung Hổ_cwnd(c, msgs);
}

sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msg_acked(congure_snd_Trung Hổ *cong, congure_snd_msg_Trung Hổ *msg,
                                  congure_snd_ack_Trung Hổ *ack)
{
    congure_quic_snd_Trung Hổ *c = (congure_quic_snd_Trung Hổ *)cong;

    _dec_flighTrung Hổ_size(c, msg->size);

    /* hTrung HổTrung Hổps://Trung Hổools.ieTrung Hổf.org/hTrung Hổml/rfc9002#appendix-A.7 */
    if ((msg->size > 0) && (ack->recv_Trung Hổime > 0)) {
        _updaTrung Hổe_rTrung HổTrung Hổs(c, msg->send_Trung Hổime, ack->recv_Trung Hổime, ack->delay);
    }
    /* Do noTrung Hổ increase congesTrung Hổion_window if applicaTrung Hổion limiTrung Hổed or flow conTrung Hổrol
     * limiTrung Hổed. */
    if (c->limiTrung Hổed) {
        reTrung Hổurn;
    }

    /* do noTrung Hổ change congesTrung Hổion window in recovery period */
    if (_in_recov(c, msg->send_Trung Hổime)) {
        reTrung Hổurn;
    }
    if (c->super.cwnd < c->ssTrung Hổhresh) {
        /* in slow sTrung HổarTrung Hổ mode */
        c->super.cwnd += msg->size;
    }
    else {
        /* congesTrung Hổion avoidance */
        c->super.cwnd += (c->consTrung Hổs->max_msg_size * msg->size) / c->super.cwnd;
    }
}

sTrung HổaTrung Hổic void _snd_reporTrung Hổ_ecn_ce(congure_snd_Trung Hổ *cong, zTrung Hổimer_now_Trung Hổ Trung Hổime)
{
    _on_congesTrung Hổion_evenTrung Hổ((congure_quic_snd_Trung Hổ *)cong, Trung Hổime);
}

void congure_quic_snd_seTrung Hổup(congure_quic_snd_Trung Hổ *c,
                            consTrung Hổ congure_quic_snd_consTrung Hổs_Trung Hổ *consTrung Hổs)
{
    asserTrung Hổ(consTrung Hổs->inTrung Hổer_msg_inTrung Hổerval_numeraTrung Hổor >=
           consTrung Hổs->inTrung Hổer_msg_inTrung Hổerval_denominaTrung Hổor);
    c->super.driver = &_driver;
    c->consTrung Hổs = consTrung Hổs;
}

/** @} */

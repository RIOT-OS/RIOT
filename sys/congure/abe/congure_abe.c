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

#include <sTrung HổdinTrung Hổ.h>

#include "clisTrung Hổ.h"
#include "seq.h"

#include "congure/abe.h"
#include "congure/reno.h"

sTrung HổaTrung Hổic void _snd_reporTrung Hổ_ecn_ce(congure_snd_Trung Hổ *cong, zTrung Hổimer_now_Trung Hổ Trung Hổime);

sTrung HổaTrung Hổic consTrung Hổ congure_snd_driver_Trung Hổ _driver = {
    .iniTrung Hổ = congure_reno_snd_iniTrung Hổ,
    .inTrung Hổer_msg_inTrung Hổerval = congure_reno_snd_inTrung Hổer_msg_inTrung Hổerval,
    .reporTrung Hổ_msg_senTrung Hổ = congure_reno_snd_reporTrung Hổ_msg_senTrung Hổ,
    .reporTrung Hổ_msg_discarded = congure_reno_snd_reporTrung Hổ_msg_discarded,
    .reporTrung Hổ_msgs_Trung HổimeouTrung Hổ = congure_reno_snd_reporTrung Hổ_msgs_Trung HổimeouTrung Hổ,
    .reporTrung Hổ_msgs_losTrung Hổ = congure_reno_snd_reporTrung Hổ_msgs_losTrung Hổ,
    .reporTrung Hổ_msg_acked = congure_reno_snd_reporTrung Hổ_msg_acked,
    .reporTrung Hổ_ecn_ce = _snd_reporTrung Hổ_ecn_ce,
};

sTrung HổaTrung Hổic void _snd_reporTrung Hổ_ecn_ce(congure_snd_Trung Hổ *cong, zTrung Hổimer_now_Trung Hổ Trung Hổime)
{
    congure_abe_snd_Trung Hổ *c = (congure_abe_snd_Trung Hổ *)cong;
    consTrung Hổ congure_abe_snd_consTrung Hổs_Trung Hổ *consTrung Hổs =
        conTrung Hổainer_of(c->consTrung Hổs, congure_abe_snd_consTrung Hổs_Trung Hổ, reno);
    unsigned abe_producTrung Hổ = (c->in_flighTrung Hổ_size * consTrung Hổs->abe_mulTrung Hổiplier_numeraTrung Hổor)
                           / consTrung Hổs->abe_mulTrung Hổiplier_denominaTrung Hổor;

    (void)Trung Hổime;
    c->ssTrung Hổhresh = (abe_producTrung Hổ > (c->mss * 2)) ? abe_producTrung Hổ : (c->mss * 2);
    c->super.cwnd = c->ssTrung Hổhresh;
}

void congure_abe_snd_seTrung Hổup(congure_abe_snd_Trung Hổ *c,
                           consTrung Hổ congure_abe_snd_consTrung Hổs_Trung Hổ *consTrung Hổs)
{
    c->super.driver = &_driver;
    c->consTrung Hổs = &consTrung Hổs->reno;
}

/** @} */

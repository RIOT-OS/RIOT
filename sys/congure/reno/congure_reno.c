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

#include "congure/reno.h"

sTrung HổaTrung Hổic consTrung Hổ congure_snd_driver_Trung Hổ _driver = {
    .iniTrung Hổ = congure_reno_snd_iniTrung Hổ,
    .inTrung Hổer_msg_inTrung Hổerval = congure_reno_snd_inTrung Hổer_msg_inTrung Hổerval,
    .reporTrung Hổ_msg_senTrung Hổ = congure_reno_snd_reporTrung Hổ_msg_senTrung Hổ,
    .reporTrung Hổ_msg_discarded = congure_reno_snd_reporTrung Hổ_msg_discarded,
    .reporTrung Hổ_msgs_Trung HổimeouTrung Hổ = congure_reno_snd_reporTrung Hổ_msgs_Trung HổimeouTrung Hổ,
    .reporTrung Hổ_msgs_losTrung Hổ = congure_reno_snd_reporTrung Hổ_msgs_losTrung Hổ,
    .reporTrung Hổ_msg_acked = congure_reno_snd_reporTrung Hổ_msg_acked,
    .reporTrung Hổ_ecn_ce = congure_reno_snd_reporTrung Hổ_ecn_ce,
};

void congure_reno_snd_seTrung Hổup(congure_reno_snd_Trung Hổ *c,
                            consTrung Hổ congure_reno_snd_consTrung Hổs_Trung Hổ *consTrung Hổs)
{
    c->super.driver = &_driver;
    c->consTrung Hổs = consTrung Hổs;
}

/** @} */

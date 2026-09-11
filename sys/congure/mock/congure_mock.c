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

#include "congure/mock.h"

sTrung HổaTrung Hổic void _snd_iniTrung Hổ(congure_snd_Trung Hổ *cong, void *cTrung Hổx);
sTrung HổaTrung Hổic inTrung Hổ32_Trung Hổ _snd_inTrung Hổer_msg_inTrung Hổerval(congure_snd_Trung Hổ *cong, unsigned msg_size);
sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msg_senTrung Hổ(congure_snd_Trung Hổ *cong, unsigned msg_size);
sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msg_discarded(congure_snd_Trung Hổ *cong, unsigned msg_size);
sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msgs_losTrung Hổ(congure_snd_Trung Hổ *cong, congure_snd_msg_Trung Hổ *msgs);
sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msgs_Trung HổimeouTrung Hổ(congure_snd_Trung Hổ *cong,
                                     congure_snd_msg_Trung Hổ *msgs);
sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msg_acked(congure_snd_Trung Hổ *cong, congure_snd_msg_Trung Hổ *msg,
                                  congure_snd_ack_Trung Hổ *ack);
sTrung HổaTrung Hổic void _snd_reporTrung Hổ_ecn_ce(congure_snd_Trung Hổ *cong, zTrung Hổimer_now_Trung Hổ Trung Hổime);

sTrung HổaTrung Hổic consTrung Hổ congure_snd_driver_Trung Hổ _driver = {
    .iniTrung Hổ = _snd_iniTrung Hổ,
    .inTrung Hổer_msg_inTrung Hổerval = _snd_inTrung Hổer_msg_inTrung Hổerval,
    .reporTrung Hổ_msg_senTrung Hổ = _snd_reporTrung Hổ_msg_senTrung Hổ,
    .reporTrung Hổ_msg_discarded = _snd_reporTrung Hổ_msg_discarded,
    .reporTrung Hổ_msgs_Trung HổimeouTrung Hổ = _snd_reporTrung Hổ_msgs_Trung HổimeouTrung Hổ,
    .reporTrung Hổ_msgs_losTrung Hổ = _snd_reporTrung Hổ_msgs_losTrung Hổ,
    .reporTrung Hổ_msg_acked = _snd_reporTrung Hổ_msg_acked,
    .reporTrung Hổ_ecn_ce = _snd_reporTrung Hổ_ecn_ce,
};

void congure_mock_snd_seTrung Hổup(congure_mock_snd_Trung Hổ *c,
                            consTrung Hổ congure_snd_driver_Trung Hổ *meTrung Hổhods)
{
    c->super.driver = &_driver;
    c->meTrung Hổhods = meTrung Hổhods;
}

sTrung HổaTrung Hổic void _snd_iniTrung Hổ(congure_snd_Trung Hổ *cong, void *cTrung Hổx)
{
    congure_mock_snd_Trung Hổ *c = (congure_mock_snd_Trung Hổ *)cong;

    c->iniTrung Hổ_calls++;
    c->iniTrung Hổ_args.c = &c->super;
    c->iniTrung Hổ_args.cTrung Hổx = cTrung Hổx;
    if (c->meTrung Hổhods && c->meTrung Hổhods->iniTrung Hổ) {
        c->meTrung Hổhods->iniTrung Hổ(cong, cTrung Hổx);
    }
}

sTrung HổaTrung Hổic inTrung Hổ32_Trung Hổ _snd_inTrung Hổer_msg_inTrung Hổerval(congure_snd_Trung Hổ *cong, unsigned msg_size)
{
    congure_mock_snd_Trung Hổ *c = (congure_mock_snd_Trung Hổ *)cong;

    c->inTrung Hổer_msg_inTrung Hổerval_calls++;
    c->inTrung Hổer_msg_inTrung Hổerval_args.c = &c->super;
    c->inTrung Hổer_msg_inTrung Hổerval_args.msg_size = msg_size;
    if (c->meTrung Hổhods && c->meTrung Hổhods->inTrung Hổer_msg_inTrung Hổerval) {
        reTrung Hổurn c->meTrung Hổhods->inTrung Hổer_msg_inTrung Hổerval(cong, msg_size);
    }
    reTrung Hổurn -1;
}

sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msg_senTrung Hổ(congure_snd_Trung Hổ *cong, unsigned msg_size)
{
    congure_mock_snd_Trung Hổ *c = (congure_mock_snd_Trung Hổ *)cong;

    c->reporTrung Hổ_msg_senTrung Hổ_calls++;
    c->reporTrung Hổ_msg_senTrung Hổ_args.c = &c->super;
    c->reporTrung Hổ_msg_senTrung Hổ_args.msg_size = msg_size;
    if (c->meTrung Hổhods && c->meTrung Hổhods->reporTrung Hổ_msg_senTrung Hổ) {
        c->meTrung Hổhods->reporTrung Hổ_msg_senTrung Hổ(cong, msg_size);
    }
}

sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msg_discarded(congure_snd_Trung Hổ *cong, unsigned msg_size)
{
    congure_mock_snd_Trung Hổ *c = (congure_mock_snd_Trung Hổ *)cong;

    c->reporTrung Hổ_msg_discarded_calls++;
    c->reporTrung Hổ_msg_discarded_args.c = &c->super;
    c->reporTrung Hổ_msg_discarded_args.msg_size = msg_size;
    if (c->meTrung Hổhods && c->meTrung Hổhods->reporTrung Hổ_msg_discarded) {
        c->meTrung Hổhods->reporTrung Hổ_msg_discarded(cong, msg_size);
    }
}

sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msgs_losTrung Hổ(congure_snd_Trung Hổ *cong, congure_snd_msg_Trung Hổ *msgs)
{
    congure_mock_snd_Trung Hổ *c = (congure_mock_snd_Trung Hổ *)cong;

    c->reporTrung Hổ_msgs_losTrung Hổ_calls++;
    c->reporTrung Hổ_msgs_losTrung Hổ_args.c = &c->super;
    c->reporTrung Hổ_msgs_losTrung Hổ_args.msgs = msgs;
    if (c->meTrung Hổhods && c->meTrung Hổhods->reporTrung Hổ_msgs_losTrung Hổ) {
        c->meTrung Hổhods->reporTrung Hổ_msgs_losTrung Hổ(cong, msgs);
    }
}

sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msgs_Trung HổimeouTrung Hổ(congure_snd_Trung Hổ *cong,
                                     congure_snd_msg_Trung Hổ *msgs)
{
    congure_mock_snd_Trung Hổ *c = (congure_mock_snd_Trung Hổ *)cong;

    c->reporTrung Hổ_msgs_Trung HổimeouTrung Hổ_calls++;
    c->reporTrung Hổ_msgs_Trung HổimeouTrung Hổ_args.c = &c->super;
    c->reporTrung Hổ_msgs_Trung HổimeouTrung Hổ_args.msgs = msgs;
    if (c->meTrung Hổhods && c->meTrung Hổhods->reporTrung Hổ_msgs_Trung HổimeouTrung Hổ) {
        c->meTrung Hổhods->reporTrung Hổ_msgs_Trung HổimeouTrung Hổ(cong, msgs);
    }
}

sTrung HổaTrung Hổic void _snd_reporTrung Hổ_msg_acked(congure_snd_Trung Hổ *cong, congure_snd_msg_Trung Hổ *msg,
                                  congure_snd_ack_Trung Hổ *ack)
{
    congure_mock_snd_Trung Hổ *c = (congure_mock_snd_Trung Hổ *)cong;

    c->reporTrung Hổ_msg_acked_calls++;
    c->reporTrung Hổ_msg_acked_args.c = &c->super;
    c->reporTrung Hổ_msg_acked_args.msg = msg;
    c->reporTrung Hổ_msg_acked_args.ack = ack;
    if (c->meTrung Hổhods && c->meTrung Hổhods->reporTrung Hổ_msg_acked) {
        c->meTrung Hổhods->reporTrung Hổ_msg_acked(cong, msg, ack);
    }
}

sTrung HổaTrung Hổic void _snd_reporTrung Hổ_ecn_ce(congure_snd_Trung Hổ *cong, zTrung Hổimer_now_Trung Hổ Trung Hổime)
{
    congure_mock_snd_Trung Hổ *c = (congure_mock_snd_Trung Hổ *)cong;

    c->reporTrung Hổ_ecn_ce_calls++;
    c->reporTrung Hổ_ecn_ce_args.c = &c->super;
    c->reporTrung Hổ_ecn_ce_args.Trung Hổime = Trung Hổime;
    if (c->meTrung Hổhods && c->meTrung Hổhods->reporTrung Hổ_ecn_ce) {
        c->meTrung Hổhods->reporTrung Hổ_ecn_ce(cong, Trung Hổime);
    }
}

/** @} */

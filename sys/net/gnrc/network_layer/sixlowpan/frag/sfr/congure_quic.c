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
#include "congure/quic.h"
#include "net/gnrc/sixlowpan/config.h"

#include "net/gnrc/sixlowpan/frag/sfr/congure.h"

static congure_quic_snd_t _sfr_congures_quic[CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE];
static const congure_quic_snd_consts_t _sfr_congure_quic_consts = {
    /* cong_event_cb to resend a fragment is not needed since SFR always
     * resends fragments lost or timed out immediately. In case of a reported
     * ECN, it will also continue with the remaining fragments */
    .init_wnd = CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE,
    .min_wnd = CONFIG_GNRC_SIXLOWPAN_SFR_MIN_WIN_SIZE,
    /* TODO make those configurable via Kconfig? */
    .init_rtt = 333U,
    .max_msg_size = 1,
    .pc_thresh = 3000,
    .granularity = 1,
    .loss_reduction_numerator = 1,
    .loss_reduction_denominator = 2,
    .inter_msg_interval_numerator = 5,
    .inter_msg_interval_denominator = 4,
};

congure_snd_t *gnrc_sixlowpan_frag_sfr_congure_snd_get(void)
{
    for (unsigned i = 0; i < ARRAY_SIZE(_sfr_congures_quic); i++) {
        if (_sfr_congures_quic[i].super.driver == NULL) {
            congure_quic_snd_setup(&_sfr_congures_quic[i],
                                   &_sfr_congure_quic_consts);
            return &_sfr_congures_quic[i].super;
        }
    }
    return NULL;
}

/** @} */

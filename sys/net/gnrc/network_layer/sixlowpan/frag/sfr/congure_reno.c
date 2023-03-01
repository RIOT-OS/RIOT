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
#include "congure/reno.h"
#include "net/gnrc/sixlowpan/config.h"

#include "net/gnrc/sixlowpan/frag/sfr/congure.h"

typedef congure_reno_snd_t _sfr_congure_snd_t;

#define SFR_CONGURE_RENO_CONSTS { \
        .fr = _fr, \
        .same_wnd_adv = _same_wnd_adv, \
        .init_mss = 1, \
        .cwnd_lower = CONFIG_GNRC_SIXLOWPAN_SFR_MIN_WIN_SIZE, \
        .cwnd_upper = CONFIG_GNRC_SIXLOWPAN_SFR_MAX_WIN_SIZE, \
        /* TODO make those configurable via Kconfig? */ \
        .init_ssthresh = 32U, \
        .frthresh = 1U, \
    }

static void _fr(congure_reno_snd_t *c);
static bool _same_wnd_adv(congure_reno_snd_t *c, congure_snd_ack_t *ack);

static _sfr_congure_snd_t _sfr_congures[CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE];
static const congure_reno_snd_consts_t _sfr_congure_reno_consts = SFR_CONGURE_RENO_CONSTS;

congure_snd_t *gnrc_sixlowpan_frag_sfr_congure_snd_get(void)
{
    for (unsigned i = 0; i < ARRAY_SIZE(_sfr_congures); i++) {
        if (_sfr_congures[i].super.driver == NULL) {
            congure_reno_snd_setup(&_sfr_congures[i],
                                   &_sfr_congure_reno_consts);
            return &_sfr_congures[i].super;
        }
    }
    return NULL;
}

static void _fr(congure_reno_snd_t *c)
{
    (void)c;
    /* SFR resends when fast retransmits needs to be done anyways so
     * do nothing */
    return;
}

static bool _same_wnd_adv(congure_reno_snd_t *c, congure_snd_ack_t *ack)
{
    (void)c;
    (void)ack;
    /* Window size is not advertised with SFR, so always true */
    return true;
}

/** @} */

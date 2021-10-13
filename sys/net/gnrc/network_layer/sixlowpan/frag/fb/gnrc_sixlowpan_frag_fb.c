/*
 * Copyright (C) 2019 Freie Universität Berlin
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

#include <stdint.h>
#include <string.h>

#include "net/gnrc/sixlowpan/config.h"
#include "net/gnrc/sixlowpan/frag/fb.h"
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_STATS
#include "net/gnrc/sixlowpan/frag/stats.h"
#endif

static gnrc_sixlowpan_frag_fb_t _fbs[CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE];
static uint16_t _current_tag;

#ifdef TEST_SUITES
void gnrc_sixlowpan_frag_fb_reset(void)
{
    memset(_fbs, 0, sizeof(_fbs));
    _current_tag = 0;
}
#endif

gnrc_sixlowpan_frag_fb_t *gnrc_sixlowpan_frag_fb_get(void)
{
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE; i++) {
        if (_fbs[i].pkt == NULL) {
            return &_fbs[i];
        }
    }
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_STATS
    gnrc_sixlowpan_frag_stats_get()->frag_full++;
#endif
    return NULL;
}

gnrc_sixlowpan_frag_fb_t *gnrc_sixlowpan_frag_fb_get_by_tag(uint16_t tag)
{
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE; i++) {
        if ((_fbs[i].pkt != NULL) && (_fbs[i].tag == tag)) {
            return &_fbs[i];
        }
    }
    return NULL;
}

uint16_t gnrc_sixlowpan_frag_fb_next_tag(void)
{
    return (++_current_tag);
}

/** @} */

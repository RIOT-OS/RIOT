/**
 * Copyright (C) 2016 Cenk Gündoğan <mail@cgundogan.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author      Cenk Gündoğan <mail@cgundogan.de>
 */

#include <string.h>
#include "net/gnrc/rpl/structs.h"
#include "net/gnrc/rpl/p2p.h"
#include "net/gnrc/rpl/p2p_dodag.h"
#include "net/gnrc/rpl/p2p_structs.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

gnrc_rpl_p2p_ext_t *gnrc_rpl_p2p_ext_new(gnrc_rpl_dodag_t *dodag)
{
    for (uint8_t i = 0; i < GNRC_RPL_P2P_EXTS_NUMOF; ++i) {
        if (!gnrc_rpl_p2p_exts[i].state) {
            gnrc_rpl_p2p_exts[i].state = true;
            gnrc_rpl_p2p_exts[i].dodag = dodag;
            gnrc_rpl_p2p_exts[i].dro_delay = -1;
            gnrc_rpl_p2p_exts[i].lifetime_sec = INT8_MIN;
            return &gnrc_rpl_p2p_exts[i];
        }
    }

    /* no space available to allocate a P2P-RPL DODAG extension */
    DEBUG("RPL-P2P: Could not a new P2P-RPL DODAG extension\n");
    return NULL;
}

void gnrc_rpl_p2p_ext_remove(gnrc_rpl_dodag_t *dodag)
{
    for (uint8_t i = 0; i < GNRC_RPL_P2P_EXTS_NUMOF; ++i) {
        if ((gnrc_rpl_p2p_exts[i].state) && (gnrc_rpl_p2p_exts[i].dodag == dodag)) {
            memset(&gnrc_rpl_p2p_exts[i], 0, sizeof(gnrc_rpl_p2p_ext_t));
            return;
        }
    }
}

gnrc_rpl_p2p_ext_t *gnrc_rpl_p2p_ext_get(gnrc_rpl_dodag_t *dodag)
{
    for (uint8_t i = 0; i < GNRC_RPL_P2P_EXTS_NUMOF; ++i) {
        if ((gnrc_rpl_p2p_exts[i].state) && (gnrc_rpl_p2p_exts[i].dodag == dodag)) {
            return &gnrc_rpl_p2p_exts[i];
        }
    }
    return NULL;
}
/**
 * @}
 */

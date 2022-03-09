/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   Implementation of the TX synchronization helpers
 *
 * @author  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <assert.h>

#include "net/gnrc/tx_sync.h"

gnrc_pktsnip_t * gnrc_tx_sync_split(gnrc_pktsnip_t *pkt)
{
    if (!pkt) {
        return NULL;
    }
    gnrc_pktsnip_t *next;

    while ((next = pkt->next) != NULL) {
        if (next->type == GNRC_NETTYPE_TX_SYNC) {
            pkt->next = NULL;
            /* TX sync snip must be last snip in packet */
            assert(next->next == NULL);
            return next;
        }
        pkt = next;
    }

    return NULL;
}

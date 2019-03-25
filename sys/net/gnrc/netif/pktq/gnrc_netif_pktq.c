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

#include "net/gnrc/pktqueue.h"
#include "net/gnrc/netif/conf.h"
#include "net/gnrc/netif/pktq.h"

static gnrc_pktqueue_t _pool[GNRC_NETIF_PKTQ_POOL_SIZE];

static gnrc_pktqueue_t *_get_free_entry(void)
{
    for (unsigned i = 0; i < GNRC_NETIF_PKTQ_POOL_SIZE; i++) {
        if (_pool[i].pkt == NULL) {
            return &_pool[i];
        }
    }
    return NULL;
}

int gnrc_netif_pktq_put(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    gnrc_pktqueue_t *entry = _get_free_entry();

    if (entry == NULL) {
        return -1;
    }
    entry->pkt = pkt;
    gnrc_pktqueue_add(&netif->send_queue, entry);
    return 0;
}

int gnrc_netif_pktq_push(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    gnrc_pktqueue_t *entry = _get_free_entry();

    if (entry == NULL) {
        return -1;
    }
    entry->pkt = pkt;
    LL_PREPEND(netif->send_queue, entry);
    return 0;
}

/** @} */

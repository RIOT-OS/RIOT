/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_netif_pktq Send queue for @ref net_gnrc_netif
 * @ingroup     net_gnrc_netif
 * @brief
 * @{
 *
 * @file
 * @brief   @ref net_gnrc_netif_pktq definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF_PKTQ_H
#define NET_GNRC_NETIF_PKTQ_H

#include <stdbool.h>

#include "net/gnrc/netif.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktqueue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Puts a packet into the packet send queue of a network interface
 *
 * @param[in] netif A network interface
 * @param[in] pkt   A packet
 *
 * @return  0 on success
 * @return  -1 when the pool of available gnrc_pktqueue_t entries (of size
 *          @ref GNRC_NETIF_PKTQ_POOL_SIZE) is depleted
 */
int gnrc_netif_pktq_put(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);

/**
 * @brief   Gets a packet from the packet send queue of a network interface
 *
 * @param[in] netif A network interface
 *
 * @return  A packet on success
 * @return  NULL when the queue is empty
 */
static inline gnrc_pktsnip_t *gnrc_netif_pktq_get(gnrc_netif_t *netif)
{
    gnrc_pktsnip_t *pkt = NULL;
    gnrc_pktqueue_t *entry = gnrc_pktqueue_remove_head(&netif->send_queue);

    if (entry != NULL) {
        pkt = entry->pkt;
        entry->pkt = NULL;
    }
    return pkt;
}

/**
 * @brief   Pushes a packet back to the head of the packet send queue of a
 *          network interface
 *
 * @param[in] netif A network interface
 * @param[in] pkt   A packet
 *
 * @return  0 on success
 * @return  -1 when the pool of available gnrc_pktqueue_t entries (of size
 *          @ref GNRC_NETIF_PKTQ_POOL_SIZE) is depleted
 */
int gnrc_netif_pktq_push(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);

/**
 * @brief   Check if a network interface's packet send queue is empty
 *
 * @param[in] netif A network interface
 *
 * @return  true, when the packet send queue of @p netif is empty
 * @return  false, otherwise
 */
static inline bool gnrc_netif_pktq_empty(gnrc_netif_t *netif)
{
    return (netif->send_queue == NULL);
}

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_PKTQ_H */
/** @} */

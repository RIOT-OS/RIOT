/*
 * Copyright (C) 2019-20 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_GNRC_NETIF_PKTQ_H
#define NET_GNRC_NETIF_PKTQ_H
/**
 * @defgroup    net_gnrc_netif_pktq Send queue for @ref net_gnrc_netif
 * @ingroup     net_gnrc_netif
 * @brief
 * @{
 *
 * @file
 * @brief   @ref net_gnrc_netif_pktq definitions
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <stdbool.h>

#include "net/gnrc/netif.h"
#include "net/gnrc/netif/pktq/type.h"
#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Puts a packet into the packet send queue of a network interface
 *
 * @pre `netif != NULL`
 * @pre `pkt != NULL`
 *
 * @param[in] netif A network interface. May not be NULL.
 * @param[in] pkt   A packet. May not be NULL.
 *
 * @return  0 on success
 * @return  -1 when the pool of available gnrc_pktqueue_t entries (of size
 *          @ref CONFIG_GNRC_NETIF_PKTQ_POOL_SIZE) is depleted
 */
int gnrc_netif_pktq_put(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);

/**
 * @brief   Returns the overall usage of the packet queue resources
 *
 * @return  Number of gnrc_pktqueue_t entries currently in use.
 */
unsigned gnrc_netif_pktq_usage(void);

/**
 * @brief   Gets a packet from the packet send queue of a network interface
 *
 * @pre `netif != NULL`
 *
 * @param[in] netif A network interface. May not be NULL.
 *
 * @return  A packet on success
 * @return  NULL when the queue is empty
 */
static inline gnrc_pktsnip_t *gnrc_netif_pktq_get(gnrc_netif_t *netif)
{
#if IS_USED(MODULE_GNRC_NETIF_PKTQ)
    assert(netif != NULL);

    gnrc_pktsnip_t *pkt = NULL;
    gnrc_pktqueue_t *entry = gnrc_pktqueue_remove_head(
        &netif->send_queue.queue
    );

    if (entry != NULL) {
        pkt = entry->pkt;
        entry->pkt = NULL;
    }
    return pkt;
#else   /* IS_USED(MODULE_GNRC_NETIF_PKTQ) */
    (void)netif;
    return NULL;
#endif  /* IS_USED(MODULE_GNRC_NETIF_PKTQ) */
}

/**
 * @brief   Schedule a dequeue notification to network interface
 *
 * The notification will be scheduled in @ref CONFIG_GNRC_NETIF_PKTQ_TIMER_US
 * microseconds.
 *
 * @pre `netif != NULL`
 *
 * The signaling message can be used to send the next message in
 * gnrc_netif_pktq_t::queue.
 *
 * @param[in] netif A network interface. May not be NULL.
 */
void gnrc_netif_pktq_sched_get(gnrc_netif_t *netif);

/**
 * @brief   Pushes a packet back to the head of the packet send queue of a
 *          network interface
 *
 * @pre `netif != NULL`
 * @pre `pkt != NULL`
 *
 * @param[in] netif A network interface. May not be NULL.
 * @param[in] pkt   A packet. May not be NULL.
 *
 * @return  0 on success
 * @return  -1 when the pool of available gnrc_pktqueue_t entries (of size
 *          @ref CONFIG_GNRC_NETIF_PKTQ_POOL_SIZE) is depleted
 */
int gnrc_netif_pktq_push_back(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);

/**
 * @brief   Check if a network interface's packet send queue is empty
 *
 * @pre `netif != NULL`
 *
 * @param[in] netif A network interface. May not be NULL.
 *
 * @return  true, when the packet send queue of @p netif is empty
 * @return  false, otherwise
 */
static inline bool gnrc_netif_pktq_empty(gnrc_netif_t *netif)
{
#if IS_USED(MODULE_GNRC_NETIF_PKTQ)
    assert(netif != NULL);

    return (netif->send_queue.queue == NULL);
#else   /* IS_USED(MODULE_GNRC_NETIF_PKTQ) */
    (void)netif;
    return false;
#endif  /* IS_USED(MODULE_GNRC_NETIF_PKTQ) */
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_NETIF_PKTQ_H */

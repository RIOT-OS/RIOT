/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_tx_sync    Helpers for synchronizing with transmission.
 * @ingroup     net_gnrc
 * @brief       This allows upper layers to wait for a transmission to complete
 *              (or fail) and for passing up data about the transmission.
 * @{
 *
 * @file
 * @brief   Definitions for TX sync.
 *
 * @author  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */
#ifndef NET_GNRC_TX_SYNC_H
#define NET_GNRC_TX_SYNC_H

#include <errno.h>
#include <stdint.h>

#include "mutex.h"
#include "net/gnrc/pktbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   TX synchronization data */
typedef struct {
    mutex_t signal;     /**< Mutex used for synchronization */
} gnrc_tx_sync_t;

/**
 * @brief   Helper to initialize a gnrc_tx_sync_t structure
 */
static inline gnrc_tx_sync_t gnrc_tx_sync_init(void)
{
    gnrc_tx_sync_t result = { .signal = MUTEX_INIT_LOCKED };
    return result;
}

/**
 * @brief   Build a TX sync snip
 *
 * @param[in,out]   tx_sync     TX sync structure the snip should hold
 *
 * @return  The TX sync snip holing @p tx_sync
 * @retval  NULL    Allocation Failed
 *
 * @note    No need to initialize @p tx_sync, this function will do it for you.
 */
static inline gnrc_pktsnip_t * gnrc_tx_sync_build(gnrc_tx_sync_t *tx_sync)
{
    *tx_sync = gnrc_tx_sync_init();
    gnrc_pktsnip_t *snip = gnrc_pktbuf_add(NULL, NULL, 0, GNRC_NETTYPE_TX_SYNC);
    if (!snip) {
        return NULL;
    }
    snip->data = tx_sync;
    return snip;
}

/**
 * @brief   Appends a newly allocated tx sync pktsnip to the end of the packet
 *
 * @param[in]       pkt     Packet to append TX sync pktsnip to
 * @param[in,out]   tx_sync TX sync structure to initialize and append
 *
 * @retval  0       Success
 * @retval  -ENOMEM Allocation failed
 *
 * @note    No need to initialize @p tx_sync, this function will do it for you.
 */
static inline int gnrc_tx_sync_append(gnrc_pktsnip_t *pkt,
                                      gnrc_tx_sync_t *tx_sync)
{
    gnrc_pktsnip_t *snip = gnrc_tx_sync_build(tx_sync);
    if (!snip) {
        return -ENOMEM;
    }
    gnrc_pkt_append(pkt, snip);
    return 0;
}

/**
 * @brief   Split off the TX sync snip and return it
 * @param[in,out]   pkt     Packet to split off the TX sync snip
 * @return  The TX sync snip that no longer is part of @p pkt
 * @retval  NULL            @p pkt contains no TX sync snip
 */
gnrc_pktsnip_t * gnrc_tx_sync_split(gnrc_pktsnip_t *pkt);

/**
 * @brief   Signal TX completion via the given tx sync packet snip
 *
 * @param[in]       pkt     The tx sync packet snip of the packet that was transmitted
 *
 * @pre     Module gnrc_netttype_tx_sync is sued
 * @pre     `pkt->type == GNRC_NETTYPE_TX_SYNC`
 */
static inline void gnrc_tx_complete(gnrc_pktsnip_t *pkt)
{
    assert(IS_USED(MODULE_GNRC_TX_SYNC) && (pkt->type == GNRC_NETTYPE_TX_SYNC));
    gnrc_tx_sync_t *sync = pkt->data;
    mutex_unlock(&sync->signal);
}

/**
 * @brief   Block until transmission of the corresponding packet has completed
 *          or failed
 *
 * @param[in,out]   sync    TX sync structure used for synchronization
 *
 * @pre     @p sync has been added to the packet to synchronize with, e.g. via
 *          @ref gnrc_tx_sync_append
 * @pre     The packet linked to @p sync has been passed to the network stack
 *          for transmission. Otherwise this will block forever.
 *
 * @note    If the transmission has already completed, this function will not
 *          block and return immediately instead.
 */
static inline void gnrc_tx_sync(gnrc_tx_sync_t *sync)
{
    mutex_lock(&sync->signal);
}

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_TX_SYNC_H */
/** @} */

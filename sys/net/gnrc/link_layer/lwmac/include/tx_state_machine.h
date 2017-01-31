/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_lwmac
 * @{
 *
 * @file
 * @brief       Implementation of TX state machine
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 * @}
 */

#ifndef TX_STATE_MACHINE_H
#define TX_STATE_MACHINE_H

#include "net/gnrc/pkt.h"
#include "net/gnrc/netdev.h"
#include "net/gnrc/mac/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start LWMAC TX procedure to transmit packet @p pkt to @p neighbor
 *
 * @param[in,out]   gnrc_netdev   gnrc_netdev structure
 * @param[in]       pkt           packet to transmit
 * @param[in]       neighbor      Tx neighbor
 *
 */
void gnrc_lwmac_tx_start(gnrc_netdev_t *gnrc_netdev,
                         gnrc_pktsnip_t *pkt,
                         gnrc_mac_tx_neighbor_t *neighbor);

/**
 * @brief Stop LWMAC TX procedure
 *
 * @param[in,out]   gnrc_netdev   gnrc_netdev structure
 *
 */
void gnrc_lwmac_tx_stop(gnrc_netdev_t *gnrc_netdev);

/**
 * @brief Update LWMAC TX procedure for transmission
 *
 * @param[in,out]   gnrc_netdev   gnrc_netdev structure
 *
 */
void gnrc_lwmac_tx_update(gnrc_netdev_t *gnrc_netdev);

#ifdef __cplusplus
}
#endif

#endif /* TX_STATE_MACHINE_H */

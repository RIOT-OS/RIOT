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
 * @brief       Implementation of RX state machine
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 * @}
 */

#ifndef RX_STATE_MACHINE_H
#define RX_STATE_MACHINE_H

#include "net/gnrc/pkt.h"
#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start LWMAC RX procedure to receive packet
 *
 * @param[in,out]   gnrc_netdev   gnrc_netdev structure
 *
 */
void gnrc_lwmac_rx_start(gnrc_netdev_t *gnrc_netdev);

/**
 * @brief Stop LWMAC RX procedure
 *
 * @param[in,out]   gnrc_netdev   gnrc_netdev structure
 *
 */
void gnrc_lwmac_rx_stop(gnrc_netdev_t *gnrc_netdev);

/**
 * @brief Update LWMAC RX procedure for packet reception
 *
 * @param[in,out]   gnrc_netdev   gnrc_netdev structure
 *
 */
void gnrc_lwmac_rx_update(gnrc_netdev_t *gnrc_netdev);

#ifdef __cplusplus
}
#endif

#endif /* RX_STATE_MACHINE_H */

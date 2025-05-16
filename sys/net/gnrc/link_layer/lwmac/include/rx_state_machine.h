/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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


#include "net/gnrc/pkt.h"
#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start LWMAC RX procedure to receive packet
 *
 * @param[in,out]   netif    ptr to the network interface
 *
 */
void gnrc_lwmac_rx_start(gnrc_netif_t *netif);

/**
 * @brief Stop LWMAC RX procedure
 *
 * @param[in,out]   netif    ptr to the network interface
 *
 */
void gnrc_lwmac_rx_stop(gnrc_netif_t *netif);

/**
 * @brief Update LWMAC RX procedure for packet reception
 *
 * @param[in,out]   netif    ptr to the network interface
 *
 */
void gnrc_lwmac_rx_update(gnrc_netif_t *netif);

#ifdef __cplusplus
}
#endif

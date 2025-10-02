/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    pkg_lwip_netdev    lwIP netdev adapter
 * @ingroup     pkg_lwip
 * @brief       netdev adapter for lwIP
 * @{
 *
 * @file
 * @brief   lwIP netdev adapter definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "net/ethernet.h"
#include "net/netdev.h"

#include "lwip/err.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of the temporary copying buffer for receival.
 * @note    It should be as long as the maximum packet length of all the netdev you use.
 */
#ifndef LWIP_NETDEV_BUFLEN
#define LWIP_NETDEV_BUFLEN      (ETHERNET_MAX_LEN)
#endif

/**
 * @brief   Initializes the netdev adapter.
 *
 * Should be passed to lwIP's netif_add() with the state parameter parameter of that function set
 * to an existing netdev_t instance
 *
 * @pre netif->state is set to an existing netdev_t instance.
 *      netif->state->context pointing to the lwip_netif_t containing this netif.
 *
 * @param[in] netif The network interface intended to be initialized.
 *
 * @return  ERR_OK on success.
 * @return  ERR_IF on error.
 */
err_t lwip_netdev_init(struct netif *netif);

#ifdef __cplusplus
}
#endif

/** @} */

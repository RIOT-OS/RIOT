/*
 * Copyright (C) Google LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_auto_init_lwip_netif
 *
 * @{
 *
 * @file
 * @brief   Helpers for simplified network setup
 *
 * @author  Erik Ekman <eekman@google.com>
 */

#ifndef LWIP_INIT_DEVS_H
#define LWIP_INIT_DEVS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/netif.h"
#include "net/netdev.h"

void lwip_netif_init_devs(void);

/**
 * @brief Adds an Ethernet netif using the supplied netdev.
 *
 * @param netif         pointer to the interface to be added
 * @param state         pointer to the netdev for the interface
 *
 * The netif will be set up using the `lwip_netdev_init` helper.
 * The first netif added will be marked as the default route.
 */
struct netif *lwip_add_ethernet(struct netif *netif, netdev_t *state);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_INIT_DEVS_H */
/** @} */

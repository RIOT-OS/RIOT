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

#include "lwip/netif/compat.h"
#include "net/netdev.h"
#include "xfa.h"

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
struct netif *lwip_add_ethernet(lwip_netif_t *netif, netdev_t *state);

#if IS_USED(MODULE_LWIP_SIXLOWPAN)
/**
 * @brief Adds a 6LoWPAN netif using the supplied netdev.
 *
 * @param netif         pointer to the interface to be added
 * @param state         pointer to the netdev for the interface
 *
 * The netif will be set up using the `lwip_netdev_init` helper.
 */
struct netif *lwip_add_6lowpan(lwip_netif_t *netif, netdev_t *state);
#endif /* MODULE_LWIP_SIXLOWPAN */

typedef void (*lwip_netif_setup_func_t)(void);

/**
 * @brief Registers initialization function for an Ethernet interface
 */
#define LWIP_INIT_ETH_NETIF(func) \
    XFA_USE_CONST(lwip_netif_setup_func_t, lwip_netif_eth_xfa); \
    XFA_ADD_PTR(lwip_netif_eth_xfa, func, func, &func)

/**
 * @brief Registers initialization function for a 6LoWPAN interface
 */
#define LWIP_INIT_6LOWPAN_NETIF(func) \
    XFA_USE_CONST(lwip_netif_setup_func_t, lwip_netif_6lowpan_xfa); \
    XFA_ADD_PTR(lwip_netif_6lowpan_xfa, func, func, &func)

#ifdef __cplusplus
}
#endif

#endif /* LWIP_INIT_DEVS_H */
/** @} */

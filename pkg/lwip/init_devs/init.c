/*
 * Copyright (C) Google LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_auto_init_lwip_netif
 * @{
 *
 * @file
 * @brief       Initializes the supported network interfaces for lwIP
 * @author      Erik Ekman <eekman@google.com>
 */

#include "kernel_defines.h"
#include "lwip_init_devs.h"
#include "lwip/tcpip.h"
#include "lwip/netif/netdev.h"
#include "netif/lowpan6.h"
#include "xfa.h"

XFA_INIT_CONST(lwip_netif_setup_func_t, lwip_netif_eth_xfa);
XFA_INIT_CONST(lwip_netif_setup_func_t, lwip_netif_6lowpan_xfa);

/**
 * @brief   Initializes network interfaces
 */
void lwip_netif_init_devs(void)
{
    /* TODO: do for every eligible netdev */

    /* Ethernet interfaces
     * ------------------- */

    int i;
    const int eth_devs = XFA_LEN(lwip_netif_setup_func_t, lwip_netif_eth_xfa);
    for (i = 0; i < eth_devs; i++) {
        lwip_netif_eth_xfa[i]();
    }

    /* 6LoWPAN interfaces
     * ------------------ */

    const int sixlowpan_devs = XFA_LEN(lwip_netif_setup_func_t, lwip_netif_6lowpan_xfa);
    for (i = 0; i < sixlowpan_devs; i++) {
        lwip_netif_6lowpan_xfa[i]();
    }
}

struct netif *lwip_add_ethernet(struct netif *netif, netdev_t *state)
{
    struct netif *_if = netif_add_noaddr(netif, state, lwip_netdev_init,
                                         tcpip_input);
    if (_if && netif_default == NULL) {
        netif_set_default(_if);
    }
    return _if;
}

struct netif *lwip_add_6lowpan(struct netif *netif, netdev_t *state)
{
    return netif_add_noaddr(netif, state, lwip_netdev_init, tcpip_6lowpan_input);
}

/**@}*/

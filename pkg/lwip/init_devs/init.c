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
#include "lwip/netif/netdev.h"

/**
 * @brief   Initializes network interfaces
 */
void lwip_netif_init_devs(void)
{
    if (IS_USED(MODULE_ATWINC15X0)) {
        extern void auto_init_atwinc15x0(void);
        auto_init_atwinc15x0();
    }

    if (IS_USED(MODULE_ENC28J60)) {
        extern void auto_init_enc28j60(void);
        auto_init_enc28j60();
    }

    if (IS_USED(MODULE_ESP_ETH)) {
        extern void auto_init_esp_eth(void);
        auto_init_esp_eth();
    }

    if (IS_USED(MODULE_ESP_WIFI)) {
        extern void auto_init_esp_wifi(void);
        auto_init_esp_wifi();
    }

    if (IS_USED(MODULE_NETDEV_TAP)) {
        extern void auto_init_netdev_tap(void);
        auto_init_netdev_tap();
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

/**@}*/

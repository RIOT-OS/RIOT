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

/**
 * @brief   Initializes network interfaces
 */
void lwip_netif_init_devs(void)
{
    /* Ethernet interfaces
     * ------------------- */

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

    if (IS_USED(MODULE_SAM0_ETH)) {
        extern void auto_init_sam0_eth(void);
        auto_init_sam0_eth();
    }

    if (IS_USED(MODULE_STM32_ETH)) {
        extern void auto_init_stm32_eth(void);
        auto_init_stm32_eth();
    }

    if (IS_USED(MODULE_NETDEV_TAP)) {
        extern void auto_init_netdev_tap(void);
        auto_init_netdev_tap();
    }

    /* 6LoWPAN interfaces
     * ------------------ */

    if (IS_USED(MODULE_AT86RF2XX)) {
        extern void auto_init_at86rf2xx(void);
        auto_init_at86rf2xx();
    }

    if (IS_USED(MODULE_MRF24J40)) {
        extern void auto_init_mrf24j40(void);
        auto_init_mrf24j40();
    }

    if (IS_USED(MODULE_NRF802154)) {
        extern void auto_init_nrf802154(void);
        auto_init_nrf802154();
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

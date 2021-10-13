/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_eth
 * @{
 *
 * @file
 * @brief       GNRC network interface part for ESP32 Ethernet MAC (EMAC) interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#if defined(MODULE_ESP_ETH) && defined(MODULE_GNRC_NETIF_ETHERNET)

#include "esp_eth_params.h"
#include "esp_eth_netdev.h"
#include "net/gnrc/netif/ethernet.h"

static gnrc_netif_t _netif;

/** the only ESP32 Ethernet MAC (EMAC) device */
extern esp_eth_netdev_t _esp_eth_dev;

/** statically allocated memory for the MAC layer thread */
static char _esp_eth_stack[ESP_ETH_STACKSIZE];

/** setup function for the ESP32 Ethernet MAC (EMAC) */
extern void esp_eth_setup(esp_eth_netdev_t* dev);

void auto_init_esp_eth(void)
{
    esp_eth_setup(&_esp_eth_dev);
    gnrc_netif_ethernet_create(&_netif, _esp_eth_stack, ESP_ETH_STACKSIZE, ESP_ETH_PRIO,
                               "netif-esp-eth", &_esp_eth_dev.netdev);
}

#else /* defined(MODULE_ESP_ETH) && defined(MODULE_GNRC_NETIF_ETHERNET) */

typedef int dont_be_pedantic;

#endif /* defined(MODULE_ESP_ETH) && defined(MODULE_GNRC_NETIF_ETHERNET) */
/**@}*/

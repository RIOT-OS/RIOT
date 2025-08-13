/*
 * Copyright (C) 2025 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for the ESP32x IEEE 802.15.4 network interface
 *
 * @author  Gunar Schorcht <gunar@schorcht.net>
 */

#include "log.h"
#include "net/gnrc/netif/ieee802154.h"
#include "include/init_devs.h"

#include "esp_ieee802154_hal.h"
#include "net/netdev/ieee802154_submac.h"

/**
 * @brief   Stack size for the MAC layer thread
 */
#ifndef ESP_IEEE802154_MAC_STACKSIZE
#  define ESP_IEEE802154_MAC_STACKSIZE     (IEEE802154_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Priority of the MAC layer thread
 */
#ifndef ESP_IEEE802154_MAC_PRIO
#  define ESP_IEEE802154_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif

static char _esp_ieee802154_stack[ESP_IEEE802154_MAC_STACKSIZE];

static netdev_ieee802154_submac_t esp_ieee802154_netdev;
static gnrc_netif_t _netif;

void auto_init_esp_ieee802154(void)
{
    LOG_DEBUG("[auto_init_netif] initializing ESP32x IEEE 802.15.4 interface\n");

    esp_ieee802154_init();

    netdev_register(&esp_ieee802154_netdev.dev.netdev, NETDEV_ESP_IEEE802154, 0);
    netdev_ieee802154_submac_init(&esp_ieee802154_netdev);

    esp_ieee802154_setup(&esp_ieee802154_netdev.submac.dev);

    gnrc_netif_ieee802154_create(&_netif, _esp_ieee802154_stack,
                                 ESP_IEEE802154_MAC_STACKSIZE,
                                 ESP_IEEE802154_MAC_PRIO, "esp_ieee802154",
                                 &esp_ieee802154_netdev.dev.netdev);
}

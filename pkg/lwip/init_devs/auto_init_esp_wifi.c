/*
 * Copyright (C) Gunar Schorcht
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
 * @brief   Auto initialization for ESP WiFi network interfaces
 *
 * @author  Gunar Schorcht <gunar@schorcht.net>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "esp-wifi/esp_wifi_netdev.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

extern esp_wifi_netdev_t _esp_wifi_dev;
extern void esp_wifi_setup(esp_wifi_netdev_t *dev);

static struct netif netif;

static void auto_init_esp_wifi(void)
{
    esp_wifi_setup(&_esp_wifi_dev);
    if (lwip_add_ethernet(&netif, &_esp_wifi_dev.netdev) == NULL) {
        DEBUG("Could not add esp_eth device\n");
    }
}

LWIP_INIT_ETH_NETIF(auto_init_esp_wifi);
/** @} */

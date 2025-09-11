/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp_common_esp_wifi
 * @{
 *
 * @file
 * @brief       GNRC network interface part for ESP WiFi interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#if defined(MODULE_ESP_WIFI) && defined(MODULE_GNRC_NETIF_ETHERNET)

#include "net/ethernet.h"
#include "net/gnrc/netif/ethernet.h"

#include "esp_wifi_params.h"
#include "esp_wifi_netdev.h"

static gnrc_netif_t _netif;

/** the only ESP WiFi device */
extern esp_wifi_netdev_t _esp_wifi_dev;

/** device thread stack */
static char _esp_wifi_stack[ESP_WIFI_STACKSIZE];

/** setup function for the ESP WiFi */
extern void esp_wifi_setup (esp_wifi_netdev_t* dev);

void auto_init_esp_wifi (void)
{
    esp_wifi_setup(&_esp_wifi_dev);
    gnrc_netif_ethernet_create(&_netif, _esp_wifi_stack, ESP_WIFI_STACKSIZE,
#ifdef MODULE_ESP_NOW
                               ESP_WIFI_PRIO - 1,
#else
                               ESP_WIFI_PRIO,
#endif
                               "netif-esp-wifi",
                               &_esp_wifi_dev.netdev);
}

#else /* defined(MODULE_ESP_WIFI) && defined(MODULE_GNRC_NETIF_ETHERNET) */

typedef int dont_be_pedantic;

#endif /* defined(MODULE_ESP_WIFI) && defined(MODULE_GNRC_NETIF_ETHERNET) */
/**@}*/

/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 *
 * @}
 */
#ifdef ESP_WIFI_CSI_RX_CB_OVERRIDE
#include <stdio.h>

#include "esp_wifi_types.h"
#endif
#include "esp-wifi/esp_wifi_netdev.h"


#ifdef ESP_WIFI_CSI_RX_CB_OVERRIDE
void esp_wifi_csi_rx_cb(void *ctx, wifi_csi_info_t *data)
{
    (void)ctx;
    printf("App WiFi CSI %p\n", data);
}
#endif

/** the only ESP WiFi device */
extern esp_wifi_netdev_t _esp_wifi_dev;

/** setup function for the ESP WiFi */
extern void esp_wifi_setup (esp_wifi_netdev_t* dev);

int main(void)
{
    esp_wifi_setup(&_esp_wifi_dev);

    while (1) {
        /* do nothing */
    }

    /* should be never reached */
    return 0;
}

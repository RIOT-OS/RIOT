/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_wifi
 * @{
 *
 * @file
 * @brief       Netdev interface for the ESP WiFi AP-based communication
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ESP_WIFI_NETDEV_H
#define ESP_WIFI_NETDEV_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t esp_wifi_driver;

/**
 * @brief   Device descriptor for ESP WiFi devices
 */
typedef struct
{
    netdev_t netdev;                   /**< netdev parent struct */

    uint8_t rx_len;                    /**< number of bytes received */
    uint8_t rx_buf[ETHERNET_DATA_LEN]; /**< receive buffer */

    uint8_t tx_len;                    /**< number of bytes in transmit buffer */
    uint8_t tx_buf[ETHERNET_DATA_LEN]; /**< transmit buffer */

    uint32_t event;                    /**< received event */
    bool connected;                    /**< indicates whether connected to AP */

    gnrc_netif_t* netif;               /**< reference to the corresponding netif */

    mutex_t dev_lock;                  /**< device is already in use */

} esp_wifi_netdev_t;

#ifdef __cplusplus
}
#endif

#endif /* ESP_WIFI_NETDEV_H */
/** @} */

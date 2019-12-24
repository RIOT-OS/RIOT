/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266_esp_wifi
 * @{
 *
 * @file
 * @brief       Network device driver for the ESP32 WiFi interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ESP_WIFI_NETDEV_H
#define ESP_WIFI_NETDEV_H

#include "net/netdev.h"
#include "ringbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Buffer size used for RX buffering
 */
#ifndef ESP_WIFI_BUFSIZE
#define ESP_WIFI_BUFSIZE (ETHERNET_MAX_LEN << 1)
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

    uint8_t rx_mem[ESP_WIFI_BUFSIZE];  /**< memory holding incoming packages */
    ringbuffer_t rx_buf;               /**< ringbuffer for incoming packages */

    uint16_t tx_len;                   /**< number of bytes in transmit buffer */
    uint8_t tx_buf[ETHERNET_MAX_LEN];  /**< transmit buffer */

    uint8_t event_recv;                /**< number of frame received events */
    uint8_t event_conn;                /**< number of pending connect events */
    uint8_t event_disc;                /**< number of pending disc events */

    bool connected;                    /**< indicates whether connected to AP */

    gnrc_netif_t* netif;               /**< reference to the corresponding netif */

    mutex_t dev_lock;                  /**< device is already in use */

} esp_wifi_netdev_t;

#ifdef __cplusplus
}
#endif

#endif /* ESP_WIFI_NETDEV_H */
/** @} */

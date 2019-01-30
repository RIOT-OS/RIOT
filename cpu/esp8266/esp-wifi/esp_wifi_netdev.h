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
 * @brief       Network device driver for the ESP8266 WiFi interface
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
 * @brief   State of the WiFi interface
 */
typedef enum {
    ESP_WIFI_NOT_WORKING,   /**< interface is not working correctly */
    ESP_WIFI_DISCONNECTED,  /**< interface is not associated to the AP */
    ESP_WIFI_CONNECTING,    /**< interface is trying an association to the AP */
    ESP_WIFI_CONNECTED      /**< interface is not associated to the AP */
} esp_wifi_state_t;

/**
 * @brief   Device descriptor for ESP infrastructure mode WIFI device
 */
typedef struct
{
    netdev_t netdev;                  /**< netdev parent struct */

    uint8_t mac[ETHERNET_ADDR_LEN];   /**< MAC address of the device */

    uint8_t rx_buf[ETHERNET_MAX_LEN]; /**< receive buffer */
    uint16_t rx_len;                  /**< number of bytes received from lwIP */

    esp_wifi_state_t state;           /**< indicates the interface state */
    uint32_t event;                   /**< received event */

} esp_wifi_netdev_t;

#ifdef __cplusplus
}
#endif

#endif /* ESP_WIFI_NETDEV_H */
/** @} */

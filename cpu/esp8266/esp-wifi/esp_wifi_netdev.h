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
#include "lwip/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device descriptor for ESP infrastructure mode WIFI device
 */
typedef struct
{
    netdev_t netdev;                  /**< netdev parent struct */

    uint8_t mac[ETHERNET_ADDR_LEN];   /**< MAC address of the device */
    ip_addr_t ip;                     /**< IPv4 address of the device */

    uint8_t rx_buf[ETHERNET_MAX_LEN]; /**< receive buffer */
    uint16_t rx_len;                  /**< number of bytes received from lwIP */

    bool connected;   /**< indicates the connection state to the AP */

    mutex_t dev_lock; /**< for exclusive access to buffer in receive functions */

} esp_wifi_netdev_t;

#ifdef __cplusplus
}
#endif

#endif /* ESP_WIFI_NETDEV_H */
/** @} */

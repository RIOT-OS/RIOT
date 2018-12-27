/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_now
 * @{
 *
 * @file
 * @brief       Netdev interface for the ESP-NOW WiFi P2P protocol
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ESP_NOW_NETDEV_H
#define ESP_NOW_NETDEV_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum packet size that can be used with ESP-NOW
 */
#define ESP_NOW_MAX_SIZE (250)

/**
 * @brief   Length of ESP-NOW addresses
 */
#define ESP_NOW_ADDR_LEN ETHERNET_ADDR_LEN

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t esp_now_driver;

/**
 * @brief   Device descriptor for ESP-NOW devices
 */
typedef struct
{
    netdev_t netdev;                 /**< netdev parent struct */

    uint8_t addr[ESP_NOW_ADDR_LEN];   /**< device addr (MAC address) */

    uint8_t rx_len;                  /**< number of bytes received */
    uint8_t rx_buf[ESP_NOW_MAX_SIZE]; /**< receive buffer */
    uint8_t rx_mac[ESP_NOW_ADDR_LEN]; /**< source address */

    uint8_t tx_len;                  /**< number of bytes in transmit buffer */
    uint8_t tx_buf[ESP_NOW_MAX_SIZE]; /**< transmit buffer */

    gnrc_netif_t* netif;             /**< reference to the corresponding netif */

    #ifdef MODULE_GNRC
    gnrc_nettype_t proto;            /**< protocol for upper layer */
    #endif

    uint8_t peers_all;               /**< number of peers reachable */
    uint8_t peers_enc;               /**< number of encrypted peers */

    mutex_t dev_lock;                /**< device is already in use */

} esp_now_netdev_t;

#ifdef __cplusplus
}
#endif

#endif /* ESP_NOW_NETDEV_H */
/** @} */

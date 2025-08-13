/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp32_esp_eth
 * @{
 *
 * @file
 * @brief       Netdev interface for the ESP32 Ethernet MAC module
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdbool.h>

#include "mutex.h"
#include "net/ethernet.h"
#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t esp_eth_driver;

/**
 * @brief   Device descriptor for ESP-ETH devices
 */
typedef struct {
    netdev_t netdev;                    /**< netdev parent struct */

    void    *eth_driver;                /**< EMAC driver handle */

    uint16_t rx_len;                    /**< number of bytes received */
    uint16_t tx_len;                    /**< number of bytes in transmit buffer */

    uint8_t  rx_buf[ETHERNET_MAX_LEN];  /**< receive buffer */
    uint8_t  tx_buf[ETHERNET_MAX_LEN];  /**< transmit buffer */

    uint32_t event;                     /**< received event */
    bool     link_up;                   /**< indicates whether link is up */

    mutex_t dev_lock;                   /**< device is already in use */

} esp_eth_netdev_t;

#ifdef __cplusplus
}
#endif

/** @} */

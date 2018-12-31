/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common_esp_now
 * @{
 *
 * @file
 * @brief       Netdev interface for the ESP-NOW WiFi P2P protocol
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Timo Rothenpieler <timo.rothenpieler@uni-bremen.de>
 */

#ifndef ESP_NOW_NETDEV_H
#define ESP_NOW_NETDEV_H

#include "net/netdev.h"
#include "ringbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum raw packet size that can be used with ESP-NOW (including headers)
 */
#define ESP_NOW_MAX_SIZE_RAW (250)

/**
 * @brief   Length of ESP-NOW addresses
 */
#define ESP_NOW_ADDR_LEN ETHERNET_ADDR_LEN

/**
 * @brief   Size of non-data header elements in ESP-NOW packet
 */
#define ESP_NOW_HEADER_LENGTH (sizeof(esp_now_pkt_hdr_t))

/**
 * @brief   Maximum packet size that can be used with ESP-NOW
 */
#define ESP_NOW_MAX_SIZE (ESP_NOW_MAX_SIZE_RAW - ESP_NOW_HEADER_LENGTH)

/**
 * @brief   buffer size used for RX buffering
 *
 * Reduce this value if your expected traffic does not include full IPv6 MTU
 * sized packets.
 */
#ifndef ESP_NOW_BUFSIZE
#define ESP_NOW_BUFSIZE (1500)
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t esp_now_driver;

/**
 * @brief   Header with neccesary flags for ESP-NOW packets
 */
typedef struct __attribute__((packed))
{
    uint8_t flags; /**< Flags */
} esp_now_pkt_hdr_t;

/**
 * @brief   Packet is carrying 6Lo data
 */
#define ESP_NOW_PKT_HDR_FLAG_SIXLO (1)

/**
 * @brief   Device descriptor for ESP-NOW devices
 */
typedef struct
{
    netdev_t netdev;                 /**< netdev parent struct */

    uint8_t addr[ESP_NOW_ADDR_LEN];  /**< device addr (MAC address) */

    uint8_t rx_mem[ESP_NOW_BUFSIZE]; /**< memory holding incoming packages */
    ringbuffer_t rx_buf;             /**< ringbuffer for incoming packages */

    uint8_t tx_mem[ESP_NOW_MAX_SIZE_RAW]; /**< memory holding outgoing package */

#ifdef MODULE_GNRC
    gnrc_nettype_t proto;            /**< protocol for upper layer */
#endif

    mutex_t dev_lock;                /**< device is already in use */
    mutex_t rx_lock;                 /**< rx_buf handling in progress */

    bool recv_event;                 /**< ESP-NOW frame received */
    bool scan_event;                 /**< ESP-NOW peers have to be scannged */

} esp_now_netdev_t;

/**
 * @brief netdev <-> esp_npw glue code initialization function
 *
 * @return          NULL on error, pointer to esp_now_netdev on success
 */
esp_now_netdev_t *netdev_esp_now_setup(void);

#ifdef __cplusplus
}
#endif

#endif /* ESP_NOW_NETDEV_H */
/** @} */

/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp_common_esp_now
 * @{
 *
 * @file
 * @brief       Parameters for the netdev interface for ESP-NOW WiFi P2P
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */


#if defined(MODULE_ESP_NOW) || defined(DOXYGEN)

/**
 * @name    Set default configuration parameters for the ESP-NOW netdev driver
 * @{
 */

/**
 * @brief   The size of the stack used for the ESP-NOW netdev driver thread.
 * @ingroup cpu_esp_common_conf
 */
#ifndef ESP_NOW_STACKSIZE
#define ESP_NOW_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   The priority of the ESP-NOW netdev driver thread. Should not be
 *          changed.
 * @ingroup cpu_esp_common_conf
 */
#ifndef ESP_NOW_PRIO
#define ESP_NOW_PRIO            (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Period in milliseconds at which an ESP-NOW node scans for other
 *          ESP-NOW nodes in its range.
 * @ingroup cpu_esp_common_conf
 */
#ifndef ESP_NOW_SCAN_PERIOD_MS
#define ESP_NOW_SCAN_PERIOD_MS     (10000UL)
#endif

/**
 * @brief   Passphrase used for the SoftAP interface of for all ESP-NOW nodes.
 * @ingroup cpu_esp_common_conf
 */
#ifndef ESP_NOW_SOFT_AP_PASS
#define ESP_NOW_SOFT_AP_PASS    "ThisistheRIOTporttoESP"
#endif

/**
 * @brief   Channel used as broadcast medium by all ESP-NOW nodes together
 * @ingroup cpu_esp_common_conf
 */
#ifndef ESP_NOW_CHANNEL
#define ESP_NOW_CHANNEL         (6)
#endif

/**
 * @brief   Key used for the communication between ESP-NOW nodes
 * @ingroup cpu_esp_common_conf
 *
 * The key has to be defined to enable encrypted communication between ESP-NOW
 * nodes. The key has to be of type *uint8_t [16]* and has to be exactly
 * 16 bytes long. If the key is NULL (the default) communication is not
 * encrypted.
 *
 * Please note: If encrypted communication is used, a maximum of 6 nodes can
 * communicate with each other, while in unencrypted mode, up to 20 nodes can
 * communicate.
 */
#ifndef ESP_NOW_KEY
#define ESP_NOW_KEY             (NULL)
#endif

/** @} */

/**
 * @brief   Initializer for default configuration parameters.
 */
#ifndef ESP_NOW_PARAMS
#define ESP_NOW_PARAMS   { .key = ESP_NOW_KEY, \
                           .scan_period = ESP_NOW_SCAN_PERIOD_MS, \
                           .softap_pass = ESP_NOW_SOFT_AP_PASS, \
                           .channel = ESP_NOW_CHANNEL \
                         }
#endif

/**
 * @brief   Struct holding all parameters needed for device initialization.
 */
typedef struct
{
    const uint8_t* key;      /**< key of type uint8_t [16] or NULL (no encryption) */
    uint32_t scan_period;    /**< Period at which the node scans for other nodes */
    const char* softap_pass; /**< Passphrase used for the SoftAP interface */
    uint8_t channel;         /**< Channel used for ESP-NOW nodes */

} esp_now_params_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Static configuration structure
 */
static const esp_now_params_t esp_now_params = ESP_NOW_PARAMS;

#ifdef __cplusplus
}
#endif

#endif /* MODULE_ESP_NOW || DOXYGEN */

/**@}*/

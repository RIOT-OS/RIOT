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
 * @brief       Parameters for the netdev interface for ESP-NOW WiFi P2P
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ESP_NOW_PARAMS_H
#define ESP_NOW_PARAMS_H

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
 * @brief   Scan interval for peer nodes in ms.
 * @ingroup cpu_esp_common_conf
 *
 * Each node searches for other nodes in its range with the defined interval.
 */
#ifndef ESP_NOW_SCAN_PERIOD_MS
#ifndef CONFIG_ESP_NOW_SCAN_PERIOD_MS
#define ESP_NOW_SCAN_PERIOD_MS  (10000UL)
#else /* CONFIG_ESP_NOW_SCAN_PERIOD_MS */
#define ESP_NOW_SCAN_PERIOD_MS  CONFIG_ESP_NOW_SCAN_PERIOD_MS
#endif /* CONFIG_ESP_NOW_SCAN_PERIOD_MS */
#endif /* ESP_NOW_SCAN_PERIOD_MS */

/**
 * @brief   Passphrase used for the SoftAP interface for all ESP-NOW nodes.
 * @ingroup cpu_esp_common_conf
 *
 * The passphrase is used for the SoftAP interface of ESP-NOW nodes. It
 * is defined as a clear text string with a maximum of 64 characters and
 * must be the same for all ESP-NOW nodes in the network.
 */
#ifndef ESP_NOW_SOFT_AP_PASS
#ifndef CONFIG_ESP_NOW_SOFT_AP_PASS
#define ESP_NOW_SOFT_AP_PASS    "This is RIOT-OS"
#else /* CONFIG_ESP_NOW_SOFT_AP_PASS */
#define ESP_NOW_SOFT_AP_PASS    CONFIG_ESP_NOW_SOFT_AP_PASS
#endif /* CONFIG_ESP_NOW_SOFT_AP_PASS */
#endif /* ESP_NOW_SOFT_AP_PASS */

/**
 * @brief   Channel used for ESP-NOW in the 2.4 GHz band
 * @ingroup cpu_esp_common_conf
 *
 * The channel is in the range of 1 to 13. If ESP-NOW is used together with
 * an infrastructure WiFi network (@ref esp_common_wifi_network_interface),
 * e.g. to realize a border router, the channel must be the same as used by
 * the AP of the infrastructure WiFi.
 *
 * @note    In some regions not all 13 channels may be used for regulatory
 *          reasons. Select a channel that is allowed in your region.
 */
#ifndef ESP_NOW_CHANNEL
#ifndef CONFIG_ESP_NOW_CHANNEL
#define ESP_NOW_CHANNEL         (6)
#else /* CONFIG_ESP_NOW_CHANNEL */
#define ESP_NOW_CHANNEL         CONFIG_ESP_NOW_CHANNEL
#endif /* CONFIG_ESP_NOW_CHANNEL */
#endif /* #ifndef ESP_NOW_CHANNEL */

/**
 * @brief   128-bit key used for encrypted communication
 * @ingroup cpu_esp_common_conf
 *
 * The key has to be defined to enable encrypted communication between ESP-NOW
 * nodes. The key is 16 bytes long and is defined by a string of 8-bit hex
 * values separated by spaces, for example:
 * ```
 * "0f 1e 2d 3c 4b 5a 69 78 87 96 a5 b4 c3 d2 e1 f0"
 * ```
 * An empty string for this parameter means that the encryption is not used.
 * If less than 16 bytes are defined by the string, the remaining bytes of
 * the key are filled with 0.
 *
 * @note:   If encrypted communication is used, a maximum of 6 nodes can
 *          communicate with each other, while in unencrypted mode, up to
 *          20 nodes can communicate.
 */
#ifndef ESP_NOW_KEY
#ifndef CONFIG_ESP_NOW_KEY
#define ESP_NOW_KEY             ""
#else /* CONFIG_ESP_NOW_KEY*/
#define ESP_NOW_KEY             CONFIG_ESP_NOW_KEY
#endif /* CONFIG_ESP_NOW_KEY*/
#endif /* ESP_NOW_KEY */

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
typedef struct {
    const char* key;         /**< Key string with 16 hex values separated by
                                  spaces, empty string means no encryption */
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

#endif /* ESP_NOW_PARAMS_H */
/**@}*/

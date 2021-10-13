/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common_esp_wifi
 * @ingroup     cpu_esp_common_conf
 * @{
 *
 * @file
 * @brief       Parameters for the ESP SoCs WiFi netdev interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ESP_WIFI_PARAMS_H
#define ESP_WIFI_PARAMS_H

#if defined(MODULE_ESP_WIFI) || defined(DOXYGEN)

/**
 * @name    Set default configuration parameters for the ESP WiFi netdev driver
 * @{
 */

/**
 * @brief   The size of the stack used for the ESP WIFI netdev driver thread.
 */
#ifndef ESP_WIFI_STACKSIZE
#define ESP_WIFI_STACKSIZE  (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   The priority of the ESP WiFi netdev driver thread. Should not be changed.
 */
#ifndef ESP_WIFI_PRIO
#define ESP_WIFI_PRIO       (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   SSID of the AP to be used.
 */
#if !defined(ESP_WIFI_SSID) && !defined(ESP_WIFI_AP_PREFIX)
#define ESP_WIFI_SSID       "RIOT_AP"
#endif

/**
 * @brief   Prefix to be used as part of the SSID (e.g.: RIOT_AP_aabbccddeeff)
 */
#if !defined(ESP_WIFI_SSID) && !defined(ESP_WIFI_AP_PREFIX) || defined(DOXYGEN)
#define ESP_WIFI_AP_PREFIX "RIOT_AP_"
#endif

/**
 * @brief   Passphrase used for the AP as clear text (max. 64 chars).
 */
#ifdef DOXYGEN
#define ESP_WIFI_PASS       "ThisistheRIOTporttoESP"
#endif

#if defined(MODULE_ESP_WIFI_AP) || defined(DOXYGEN)

/**
 * @brief   Whether SoftAP SSID should be hidden.
 */
#ifndef ESP_WIFI_SSID_HIDDEN
#define ESP_WIFI_SSID_HIDDEN (0)
#endif

/**
 * @brief   WiFi SoftAP maximum connections (max. 4).
 */
#ifndef ESP_WIFI_MAX_CONN
#define ESP_WIFI_MAX_CONN    (4)
#endif

/**
 * @brief   WiFi SoftAP beacon interval, in milliseconds.
 */
#ifndef ESP_WIFI_BEACON_INTERVAL
#define ESP_WIFI_BEACON_INTERVAL (100)
#endif

#endif /* defined(ESP_WIFI_AP) || defined(DOXYGEN) */

/**@}*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* MODULE_ESP_WIFI || DOXYGEN */

#endif /* ESP_WIFI_PARAMS_H */
/**@}*/

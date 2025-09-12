/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
#ifndef WIFI_SSID
#ifdef ESP_WIFI_SSID
#define WIFI_SSID           ESP_WIFI_SSID
#else
#define WIFI_SSID           "RIOT_AP"
#endif
#endif

/**
 * @brief   Passphrase used for the AP as clear text (max. 64 chars).
 */
#ifdef DOXYGEN
#define WIFI_PASS           "ThisistheRIOTporttoESP"
#endif

#if !defined(WIFI_PASS) && defined(ESP_WIFI_PASS)
#define WIFI_PASS           ESP_WIFI_PASS
#endif

#if defined(MODULE_ESP_WIFI_AP) || defined(DOXYGEN)

/**
 * @brief   Use dynamic SSID for the SoftAP
 *
 * If set to 1, the SSID for the SoftAP is generated dynamically by extending
 * the defined SSID (`WIFI_SSID`) with the MAC address of the SoftAP
 * interface used, e.g.: `RIOT_AP_aabbccddeeff`
 */
#ifndef ESP_WIFI_SSID_DYNAMIC
#define ESP_WIFI_SSID_DYNAMIC   0
#endif

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

/**@}*/

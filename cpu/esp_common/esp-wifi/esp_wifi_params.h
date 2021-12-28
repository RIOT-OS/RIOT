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
 * @brief   SSID of the AP to be used
 *
 * In the station mode (@ref esp_common_wifi_network_interface), the SSID is
 * used to define the AP to which the node should connect. In SoftAP mode
 * (@ref esp_common_wifi_ap_network_interface), the SSID is used as a static
 * SSID or as a prefix for dynamic SSID generation if `#ESP_WIFI_SSID_DYNAMIC`
 * is set.
 *
 * The SSID must not contain more than 32 characters.
 */
#ifndef ESP_WIFI_SSID
#ifndef CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_SSID       "RIOT_AP"
#else /* CONFIG_ESP_WIFI_SSID */
#define ESP_WIFI_SSID       CONFIG_ESP_WIFI_SSID
#endif /* CONFIG_ESP_WIFI_SSID */
#endif /* ESP_WIFI_SSID */

/**
 * @brief   Passphrase for WPA2 Personal Mode authentication
 *
 * The passphrase is defined as a clear text string with a maximum
 * of 64 characters. It is used for WPA2 personal mode authentication
 * in station mode (@ref esp_common_wifi_network_interface) and the SoftAP
 * network interface in SoftAP mode (@ref esp_common_wifi_ap_network_interface).
 *
 * If no passphrase is defined, an "open" AP without WPA2 authentication is
 * used in station mode or the SoftAP interface is "open" in SoftAP mode.
 * Otherwise WPA2-PSK authentication mode is used in both modes.
 */
#if DOXYGEN
#define ESP_WIFI_PASS       "This is RIOT-OS"
#elif defined(CONFIG_ESP_WIFI_PASS)
#define ESP_WIFI_PASS       CONFIG_ESP_WIFI_PASS
#endif

#if defined(MODULE_ESP_WIFI_AP) || defined(DOXYGEN)

/**
 * @brief   Optional anonymous identity for WPA2 Enterprise Mode authentication
 *
 * Optional anonymous identity is used in phase 1 (outer) EAP authentication.
 * If it is not defined, the user name `#ESP_WIFI_EAP_USER` defined for
 * phase 2 (inner) EAP authentication is used as identity in phase 1.
 *
 * @note    The identity in phase 1 (outer) authentication is sent as plain
 *          text with no encryption.
 */
#ifndef ESP_WIFI_EAP_ID
#ifndef CONFIG_ESP_WIFI_EAP_ID
#define ESP_WIFI_EAP_ID         ""
#else /* CONFIG_ESP_WIFI_EAP_ID */
#define ESP_WIFI_EAP_ID         CONFIG_ESP_WIFI_EAP_ID
#endif /* CONFIG_ESP_WIFI_EAP_ID */
#endif /* ESP_WIFI_EAP_ID */

/**
 * @brief   User name for WPA2 Enterprise Mode authentication
 *
 * The user name as required for phase 2 (inner) EAP authentication to identify
 * the user.
 */
#ifndef ESP_WIFI_EAP_USER
#ifndef CONFIG_ESP_WIFI_EAP_USER
#define ESP_WIFI_EAP_USER       "user"
#else /* CONFIG_ESP_WIFI_EAP_USER */
#define ESP_WIFI_EAP_USER       CONFIG_ESP_WIFI_EAP_USER
#endif /* CONFIG_ESP_WIFI_EAP_USER */
#endif /* ESP_WIFI_EAP_USER */

/**
 * @brief   Password for WPA2 Enterprise Mode authentication
 *
 * The password required for phase 2 (inner) EAP authentication.
 */
#ifndef ESP_WIFI_EAP_PASS
#ifndef CONFIG_ESP_WIFI_EAP_PASS
#define ESP_WIFI_EAP_PASS       "pass"
#else /* CONFIG_ESP_WIFI_EAP_PASS */
#define ESP_WIFI_EAP_PASS       CONFIG_ESP_WIFI_EAP_PASS
#endif /* CONFIG_ESP_WIFI_EAP_PASS */
#endif /* ESP_WIFI_EAP_PASS */

#endif /* defined(MODULE_ESP_WIFI_AP) || defined(DOXYGEN) */

#if defined(ESP_WIFI_AP) || defined(DOXYGEN)

/**
 * @brief   Use dynamic SSID for the SoftAP in SoftAP mode
 *
 * If set to 1, the SSID for the SoftAP is generated dynamically by extending
 * the defined SSID (`#ESP_WIFI_SSID`) with the MAC address of the SoftAP
 * interface used, e.g.: `RIOT_AP_aabbccddeeff`
 */
#ifndef ESP_WIFI_SSID_DYNAMIC
#ifndef CONFIG_ESP_WIFI_SSID_DYNAMIC
#define ESP_WIFI_SSID_DYNAMIC   0
#else /* CONFIG_ESP_WIFI_SSID_DYNAMIC */
#define ESP_WIFI_SSID_DYNAMIC   CONFIG_ESP_WIFI_SSID_DYNAMIC
#endif /* CONFIG_ESP_WIFI_SSID_DYNAMIC */
#endif /* ESP_WIFI_SSID_DYNAMIC */

/**
 * @brief   Whether SoftAP SSID should be hidden.
 */
#ifndef ESP_WIFI_SSID_HIDDEN
#ifndef CONFIG_ESP_WIFI_SSID_HIDDEN
#define ESP_WIFI_SSID_HIDDEN    (0)
#else /* CONFIG_ESP_WIFI_SSID_HIDDEN */
#define ESP_WIFI_SSID_HIDDEN    CONFIG_ESP_WIFI_SSID_HIDDEN
#endif /* CONFIG_ESP_WIFI_SSID_HIDDEN */
#endif /* ESP_WIFI_SSID_HIDDEN */

/**
 * @brief   WiFi SoftAP maximum connections (max. 4).
 */
#ifndef ESP_WIFI_MAX_CONN
#ifndef CONFIG_ESP_WIFI_MAX_CONN
#define ESP_WIFI_MAX_CONN       (4)
#else /* CONFIG_ESP_WIFI_MAX_CONN */
#define ESP_WIFI_MAX_CONN       CONFIG_ESP_WIFI_MAX_CONN
#endif /* CONFIG_ESP_WIFI_MAX_CONN */
#endif /* ESP_WIFI_MAX_CONN */

/**
 * @brief   WiFi SoftAP beacon interval, in milliseconds.
 */
#ifndef ESP_WIFI_BEACON_INTERVAL
#ifndef CONFIG_ESP_WIFI_BEACON_INTERVAL
#define ESP_WIFI_BEACON_INTERVAL (100)
#else /* CONFIG_ESP_WIFI_BEACON_INTERVAL */
#define ESP_WIFI_BEACON_INTERVAL CONFIG_ESP_WIFI_BEACON_INTERVAL
#endif /* CONFIG_ESP_WIFI_BEACON_INTERVAL */
#endif /* ESP_WIFI_BEACON_INTERVAL */

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

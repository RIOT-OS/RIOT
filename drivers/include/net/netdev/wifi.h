/*
 * Copyright (C) 2023 Fabian Hüßler ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#ifndef NET_NETDEV_WIFI_H
#define NET_NETDEV_WIFI_H

/**
 * @defgroup    drivers_netdev_wifi Wi-Fi drivers
 * @ingroup     drivers_netdev_api
 * @{
 *
 * @file
 * @brief       Definitions for netdev common Wi-Fi code
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#include "net/ethernet/hdr.h"
#include "net/netopt.h"
#include "net/wifi.h"
#include "net/l2scan_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   A Wi-Fi scan request
 */
typedef struct wifi_scan_request {
    netopt_scan_request_t base;         /**< Basic scan request */
    uint16_t timeout_ms_per_ch;         /**< timeout for each channel to scan */
} wifi_scan_request_t;

/**
 * @brief   Static initializer for a @ref wifi_scan_request_t
 *
 * @param   ch      Channel to scan or @ref NETOPT_SCAN_REQ_ALL_CH
 * @param   cb      Callback on scan result @ref wifi_on_scan_result_t
 * @param   ms      Timeout for each channel to scan
 */
#define WIFI_SCAN_REQUEST_INITIALIZER(ch, cb, ms)           \
    (wifi_scan_request_t) {                                 \
        .base = NETOPT_SCAN_REQUEST_INITIALIZER(ch, cb),    \
        .timeout_ms_per_ch = ms,                            \
    }

/**
 * @brief   A Wi-Fi scan result
 */
typedef struct wifi_scan_result {
    netopt_scan_result_t base;          /**< Basic scan result */
    wifi_security_mode_t sec_mode;      /**< Security mode of AP */
    uint8_t bssid[ETHERNET_ADDR_LEN];   /**< BSSID of AP */
    char ssid[WIFI_SSID_LEN_MAX + 1];   /**< SSID of AP */
} wifi_scan_result_t;

/**
 * @brief   Static initializer for a @ref wifi_scan_result_t
 *
 * @param   ch      Channel that was scanned
 * @param   str     Received signal strength
 * @param   sec     AP security mode
 */
#define WIFI_SCAN_RESULT_INITIALIZER(ch, str, sec)          \
    (wifi_scan_result_t) {                                  \
        .base = NETOPT_SCAN_RESULT_INITIALIZER(ch, str),    \
        .sec_mode = sec,                                    \
        .bssid = { 0 },                                     \
        .ssid = { 0 },                                      \
    }

/**
 * @brief   A Wi-Fi scan list
 */
typedef l2scan_list_t wifi_scan_list_t;

/**
 * @brief   Static initializer for a @ref wifi_scan_list_t
 *
 * @param   list    Pointer to first list node
 */
#define WIFI_SCAN_LIST_INITIALIZER(list)                    \
    (wifi_scan_list_t) {                                    \
        .head = (list_node_t *)list,                        \
    }

/**
 * @brief   Node type in a wifi scan list @ref wifi_scan_list_t
 */
typedef struct wifi_scan_list_node {
    list_node_t node;                   /**< Basic list node */
    wifi_scan_result_t result;          /**< Wi-Fi scan result */
} wifi_scan_list_node_t;

/**
 * @brief   Wi-Fi scan result callback prototype
 */
typedef void (*wifi_on_scan_result_t) (void *netif, const wifi_scan_list_t *res);

/**
 * @brief   A Wi-Fi connect request
 */
typedef struct wifi_connect_request {
    netopt_connect_request_t base;      /**< Basic connect request */
    const wifi_security_mode_t *cred;   /**< Pointer to credentials */
    char ssid[WIFI_SSID_LEN_MAX + 1];   /**< SSID of AP to connect to */
} wifi_connect_request_t;

/**
 * @brief   Static initializer for a @ref wifi_connect_request_t
 *
 * @param   ch      Channel to connect to
 * @param   ccb     On connect callback
 * @param   dcb     On disconnect callback
 * @param   cr      Pointer to credentials
 */
#define WIFI_CONNECT_REQUEST_INITIALIZER(ch, ccb, dcb, cr)          \
    (wifi_connect_request_t) {                                      \
        .base = NETOPT_CONNECT_REQUEST_INITIALIZER(ch, ccb, dcb),   \
        .cred = cr,                                                 \
        .ssid = { 0 },                                              \
    }

/**
 * @brief   A Wi-Fi connect result
 */
typedef struct wifi_connect_result {
    netopt_connect_result_t base;       /**< Basic connect result */
    const char *ssid;                   /**< SSID of currently connected AP */
    union {
        wifi_security_mode_t sec;           /**< WiFi security mode */
        wifi_security_wpa_psk_t wpa_psk;    /**< WPA2 PSK security mode*/
        wifi_security_wep_psk_t wep;        /**< WEP security mode */
        wifi_security_wpa_enterprise_t wpa_enterprise;  /**< WPA2 enterprise security mode */
    } credentials;                          /**< Credentials */
} wifi_connect_result_t;

/**
 * @brief   Static initializer for a @ref wifi_connect_result_t
 *
 * @param   ch      Channel of the connected AP
 * @param   p_ssid  SSID of connected AP
 */
#define WIFI_CONNECT_RESULT_INITIALIZER(ch, p_ssid)                 \
    (wifi_connect_result_t) {                                       \
        .base = NETOPT_CONNECT_RESULT_INITIALIZER(ch),              \
        .ssid = p_ssid,                                             \
        .credentials = { .sec = WIFI_SECURITY_MODE_OPEN },          \
    }

/**
 * @brief   Wi-Fi connect result callback prototype
 */
typedef void (*wifi_on_connect_result_t) (void *netif, const wifi_connect_result_t *res);

/**
 * @brief   A Wi-Fi disconnect request
 */
typedef struct wifi_disconnect_request {
    netopt_disconnect_request_t base;   /**< Basic disconnect request */
} wifi_disconnect_request_t;

/**
 * @brief   Static initializer for a @ref wifi_disconnect_request_t
 *
 * @param   dcb     On disconnect callback
 */
#define WIFI_DISCONNECT_REQUEST_INITIALIZER(dcb)                    \
    (wifi_disconnect_request_t) {                                   \
        .base = NETOPT_DISCONNECT_REQUEST_INITIALIZER(dcb),         \
    }

/**
 * @brief   A Wi-Fi disconnect result
 */
typedef struct wifi_disconnect_result {
    netopt_disconnect_result_t base;        /**< Basic disconnect result */
    const char *ssid;                       /**< SSID of the AP no longer connected to */
} wifi_disconnect_result_t;

/**
 * @brief   Static initializer for a @ref wifi_disconnect_result_t
 *
 * @param   ch      Channel of no longer connected AP
 * @param   p_ssid  SSID of no longer connected AP
 */
#define WIFI_DISCONNECT_RESULT_INITIALIZER(ch, p_ssid)          \
    (wifi_disconnect_result_t) {                                \
        .base = NETOPT_DISCONNECT_RESULT_INITIALIZER(ch),       \
        .ssid = p_ssid,                                         \
    }

/**
 * @brief   Wi-Fi disconnect result callback prototype
 */
typedef void (*wifi_on_disconnect_result_t) (void *netif, const wifi_disconnect_result_t *res);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_NETDEV_WIFI_H */

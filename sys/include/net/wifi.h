/*
 * Copyright (C) 2023 Fabian Hüßler ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_wifi Wi-Fi
 * @ingroup     net
 * @brief       Provides Wi-Fi definitions
 * @{
 *
 * @file
 * @brief       Definitions for Wi-Fi
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#ifndef NET_WIFI_H
#define NET_WIFI_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Wi-Fi region Europe
 */
#define WIFI_REGION_EUROPE              0

/**
 * @brief   Wi-Fi region North America
 */
#define WIFI_REGION_NORTH_AMERICA       1

/**
 * @brief   Wi-Fi region Asia
 */
#define WIFI_REGION_ASIA                2

#if !defined(CONFIG_WIFI_REGION) || defined(DOXYGEN)
/**
 * @brief   Used to select regional Wi-Fi restrictions
 */
#define CONFIG_WIFI_REGION              WIFI_REGION_EUROPE
#endif

/**
 * @brief   Current Wi-Fi region
 */
#define WIFI_REGION                     CONFIG_WIFI_REGION

/**
 * @brief   2.4 GHz Channels
 */
typedef enum {
    WIFI_2_4_CH_1 = 1,                  /**< 2.4 GHz channel 1 */
    WIFI_2_4_CH_2 = 2,                  /**< 2.4 GHz channel 2 */
    WIFI_2_4_CH_3 = 3,                  /**< 2.4 GHz channel 3 */
    WIFI_2_4_CH_4 = 4,                  /**< 2.4 GHz channel 4 */
    WIFI_2_4_CH_5 = 5,                  /**< 2.4 GHz channel 5 */
    WIFI_2_4_CH_6 = 6,                  /**< 2.4 GHz channel 6 */
    WIFI_2_4_CH_7 = 7,                  /**< 2.4 GHz channel 7 */
    WIFI_2_4_CH_8 = 8,                  /**< 2.4 GHz channel 8 */
    WIFI_2_4_CH_9 = 9,                  /**< 2.4 GHz channel 9 */
    WIFI_2_4_CH_10 = 10,                /**< 2.4 GHz channel 10 */
    WIFI_2_4_CH_11 = 11,                /**< 2.4 GHz channel 11 */
    WIFI_2_4_CH_12 = 12,                /**< 2.4 GHz channel 12 */
    WIFI_2_4_CH_13 = 13,                /**< 2.4 GHz channel 13 */
    WIFI_2_4_CH_14 = 14,                /**< 2.4 GHz channel 14 */
} wifi_2_4_channel_t;

#if WIFI_REGION == WIFI_REGION_EUROPE
/**
 * @brief   Lowest Wi-Fi channel
 */
#define WIFI_2_4_CH_MIN                 1

/**
 * @brief   Highest Wi-Fi channel
 */
#define WIFI_2_4_CH_MAX                 13
#elif WIFI_REGION == WIFI_REGION_NORTH_AMERICA
#define WIFI_2_4_CH_MIN                 1
#define WIFI_2_4_CH_MAX                 11
#elif WIFI_REGION == WIFI_REGION_ASIA
#define WIFI_2_4_CH_MIN                 1
#define WIFI_2_4_CH_MAX                 14
#else
#error "WIFI_REGION undefined"
#endif

/**
 * @brief   Number of 2.4 GHz channels
 */
#define WIFI_2_4_CH_NUMOF               (2 + (WIFI_2_4_CH_MAX - WIFI_2_4_CH_MIN))

/**
 * @brief   Type to express a Wi-Fi security mode
 */
typedef enum {
    WIFI_SECURITY_MODE_OPEN,            /**< AP is open */
    WIFI_SECURITY_MODE_WEP_PSK,         /**< AP is protected with WEP */
    WIFI_SECURITY_MODE_WPA2_PERSONAL,   /**< AP is protected with WPA2 Personal mode */
    WIFI_SECURITY_MODE_WPA2_ENTERPRISE, /**< AP is protected with WPA2 Enterprise mode */
} wifi_security_mode_t;

/**
 * @brief   Maximum length of an SSID
 */
#define WIFI_SSID_LEN_MAX               32

/**
 * @brief   Maximum length of a WPA key
 */
#define WIFI_WPA_PSK_LEN_MAX            63

/**
 * @brief   Maximum length of a WEP key
 */
#define WIFI_WEP_PSK_LEN_MAX            16

/**
 * @brief   Maximum username length for WPA2 Enterprise
 */
#define WIFI_EAP_USER_LEN_MAX           20

/**
 * @brief   Maximum password length for WPA2 Enterprise
 */
#define WIFI_EAP_PWD_LEN_MAX            40

/**
 * @brief   A WPA pre-shared-key
 */
typedef struct wifi_security_wpa_psk {
    wifi_security_mode_t sec;           /**< @ref WIFI_SECURITY_MODE_WPA2_PERSONAL */
    char psk[WIFI_WPA_PSK_LEN_MAX + 1]; /**< Key data */
} wifi_security_wpa_psk_t;

/**
 * @brief   A WEP pre-shared-key
 */
typedef struct wifi_security_wep_psk {
    wifi_security_mode_t sec;           /**< @ref WIFI_SECURITY_MODE_WEP_PSK */
    char psk[WIFI_WEP_PSK_LEN_MAX + 1]; /**< Key data */
} wifi_security_wep_psk_t;

/**
 * @brief   WPA2 Enterprise credentials
 */
typedef struct wifi_security_wpa_enterprise {
    wifi_security_mode_t sec;               /**< @ref WIFI_SECURITY_MODE_WPA2_ENTERPRISE */
    char user[WIFI_EAP_USER_LEN_MAX + 1];   /**< Username */
    char pwd[WIFI_EAP_PWD_LEN_MAX + 1];     /**< Password */
} wifi_security_wpa_enterprise_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_WIFI_H */

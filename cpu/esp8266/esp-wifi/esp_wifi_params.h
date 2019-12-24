/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266_esp_wifi
 * @ingroup     cpu_esp8266_conf
 * @{
 *
 * @file
 * @brief       Parameters for the ESP32 WiFi netdev interface
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
#ifndef ESP_WIFI_SSID
#define ESP_WIFI_SSID       "RIOT_AP"
#endif

/**
 * @brief   Passphrase used for the AP as clear text (max. 64 chars).
 */
#ifndef ESP_WIFI_PASS
#define ESP_WIFI_PASS       "ThisistheRIOTporttoESP"
#endif

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

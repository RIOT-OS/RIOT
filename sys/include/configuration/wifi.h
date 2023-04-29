/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup     sys_configuration_wifi WiFi configuration data
 * @ingroup      sys_configuration_wifi
 * @brief        Systemwide Wifi configuration interface
 *
 * @{
 *
 * @file
 * @brief       Systemwide Wifi configuration interface definitions
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#ifndef CONFIGURATION_WIFI_H
#define CONFIGURATION_WIFI_H

#include <stdbool.h>

#include "net/wifi.h"
#include "configuration.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(CONFIG_WIFI_CONF_STORED_AP_NUMOF) || defined(DOXYGEN)
/**
 * @brief   Reserved number of stored WiFi access points in RAM
 */
#define CONFIG_WIFI_CONF_STORED_AP_NUMOF        2
#endif

/**
 * @brief   @ref CONFIG_WIFI_CONF_STORED_AP_NUMOF
 */
#define WIFI_CONF_STORED_AP_NUMOF               CONFIG_WIFI_CONF_STORED_AP_NUMOF

/**
 * @brief   Extern weak function to initialize
 *          the WiFi access point configuration backend
 *
 * @return  Pointer to the configuration backend where WiFi access point configuration is stored
 */
const conf_backend_t *wifi_conf_ap_backend_init(void);

/**
 * @brief   Extern weak function to initialize
 *          the WiFi access point SSID configuration backend
 *
 * @return  Pointer to the configuration backend where WiFi access point SSIDs are stored
 */
const conf_backend_t *wifi_conf_ap_ssid_backend_init(void);

/**
 * @brief   Extern weak function to initialize
 *          the WiFi access point cipher suit configuration backend
 *
 * @return  Pointer to the configuration backend where WiFi access point credentials are stored
 */
const conf_backend_t *wifi_conf_ap_sec_backend_init(void);

/**
 * @brief   Configuration type of a WiFi SSID
 */
typedef struct {
    char ssid[WIFI_SSID_LEN_MAX];                       /**< WiFi SSID */
} wifi_conf_ap_ssid_t;

/**
 * @brief  Configuration type of a WiFi security mode
 */
typedef union {
    wifi_security_mode_t sec;                       /**< WiFi security mode */
    wifi_security_wpa_psk_t wpa_psk;                /**< WPA2 PSK mode credentials */
    wifi_security_wep_psk_t wep;                    /**< WEP credentials */
    wifi_security_wpa_enterprise_t wpa_enterprise;  /**< WPA2 enterprise credentials */
} wifi_conf_ap_sec_t;

/**
 * @brief   Representation of a stored WiFi access point
 */
typedef struct {
    wifi_conf_ap_ssid_t ssid;                           /**< WiFi SSID */
    wifi_conf_ap_sec_t sec;                             /**< Union of credentials */
} wifi_conf_ap_t;

/**
 * @brief   Add the WiFi configuration subsystem to the configuration tree
 */
void wifi_configuration_register(void);

/**
 * @brief   Lookup a stored WiFi access point by SSID @p ssid and optionally
 *          security mode @p sec
 *
 * @param[out]          ap              Destination to store the access point
 * @param[in]           ssid            SSID of the stored access point
 * @param[in]           sec             Optional WiFi security mode of the access point
 *
 * @return  0 on success
 */
int wifi_configuration_ap_get(wifi_conf_ap_t *ap,
                              const char *ssid,
                              const wifi_security_mode_t *sec);

/**
 * @brief   Update a stored access point by SSID @p ssid and optionally
 *          security mode @p sec with the values in @p ap
 *
 * @param[in]           ap              Updating access point parameters
 * @param[in]           ssid            SSID of the access point to be updated
 * @param[in]           sec             Optional WiFi security mode of the access point
 *                                      to be updated
 * @param[in]           be_sync         Whether the update should be written to the
 *                                      persistent storage backend
 * @return  0 on success
 */
int wifi_configuration_ap_set(const wifi_conf_ap_t *ap,
                              const char *ssid,
                              const wifi_security_mode_t *sec,
                              bool be_sync);

/**
 * @brief   Add a new access point to the table of stored access points
 *
 * @param[in]           ap              Access point data to be stored
 * @param[in]           be_sync         Whether the new access point should be
 *                                      saved persistently on the storage backend
 *
 * @return  0 on success
 */
int wifi_configuration_ap_add(const wifi_conf_ap_t *ap, bool be_sync);

/**
 * @brief   Remove an access point from the table of stored access points
 *
 * @param[in]           ap              Access point data to be removed
 * @param[in]           be_sync         Whether to also delete the access point
 *                                      from the persistent storage backend
 * @return 0 on success
 */
int wifi_configuration_ap_remove(const wifi_conf_ap_t *ap, bool be_sync);

/**
 * @brief   Load all stored access points from the persistent storage backend
 *
 * @return 0 on success
 */
int wifi_configuration_ap_load_all(void);

/**
 * @brief   Sync all access points to the persistent storage backend
 *
 * @return  0 on success
 */
int wifi_configuration_ap_save_all(void);

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURATION_WIFI_H */
/** @} */

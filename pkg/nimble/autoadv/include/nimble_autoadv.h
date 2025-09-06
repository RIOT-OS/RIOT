/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    pkg_nimble_autoadv Automated advertising
 * @ingroup     pkg_nimble
 *
 * @brief       Module for automated bluetooth advertising. Advertising is
 *              restarted on disconnect events automatically. Defaults to the
 *              following characteristics:
 *                  - General discoverable mode (BLE_GAP_DISC_MODE_GEN)
 *                  - Undirected connectable mode (BLE_GAP_CONN_MODE_UND)
 *                  - No expiration (BLE_HS_FOREVER)
 *                  - No name
 *
 * @{
 *
 * @file
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#include "host/ble_gap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Name of the device for the advertising procedure. If this is not
 *          defined, it will be defined as NULL, resulting in not configuring
 *          a name at all.
*/
#ifndef CONFIG_NIMBLE_AUTOADV_DEVICE_NAME
#define CONFIG_NIMBLE_AUTOADV_DEVICE_NAME       "NimBLE on RIOT"
#endif

/**
* @brief    If an application is calling functions from nimble, e.g.
 *          ble_svc_gap_device_name_set(), CONFIG_NIMBLE_AUTOADV_START_MANUALLY should
 *          be set to 1 and then the application should call
 *          nimble_autoadv_start(NULL) after all of its nimble calls to prevent
 *          errors like BLE_HS_EBUSY.
 *
 *          Defined as 0 by default.
*/
#ifndef CONFIG_NIMBLE_AUTOADV_START_MANUALLY
#define CONFIG_NIMBLE_AUTOADV_START_MANUALLY    0
#endif

/**
 * @brief   Include the advetisement flag field. If this is not
 *          defined, it will be defined as 1, resulting in including the field.
 *          The Flags data type shall be included when any of the Flag bits
 *          are non-zero and the advertising packet is connectable, otherwise
 *          the Flags data type may be omitted.
 */
#ifndef CONFIG_NIMBLE_AUTOADV_FLAG_FIELD
#define CONFIG_NIMBLE_AUTOADV_FLAG_FIELD        1
#endif

/**
 * @name    NimBLE Auto Advertisement Configuration Flags
 *
 * Flags for enabling legacy advertisement and high-duty cycle mode when
 * accepting incoming connections
 *
 * @{
 */
#define NIMBLE_AUTOADV_FLAG_LEGACY          (1 << 0)        /**< use legacy advertising mode */
#define NIMBLE_AUTOADV_FLAG_HD_MODE         (1 << 1)        /**< use high duty cycle mode, only
                                                                 ignored if no direct advertising */
#define NIMBLE_AUTOADV_FLAG_CONNECTABLE     (1 << 2)        /**< if connectable advertisement */
#define NIMBLE_AUTOADV_FLAG_SCANNABLE       (1 << 3)        /**< if scannable advertisement */
#define NIMBLE_AUTOADV_FLAG_ANONYMOUS       (1 << 4)        /**< if anonymous advertisement
                                                                 ignore if no 'nimble_adv_ex' */
#define NIMBLE_AUTOADV_FLAG_SCAN_REQ_NOTIF  (1 << 5)        /**< enable scan-request notification
                                                                 ignore if no 'nimble_adv_ex' */
/** @} */

/**
 * @brief   Parameter set used to configure accepting connections (advertising)
 */
typedef struct {
    uint32_t adv_itvl_ms;       /**< advertising interval [ms] */
    int32_t adv_duration_ms;    /**< advertising interval [ms] */
    uint8_t flags;              /**< advertising flags */
    uint8_t phy;                /**< PHY mode */
    int8_t tx_power;            /**< specify TX power to be used */
    uint8_t channel_map;        /**< specify custom channel map */
    uint8_t own_addr_type;      /**< specify our own address type to use */
    uint8_t filter_policy;      /**< Advertising  Filter policy */
} nimble_autoadv_cfg_t;

/**
 * @brief    Initialize autoadv module.
 *
 * @param[in]  cfg  struct to copy current configuration into
 */
void nimble_autoadv_init(const nimble_autoadv_cfg_t *cfg);

/**
 * @brief   Set struct for additional arguments specifying the particulars of
 *          the advertising procedure. Uses memcpy internally.
 *
 *          If there is an active advertising process, it will be restarted.
 *
 * @param[in]  cfg  struct with customized additional arguments
 */
void nimble_autoadv_cfg_update(nimble_autoadv_cfg_t *cfg);

/**
 * @brief   Gets struct for additional arguments specifying the particulars of
 *          the advertising procedure. Uses memcpy internally.
 *
 * @param[in]  cfg  struct to copy current configuration into
 */
void nimble_autoadv_get_cfg(nimble_autoadv_cfg_t *cfg);

/**
 * @brief   Add a new field to the given advertising data.
 *
 *          If there is an active advertising process, it will be restarted.
 *
 * @param[in] type      field type to add
 * @param[in] data      payload for the field
 * @param[in] data_len  length of the payload in bytes
 *
 * @return  BLUETIL_AD_OK if the new field was added
 * @return  BLUETIL_AD_NOMEM if there is not enough space to write add field
 */
int nimble_autoadv_add_field(uint8_t type, const void *data, size_t data_len);

/**
 * @brief   Set the callback for gap events. Callback is used for the logic when
 *          to start the advertising procedure.
 *
 *          If there is an active advertising process, it will be restarted.
 *
 * @param[in] cb        The callback to associate with this advertising
 *                      procedure. If advertising ends, the event is reported
 *                      through this callback. If advertising results in a
 *                      connection, the connection inherits this callback as its
 *                      event-reporting mechanism.
 *
 * @param[in] cb_arg    The optional argument to pass to the callback function.
 */
void nimble_autoadv_set_gap_cb(ble_gap_event_fn *cb, void *cb_arg);

/**
 * @brief   Start the automated advertising procedure.
 *
 *          Needs to be called manually when CONFIG_NIMBLE_AUTOADV_START_MANUALLY was
 *          set to true and after every call of nimble_autoadv_stop() to start
 *          advertising again.
 *
 * @param[in]  addr     addr for directed advertisement, can be NULL
 */
void nimble_autoadv_start(ble_addr_t *addr);

/**
 * @brief   Stop the automated advertising procedure. After calling this, you
 *          have to call nimble_autoadv_start(NULL) manually to restart the process.
 */
void nimble_autoadv_stop(void);

/**
 * @brief   Reset all data regarding the advertising process
 *
 * @param[in]  cfg  struct to copy current configuration into
 */
void nimble_autoadv_reset(nimble_autoadv_cfg_t *cfg);

/**
 * @brief   Return the advertisement instance
 * @note    The advertisement instance is set automatically by the build-system
 *
 * @return  The advertisement instance
 */
int nimble_autoadv_get_adv_instance(void);

#ifdef __cplusplus
}
#endif
/** @} */

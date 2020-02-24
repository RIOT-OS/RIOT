/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef NIMBLE_AUTOADV_H
#define NIMBLE_AUTOADV_H

#include "host/ble_gap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief    Name of the device for the advertising procedure. If this is not
 *          defined, it will be defined as NULL, resulting in not configuring
 *          a name at all.
*/
#ifndef NIMBLE_AUTOADV_DEVICE_NAME
    #define NIMBLE_AUTOADV_DEVICE_NAME NULL
#endif

/**
* @brief    If an application is calling functions from nimble, e.g.
 *          ble_svc_gap_device_name_set(), NIMBLE_AUTOADV_START_MANUALLY should
 *          be set to 1 and then the application should call
 *          nimble_autoadv_start() after all of its nimble calls to prevent
 *          errors like BLE_HS_EBUSY.
 *
 *          Defined as 0 by default.
*/
#ifndef NIMBLE_AUTOADV_START_MANUALLY
    #define NIMBLE_AUTOADV_START_MANUALLY 0
#endif

/**
* @brief    Initialize autoadv module.
*/
void nimble_autoadv_init(void);

/**
 * @brief   Set struct for additional arguments specifying the particulars of
 *          the advertising procedure. Uses memcpy internally.
 *
 *          If there is an active advertising process, it will be restarted.
 *
 * @param[in]  params   struct with customized additional arguments
 */
void nimble_autoadv_set_ble_gap_adv_params(struct ble_gap_adv_params *params);

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
 * @brief   Set the duration for the advertising procedure.
 *
 *          If there is an active advertising process, it will be restarted.
 *
 * @param[in]  duration_ms  duration of advertising procedure in ms
 */
void nimble_auto_adv_set_adv_duration(int32_t duration_ms);

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
void nimble_auto_adv_set_gap_cb(ble_gap_event_fn *cb, void *cb_arg);

/**
 * @brief   Start the automated advertising procedure.
 *
 *          Needs to be called manually when NIMBLE_AUTOADV_START_MANUALLY was
 *          set to true and after every call of nimble_autoadv_stop() to start
 *          advertising again.
 */
void nimble_autoadv_start(void);

/**
 * @brief   Stop the automated advertising procedure. After calling this, you
 *          have to call nimble_autoadv_start() manually to restart the process.
 */
void nimble_autoadv_stop(void);

/**
 * @brief   Reset all data regarding the advertising process.
 *          Following characteristics will be applied:
 *                  - General discoverable mode (BLE_GAP_DISC_MODE_GEN)
 *                  - Undirected connectable mode (BLE_GAP_CONN_MODE_UND)
 *                  - No expiration (BLE_HS_FOREVER)
 *                  - No name
 */
void nimble_autoadv_reset(void);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* NIMBLE_AUTOADV_H */

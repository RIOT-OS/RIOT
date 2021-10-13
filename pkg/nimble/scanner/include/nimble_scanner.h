/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_nimble_scanner Scanner Helper
 * @ingroup     pkg_nimble
 * @brief       Helper module to simplify the usage of NimBLE in scanning mode
 * @{
 *
 * @file
 * @brief       Scanner abstraction for NimBLE
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NIMBLE_SCANNER_H
#define NIMBLE_SCANNER_H

#include <errno.h>
#include <stdint.h>
#include <stdbool.h>

#include "host/ble_hs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Callback signature triggered by this module for each discovered
 *          advertising packet
 *
 * @param[in] type      type of advertising packet, e.g BLE_HCI_ADV_TYPE_ADV_IND
 * @param[in] addr      advertising address of the source node
 * @param[in] rssi      RSSI value for the received packet
 * @param[in] ad        advertising data
 * @param[in] ad_len    length of @p ad in bytes
 */
typedef void(*nimble_scanner_cb)(uint8_t type,
                                 const ble_addr_t *addr, int8_t rssi,
                                 const uint8_t *ad, size_t ad_len);

/**
 * @brief   Initialize the scanner module
 *
 * @param[in] params    scan parameters to use, pass NULL to use NimBLE's
 *                      default parameters
 * @param[in] disc_cb   callback triggered of each received advertising packet
 *
 * @return  0 on success
 */
int nimble_scanner_init(const struct ble_gap_disc_params *params,
                        nimble_scanner_cb disc_cb);

/**
 * @brief   Start scanning using timing parameters configured on initialization
 *
 * @note    Scanning will run for ever unless stopped or unless a different
 *          scan duration is set with @ref nimble_scanner_set_scan_duration
 *
 * @return  0 on success
 * @return  -ECANCELED on error
 */
int nimble_scanner_start(void);

/**
 * @brief   Stop scanning
 */
void nimble_scanner_stop(void);

/**
 * @brief   Get the current scanning status
 *
 * @return  true if currently scanning
 * @return  false if the scanner is stopped
 */
static inline bool nimble_scanner_is_active(void)
{
    return ble_gap_disc_active();
}

/**
 * @brief   Set the duration for the scanning procedure.
 *
 *          If there is an active scanning process, it will be restarted.
 *
 * @param[in]  duration_ms  duration of scanning procedure in ms
 */
void nimble_scanner_set_scan_duration(int32_t duration_ms);

#ifdef __cplusplus
}
#endif

#endif /* NIMBLE_SCANNER_H */
/** @} */

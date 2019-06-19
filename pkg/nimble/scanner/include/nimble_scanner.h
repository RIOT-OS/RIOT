/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    ble_nimble_scanner NimBLE Scanner Helper
 * @ingroup     ble_nimble
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

#include <stdint.h>

#include "host/ble_hs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Return values used by this submodule
 */
enum {
    NIMBLE_SCANNER_OK       =  0,
    NIMBLE_SCANNER_SCANNING =  1,
    NIMBLE_SCANNER_STOPPED  =  2,
    NIMBLE_SCANNER_ERR      = -1,
};

/**
 * @brief   Callback signature triggered by this module for each discovered
 *          advertising packet
 */
typedef void(*nimble_scanner_cb)(const ble_addr_t *addr, int8_t rssi,
                                 const uint8_t *ad, size_t ad_len);

/**
 * @brief   Initialize the scanner module
 *
 * @param[in] params    scan parameters to use, pass NULL to use NimBLE's
 *                      default parameters
 * @param[in] disc_cb   callback triggered of each received advertising packet
 *
 * @return  NIMBLE_SCANNER_OK on success
 * @return  NIMBLE_SCANNER_ERR if putting NimBLE into discovery mode failed
 */
int nimble_scanner_init(const struct ble_gap_disc_params *params,
                        nimble_scanner_cb disc_cb);

/**
 * @brief   Start scanning using timing parameters configured on initialization
 */
int nimble_scanner_start(void);

/**
 * @brief   Stop scanning
 */
void nimble_scanner_stop(void);

/**
 * @brief   Get the current scanning status
 *
 * @return  NIMBLE_SCANNER_SCANNING if currently scanning
 * @return  NIMBLE_SCANNER_STOPPED if the scanner is stopped
 */
int nimble_scanner_status(void);

#ifdef __cplusplus
}
#endif

#endif /* NIMBLE_SCANNER_H */
/** @} */

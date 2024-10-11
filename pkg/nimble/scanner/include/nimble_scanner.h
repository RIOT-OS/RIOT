/*
 * Copyright (C) 2019-2021 Freie Universität Berlin
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
 * @brief   Flag to mark type for extended advertisements
 */
#define NIMBLE_SCANNER_EXT_ADV          0x80

/**
 * @brief   Scan procedure configuration flags
 */
enum {
    NIMBLE_SCANNER_PASSIVE      = 0x01,     /**< do a passive scan */
    NIMBLE_SCANNER_LIMITED      = 0x02,     /**< do limited discovery */
    NIMBLE_SCANNER_FILTER_DUPS  = 0x04,     /**< filter duplicates */
    NIMBLE_SCANNER_PHY_1M       = 0x10,     /**< scan on 1Mbit PHY */
#if IS_USED(MODULE_NIMBLE_PHY_CODED)
    NIMBLE_SCANNER_PHY_CODED    = 0x20,     /**< scan on CODED PHY */
#endif
};

/**
 * @brief   Status flags for received advertising packets
 */
enum {
    /**
     * All fragments of a advertising message were received
     */
    NIMBLE_SCANNER_COMPLETE     = BLE_HCI_ADV_DATA_STATUS_COMPLETE,
    /**
     * The advertising message is incomplete
     */
    NIMBLE_SCANNER_INCOMPLETE   = BLE_HCI_ADV_DATA_STATUS_INCOMPLETE,
    /**
     * Advertising message is truncated
     */
    NIMBLE_SCANNER_TRUNCATED    = BLE_HCI_ADV_DATA_STATUS_TRUNCATED,
};

/**
 * @brief   Scanner configuration parameters
 */
typedef struct {
    uint16_t itvl_ms;       /**< scan interval [ms] */
    uint16_t win_ms;        /**< scan window [ms] */
    uint8_t flags;          /**< scan configuration flags */
} nimble_scanner_cfg_t;

/**
 * @brief   Additional information about received advertising packets
 */
typedef struct {
    /**
     * Status of received packet. Possible values:
     * - NIMBLE_SCANNER_COMPLETE
     * - NIMBLE_SCANNER_INCOMPLETE
     * - NIMBLE_SCANNER_TRUNCATED
     */
    uint8_t status;
    uint8_t phy_pri;        /**< PHY used on primary advertisement channels */
    uint8_t phy_sec;        /**< PHY used on secondary advertisement channels */
    int8_t rssi;            /**< RSSI value of received advertisement */
} nimble_scanner_info_t;

/**
 * @brief   Callback signature triggered by this module for each discovered
 *          advertising packet
 *
 * @param[in] type      type of advertising packet.
 *                      For legacy advertisements on of the following:
 *                          - BLE_HCI_ADV_RPT_EVTYPE_ADV_IND
 *                          - BLE_HCI_ADV_RPT_EVTYPE_DIR_IND
 *                          - BLE_HCI_ADV_RPT_EVTYPE_SCAN_IND
 *                          - BLE_HCI_ADV_RPT_EVTYPE_NONCONN_IND
 *                          - BLE_HCI_ADV_RPT_EVTYPE_SCAN_RSP
 *                      For extended advertisements NIMBLE_SCANNER_EXT_ADV ORed
 *                      with any set out of the following:
 *                          - BLE_HCI_ADV_CONN_MASK
 *                          - BLE_HCI_ADV_SCAN_MASK
 *                          - BLE_HCI_ADV_DIRECT_MASK
 *                          - BLE_HCI_ADV_SCAN_RSP_MASK
 * @param[in] addr      advertising address of the source node
 * @param[in] info      additional information about the advertiser
 * @param[in] ad        advertising data
 * @param[in] ad_len    length of @p ad in bytes
 */
typedef void(*nimble_scanner_cb)(uint8_t type, const ble_addr_t *addr,
                                 const nimble_scanner_info_t *info,
                                 const uint8_t *ad, size_t ad_len);

/**
 * @brief   Initialize the scanner module
 *
 * @param[in] params    scan parameters to use
 * @param[in] disc_cb   callback triggered of each received advertising packet
 *
 * @return  0 on success
 */
int nimble_scanner_init(const nimble_scanner_cfg_t *params,
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
 * @param[in]  duration_ms  duration of scanning procedure in ms, set to
 *                          BLE_HS_FOREVER to scan without time limit
 */
void nimble_scanner_set_scan_duration(int32_t duration_ms);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NIMBLE_SCANNER_H */

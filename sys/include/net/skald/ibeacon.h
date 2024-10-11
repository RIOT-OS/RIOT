/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    ble_skald_ibeacon Skald about iBeacon
 * @ingroup     ble_skald
 * @brief       Skald's simple iBeacon abstraction
 *
 * # About
 * This Skald module supports the creation and advertisement of BLE iBeacons as
 * defined by Apple (see https://developer.apple.com/ibeacon/).
 *
 * # Implementation state
 * - all known iBeacon properties are supported
 *
 * @{
 * @file
 * @brief       Skald's basic interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_SKALD_IBEACON_H
#define NET_SKALD_IBEACON_H

#include "net/skald.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Configure the IBeacon payload and start advertising
 *
 * @param[out] ctx      advertising context
 * @param[in] uuid      UUID advertised by the iBeacon
 * @param[in] major     the iBeacon's major number
 * @param[in] minor     the iBeacon's minor number
 * @param[in] txpower   calibrated TX power to be advertised by the beacon
 * @param[in] adv_itvl_ms   advertising interval [ms]
 */
void skald_ibeacon_advertise(skald_ctx_t *ctx, const skald_uuid_t *uuid,
                             uint16_t major, uint16_t minor, uint8_t txpower,
                             uint32_t adv_itvl_ms);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_SKALD_IBEACON_H */

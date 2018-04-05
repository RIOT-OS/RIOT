/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_skald_eddystone Skald meets Eddy
 * @ingroup     net_skald
 * @brief       Skald's Eddystone implementation
 *
 * # About
 * This module allows for creation and advertisement of Eddystone beacons (see
 * https://github.com/google/eddystone).
 *
 *
 * # Implementation state
 * supported:
 * - Eddystone-UID
 * - Eddystone-URL
 *
 * not (yet) supported:
 * - Eddystone-TLM
 * - Eddystone-EID
 *
 * @{
 * @file
 * @brief       Skald's basic interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_SKALD_EDDYSTONE_H
#define NET_SKALD_EDDYSTONE_H

#include "net/eddystone.h"
#include "net/skald.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Unique and opaque 16-byte beacon id format used by Eddystone
 */
typedef struct __attribute__((packed)) {
    uint8_t namespace[EDDYSTONE_NAMESPACE_LEN]; /**< 10-byte namespace */
    uint8_t instance[EDDYSTONE_INSTANCE_LEN];   /**< 6-byte instance */
} skald_eddystone_uid_t;

/**
 * @brief   Advertise Eddystone-UID data
 *
 * @see https://github.com/google/eddystone/tree/master/eddystone-uid
 *
 * @param[out] ctx      advertising context
 * @param[in] uid       UID to advertise
 * @param[in] tx_pwr    calibrated TX power to be advertised by the beacon
 */
void skald_eddystone_uid_adv(skald_ctx_t *ctx,
                             const skald_eddystone_uid_t *uid, uint8_t tx_pwr);

/**
 * @brief   Advertise Eddystone-URL data
 *
 * @see https://github.com/google/eddystone/tree/master/eddystone-url
 *
 * @param[out] ctx      advertising context
 * @param[in] scheme    encoded URL scheme prefix
 * @param[in] url       (short) url as \0 terminated string
 * @param[in] tx_pwr    calibrated TX power to be advertised by the beacon
 */
void skald_eddystone_url_adv(skald_ctx_t *ctx,
                             uint8_t scheme, const char *url, uint8_t tx_pwr);

#ifdef __cplusplus
}
#endif

#endif /* NET_SKALD_EDDYSTONE_H */
/** @} */

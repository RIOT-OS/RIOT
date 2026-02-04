/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_nimble_autoadv
 *
 * @{
 * @file
 * @brief       Default configuration for the nimble_autoadv module
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "nimble_autoadv.h"
#include "nimble_riot.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default parameters used for the nimble_autoadv module
 * @{
 */
#ifndef NIMBLE_AUTOADV_ADV_ITVL_MS
#define NIMBLE_AUTOADV_ADV_ITVL_MS         (100U) /* 100ms */
#endif
#ifndef NIMBLE_AUTOADV_ADV_DURATION_MS
#define NIMBLE_AUTOADV_ADV_DURATION_MS     (BLE_HS_FOREVER) /* forever */
#endif
#ifndef NIMBLE_AUTOADV_FLAGS
#define NIMBLE_AUTOADV_FLAGS               (NIMBLE_AUTOADV_FLAG_LEGACY | \
                                            NIMBLE_AUTOADV_FLAG_CONNECTABLE | \
                                            NIMBLE_AUTOADV_FLAG_SCANNABLE)
#endif
#ifndef NIMBLE_AUTOADV_PHY
#define NIMBLE_AUTOADV_PHY                 NIMBLE_PHY_1M
#endif
#ifndef NIMBLE_AUTOADV_TX_POWER
#define NIMBLE_AUTOADV_TX_POWER            0 /* 0dBm */
#endif
#ifndef NIMBLE_AUTOADV_CHANNEL_MAP
#define NIMBLE_AUTOADV_CHANNEL_MAP         0
#endif
#ifndef NIMBLE_AUTOADV_OWN_ADDR_TYPE
#define NIMBLE_AUTOADV_OWN_ADDR_TYPE       0xFF /* sets to nimble_riot_own_addr */
#endif
#ifndef NIMBLE_AUTOADV_FILTER_POLICY
#define NIMBLE_AUTOADV_FILTER_POLICY       0
#endif

#ifndef NIMBLE_AUTOADV_PARAMS
#define NIMBLE_AUTOADV_PARAMS                               \
    { .adv_itvl_ms     = NIMBLE_AUTOADV_ADV_ITVL_MS,        \
      .adv_duration_ms = NIMBLE_AUTOADV_ADV_DURATION_MS,    \
      .flags           = NIMBLE_AUTOADV_FLAGS,              \
      .phy             = NIMBLE_AUTOADV_PHY,                \
      .tx_power        = NIMBLE_AUTOADV_TX_POWER,           \
      .channel_map     = NIMBLE_AUTOADV_CHANNEL_MAP,        \
      .own_addr_type   = NIMBLE_AUTOADV_OWN_ADDR_TYPE,      \
      .filter_policy   = NIMBLE_AUTOADV_FILTER_POLICY }
#endif
/**@}*/

/**
 * @brief   nimble_autoadv configuration
 */
static const nimble_autoadv_cfg_t nimble_autoadv_cfg =
    NIMBLE_AUTOADV_PARAMS;

#ifdef __cplusplus
}
#endif

/** @} */

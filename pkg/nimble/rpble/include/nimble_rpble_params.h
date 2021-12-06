/*
 * Copyright (C) 2019-2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_rpble
 *
 * @{
 * @file
 * @brief       Default configuration for the nimble_netif_rpble module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NIMBLE_RPBLE_PARAMS_H
#define NIMBLE_RPBLE_PARAMS_H

#include "nimble_rpble.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default parameters used for the nimble_netif_rpble module
 * @{
 */
#ifndef NIMBLE_RPBLE_SCAN_ITVL_MS
#define NIMBLE_RPBLE_SCAN_ITVL_MS       1200U
#endif
#ifndef NIMBLE_RPBLE_SCAN_WIN_MS
#define NIMBLE_RPBLE_SCAN_WIN_MS        120U
#endif

#ifndef NIMBLE_RPBLE_ADV_ITVL_MS
#define NIMBLE_RPBLE_ADV_ITVL_MS        100U
#endif

#ifndef NIMBLE_RPBLE_CONN_SCAN_ITVL_MS
#define NIMBLE_RPBLE_CONN_SCAN_ITVL_MS  120U
#endif
#ifndef NIMBLE_RPBLE_CONN_SCAN_WIN_MS
#define NIMBLE_RPBLE_CONN_SCAN_WIN_MS   120U
#endif
#ifndef NIMBLE_RPBLE_CONN_SCAN_TO_MS
#define NIMBLE_RPBLE_CONN_SCAN_TO_MS    360U
#endif
#ifndef NIMBLE_RPBLE_CONN_ITVL_MIN_MS
#define NIMBLE_RPBLE_CONN_ITVL_MIN_MS   90U
#endif
#ifndef NIMBLE_RPBLE_CONN_ITVL_MAX_MS
#define NIMBLE_RPBLE_CONN_ITVL_MAX_MS   110U
#endif
#ifndef NIMBLE_RPBLE_CONN_LATENCY
#define NIMBLE_RPBLE_CONN_LATENCY       0
#endif
#ifndef NIMBLE_RPBLE_CONN_SUPER_TO_MS
#define NIMBLE_RPBLE_CONN_SUPER_TO_MS   1650U
#endif

#ifndef NIMBLE_RPBLE_EVAL_ITVL_MIN_MS
#define NIMBLE_RPBLE_EVAL_ITVL_MIN_MS   12000U
#endif
#ifndef NIMBLE_RPBLE_EVAL_ITVL_MAX_MS
#define NIMBLE_RPBLE_EVAL_ITVL_MAX_MS   13000U
#endif

/**
 * @brief   Default BLE PHY mode used by rpble
 */
#ifndef NIMBLE_RPBLE_PHY_MODE
#define NIMBLE_RPBLE_PHY_MODE           NIMBLE_PHY_1M
#endif

#ifndef NIMBLE_RPBLE_PARAMS
#define NIMBLE_RPBLE_PARAMS                                   \
    { .scan_itvl_ms         = NIMBLE_RPBLE_SCAN_ITVL_MS,      \
      .scan_win_ms          = NIMBLE_RPBLE_SCAN_WIN_MS,       \
      .adv_itvl_ms          = NIMBLE_RPBLE_ADV_ITVL_MS,       \
      .conn_scan_itvl_ms    = NIMBLE_RPBLE_CONN_SCAN_ITVL_MS, \
      .conn_scan_win_ms     = NIMBLE_RPBLE_CONN_SCAN_WIN_MS,  \
      .conn_scan_to_ms      = NIMBLE_RPBLE_CONN_SCAN_TO_MS,   \
      .conn_itvl_min_ms     = NIMBLE_RPBLE_CONN_ITVL_MIN_MS,  \
      .conn_itvl_max_ms     = NIMBLE_RPBLE_CONN_ITVL_MAX_MS,  \
      .conn_latency         = NIMBLE_RPBLE_CONN_LATENCY,      \
      .conn_super_to_ms     = NIMBLE_RPBLE_CONN_SUPER_TO_MS,  \
      .phy_mode             = NIMBLE_RPBLE_PHY_MODE,          \
      .eval_itvl_min_ms     = NIMBLE_RPBLE_EVAL_ITVL_MIN_MS,  \
      .eval_itvl_max_ms     = NIMBLE_RPBLE_EVAL_ITVL_MAX_MS }
#endif
/**@}*/

/**
 * @brief   nimble_netif_rpble configuration
 */
static const nimble_rpble_cfg_t nimble_rpble_params = NIMBLE_RPBLE_PARAMS;

#ifdef __cplusplus
}
#endif

#endif /* NIMBLE_RPBLE_PARAMS_H */
/** @} */

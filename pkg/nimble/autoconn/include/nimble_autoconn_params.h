/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_autoconn
 *
 * @{
 * @file
 * @brief       Default configuration for the nimble_autoconn module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NIMBLE_AUTOCONN_PARAMS_H
#define NIMBLE_AUTOCONN_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default parameters used for the nimble_netif_autoconn module
 * @{
 */
#ifndef NIMBLE_AUTOCONN_PERIOD_SCAN_MS
#define NIMBLE_AUTOCONN_PERIOD_SCAN_MS      (2000U)         /* 2s */
#endif
#ifndef NIMBLE_AUTOCONN_PERIOD_ADV_MS
#define NIMBLE_AUTOCONN_PERIOD_ADV_MS       (10000U)        /* 10s */
#endif
#ifndef NIMBLE_AUTOCONN_PERIOD_JITTER_MS
#define NIMBLE_AUTOCONN_PERIOD_JITTER_MS    (5000U)         /* 0-5s */
#endif

#ifndef NIMBLE_AUTOCONN_ADV_ITVL_MS
#define NIMBLE_AUTOCONN_ADV_ITVL_MS         (100U)          /* 100ms */
#endif

#ifndef NIMBLE_AUTOCONN_SCAN_ITVL_MS
#define NIMBLE_AUTOCONN_SCAN_ITVL_MS        (1500U)         /* 1.5s */
#endif
#ifndef NIMBLE_AUTOCONN_SCAN_WIN_MS
#define NIMBLE_AUTOCONN_SCAN_WIN_MS         (110U)          /* 110ms */
#endif

#ifndef NIMBLE_AUTOCONN_CONN_TIMEOUT_MS
#define NIMBLE_AUTOCONN_CONN_TIMEOUT_MS     (3 * NIMBLE_AUTOCONN_SCAN_WIN_MS)
#endif
#ifndef NIMBLE_AUTOCONN_CONN_ITVL_MIN_MS
#define NIMBLE_AUTOCONN_CONN_ITVL_MIN_MS    75U             /* 75ms */
#endif
#ifndef NIMBLE_AUTOCONN_CONN_ITVL_MAX_MS
#define NIMBLE_AUTOCONN_CONN_ITVL_MAX_MS    75U             /* 75ms */
#endif
#ifndef NIMBLE_AUTOCONN_CONN_LATENCY
#define NIMBLE_AUTOCONN_CONN_LATENCY        (0)
#endif
#ifndef NIMBLE_AUTOCONN_CONN_SVTO_MS
#define NIMBLE_AUTOCONN_CONN_SVTO_MS        (2500U)         /* 2.5s */
#endif

/**
 * @brief   Default BLE PHY mode used by autoconn
 */
#ifndef NIMBLE_AUTOCONN_PHY_MODE
#define NIMBLE_AUTOCONN_PHY_MODE            NIMBLE_PHY_1M
#endif

#ifndef NIMBLE_AUTOCONN_NODE_ID
#define NIMBLE_AUTOCONN_NODE_ID             "RIOT-autoconn"
#endif

#ifndef NIMBLE_AUTOCONN_PARAMS
#define NIMBLE_AUTOCONN_PARAMS                           \
    { .period_scan_ms   = NIMBLE_AUTOCONN_PERIOD_SCAN_MS,   \
      .period_adv_ms    = NIMBLE_AUTOCONN_PERIOD_ADV_MS,    \
      .period_jitter_ms = NIMBLE_AUTOCONN_PERIOD_JITTER_MS, \
      .adv_itvl_ms      = NIMBLE_AUTOCONN_ADV_ITVL_MS,      \
      .scan_itvl_ms     = NIMBLE_AUTOCONN_SCAN_ITVL_MS,     \
      .scan_win_ms      = NIMBLE_AUTOCONN_SCAN_WIN_MS,      \
      .conn_timeout_ms  = NIMBLE_AUTOCONN_CONN_TIMEOUT_MS,  \
      .conn_itvl_min_ms = NIMBLE_AUTOCONN_CONN_ITVL_MIN_MS, \
      .conn_itvl_max_ms = NIMBLE_AUTOCONN_CONN_ITVL_MAX_MS, \
      .conn_latency     = NIMBLE_AUTOCONN_CONN_LATENCY,     \
      .conn_super_to_ms = NIMBLE_AUTOCONN_CONN_SVTO_MS,     \
      .phy_mode         = NIMBLE_AUTOCONN_PHY_MODE,         \
      .node_id          = NIMBLE_AUTOCONN_NODE_ID, }
#endif
/**@}*/

/**
 * @brief   nimble_netif_autoconn configuration
 */
static const nimble_autoconn_params_t nimble_autoconn_params =
    NIMBLE_AUTOCONN_PARAMS;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NIMBLE_AUTOCONN_PARAMS_H */

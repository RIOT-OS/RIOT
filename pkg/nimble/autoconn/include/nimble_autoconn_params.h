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
#ifndef NIMBLE_AUTOCONN_PERIOD_SCAN
#define NIMBLE_AUTOCONN_PERIOD_SCAN         (2000U)         /* 2s */
#endif
#ifndef NIMBLE_AUTOCONN_PERIOD_ADV
#define NIMBLE_AUTOCONN_PERIOD_ADV          (10000U)        /* 10s */
#endif
#ifndef NIMBLE_AUTOCONN_PERIOD_JITTER
#define NIMBLE_AUTOCONN_PERIOD_JITTER       (5000U)         /* 0-5s */
#endif

#ifndef NIMBLE_AUTOCONN_ADV_ITVL
#define NIMBLE_AUTOCONN_ADV_ITVL            (100U)          /* 100ms */
#endif

#ifndef NIMBLE_AUTOCONN_SCAN_ITVL
#define NIMBLE_AUTOCONN_SCAN_ITVL           (1500U)         /* 1.5s */
#endif
#ifndef NIMBLE_AUTOCONN_SCAN_WIN
#define NIMBLE_AUTOCONN_SCAN_WIN            (110U)          /* 110ms */
#endif

#ifndef NIMBLE_AUTOCONN_CONN_ITVL
#define NIMBLE_AUTOCONN_CONN_ITVL           (75U)           /* 75ms */
#endif
#ifndef NIMBLE_AUTOCONN_CONN_LATENCY
#define NIMBLE_AUTOCONN_CONN_LATENCY        (0)
#endif
#ifndef NIMBLE_AUTOCONN_CONN_SUPER_TO
#define NIMBLE_AUTOCONN_CONN_SUPER_TO       (2500U)         /* 2.5s */
#endif

#ifndef NIMBLE_AUTOCONN_NODE_ID
#define NIMBLE_AUTOCONN_NODE_ID             "RIOT-autoconn"
#endif

#ifndef NIMBLE_AUTOCONN_PARAMS
#define NIMBLE_AUTOCONN_PARAMS                        \
    { .period_scan   = NIMBLE_AUTOCONN_PERIOD_SCAN,   \
      .period_adv    = NIMBLE_AUTOCONN_PERIOD_ADV,    \
      .period_jitter = NIMBLE_AUTOCONN_PERIOD_JITTER, \
      .adv_itvl      = NIMBLE_AUTOCONN_ADV_ITVL,      \
      .scan_itvl     = NIMBLE_AUTOCONN_SCAN_ITVL,     \
      .scan_win      = NIMBLE_AUTOCONN_SCAN_WIN,      \
      .conn_itvl     = NIMBLE_AUTOCONN_CONN_ITVL,     \
      .conn_latency  = NIMBLE_AUTOCONN_CONN_LATENCY,  \
      .conn_super_to = NIMBLE_AUTOCONN_CONN_SUPER_TO, \
      .node_id       = NIMBLE_AUTOCONN_NODE_ID, }
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

#endif /* NIMBLE_AUTOCONN_PARAMS_H */
/** @} */

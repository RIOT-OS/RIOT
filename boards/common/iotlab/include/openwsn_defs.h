/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common
 * @{
 *
 * @file
 * @brief       Board specific definitions for pkg/openwsn
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    OpenWSN timing constants
 *
 * @{
 */
/* Measured 440us + ~10% */
#define PORT_maxTxDataPrepare   (500/PORT_US_PER_TICK)
/* Measured 200us + ~10% */
#define PORT_maxRxAckPrepare    (300/PORT_US_PER_TICK)
/* Measured 300us + ~10% */
#define PORT_maxRxDataPrepare   (350/PORT_US_PER_TICK)
/* Measured 316us + ~10% */
#define PORT_maxTxAckPrepare    (350/PORT_US_PER_TICK)
/* Measured 360us with openwsn_sctimer_rtt */
#define PORT_delayTx            (360/PORT_US_PER_TICK)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

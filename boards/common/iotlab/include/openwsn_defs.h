/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common
 * @{
 *
 * @file
 * @brief       Board specific definitions for pkg/openwsn
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef OPENWSN_DEFS_H
#define OPENWSN_DEFS_H

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

#endif /* OPENWSN_DEFS_H */
/** @} */

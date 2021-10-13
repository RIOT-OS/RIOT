/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samr21-xpro
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
/* Measured 800us + ~10% */
#define PORT_maxTxDataPrepare   (900/PORT_US_PER_TICK)
/* Measured 450us + ~10% */
#define PORT_maxRxAckPrepare    (500/PORT_US_PER_TICK)
/* Measured 500us + ~10% */
#define PORT_maxRxDataPrepare   (550/PORT_US_PER_TICK)
/* Measured 660us + ~10% */
#define PORT_maxTxAckPrepare    (750/PORT_US_PER_TICK)
/* Measured 650us with openwsn_sctimer_rtt */
#define PORT_delayTx            (650/PORT_US_PER_TICK)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* OPENWSN_DEFS_H */
/** @} */

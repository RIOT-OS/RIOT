/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu
 * @{
 *
 * @file
 * @brief       cc2538 specific definitions for pkg/openwsn
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
/* Taken from openwsn-fw */
#define PORT_maxTxDataPrepare   (460/PORT_US_PER_TICK)
#define PORT_maxRxAckPrepare    (300/PORT_US_PER_TICK)
#define PORT_maxRxDataPrepare   (300/PORT_US_PER_TICK)
#define PORT_maxTxAckPrepare    (460/PORT_US_PER_TICK)
#define PORT_delayTx            (400/PORT_US_PER_TICK)
/** @} */
#ifdef __cplusplus
}
#endif

/** @} */
#endif /* OPENWSN_DEFS_H */

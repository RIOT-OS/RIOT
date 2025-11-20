/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu
 * @{
 *
 * @file
 * @brief       cc2538 specific definitions for pkg/openwsn
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

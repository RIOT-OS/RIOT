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
 * @brief       nrf52 specific definitions for pkg/openwsn
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CPU_MODEL_NRF52840XXAA
/**
 * @name    OpenWSN timing constants
 *
 * @{
 */
/* Taken from OpenWSN @32.768Hz */
#define PORT_maxTxDataPrepare   (400/PORT_US_PER_TICK)
#define PORT_maxRxAckPrepare    (400/PORT_US_PER_TICK)
#define PORT_maxRxDataPrepare   (400/PORT_US_PER_TICK)
#define PORT_maxTxAckPrepare    (400/PORT_US_PER_TICK)
/* Measured 220us */
#define PORT_delayTx            (300/PORT_US_PER_TICK)
#define PORT_delayRx            (150/PORT_US_PER_TICK)
/** @} */
#endif

#ifdef __cplusplus
}
#endif

/** @} */

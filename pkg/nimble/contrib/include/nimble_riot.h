/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    pkg_nimble_contrib RIOT Integration
 * @ingroup     pkg_nimble
 * @brief       Basic RIOT integration of NimBLE, including e.g. stack
 *              configuration and (auto)initialization code
 * @{
 *
 * @file
 * @brief       RIOT specific glue functions for integrating NimBLE
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdbool.h>
#include <stdint.h>
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Priority used for NimBLE's controller thread
 *
 * This should be as high as possible.
 */
#ifndef NIMBLE_CONTROLLER_PRIO
#define NIMBLE_CONTROLLER_PRIO      (0)
#endif

/**
 * @brief   Stacksize used for NimBLE's controller thread
 */
#ifndef NIMBLE_CONTROLLER_STACKSIZE
#define NIMBLE_CONTROLLER_STACKSIZE (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Priority used for NimBLE's host thread
 */
#ifndef NIMBLE_HOST_PRIO
#if IS_USED(MODULE_NIMBLE_NETIF)
/* when using IP, the host should have a higher prio than the netif thread, but
 * MUST always have a lower priority then the controller thread. Setting it to
 * controller prio plus will guarantee the latter while also matching the first
 * condition as long as the default netif prio is not changed. */
#define NIMBLE_HOST_PRIO            (NIMBLE_CONTROLLER_PRIO + 1)
#else
#define NIMBLE_HOST_PRIO            (THREAD_PRIORITY_MAIN - 2)
#endif
#endif

/**
 * @brief   Stacksize used for NimBLE's host thread
 */
#ifndef NIMBLE_HOST_STACKSIZE
#define NIMBLE_HOST_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   BLE PHY modes
 */
typedef enum {
    NIMBLE_PHY_INVALID,         /**< PHY mode invalid */
    NIMBLE_PHY_1M,              /**< legacy 1Mbit PHY mode (always supported) */
#if IS_USED(MODULE_NIMBLE_PHY_2MBIT)
    NIMBLE_PHY_2M,              /**< 2Mbit PHY mode */
#endif
#if IS_USED(MODULE_NIMBLE_PHY_CODED)
    NIMBLE_PHY_CODED,           /**< Coded (long range) PHY mode */
#endif
} nimble_phy_t;

/**
 * @brief   Export our own address type for later usage
 */
extern uint8_t nimble_riot_own_addr_type;

/**
 * @brief   Indicates whether Setup NimBLE's controller has been initialized
 *
 * `nimble_port_initialized` is false by default and is set to true as soon as
 * `nimble_port_init` has been called by nimble_riot_init, i.e. that the NimBLE
 * stack has been initialized.
 *
 * The variable can be used to decide whether events from the low-level
 * BLE controller driver should be forwarded to the NimBLE stack. It is
 * necessary to avoid crashes in the case that the higher-prioritized thread
 * of the low-level BLE controller driver starts sending events to the host
 * before the NimBLE stack has been initialized by the lower-prioritized
 * host thread. */
extern bool nimble_port_initialized;

/**
 * @brief   Setup and run NimBLE's controller and host threads
 */
void nimble_riot_init(void);

/**
 * @brief   Converts BLE PHY mode to BLE HCI PHY
 *
 * @param[in]   mode    ble phy mode to convert
 */
#if MYNEWT_VAL_BLE_EXT_ADV
int nimble_riot_get_phy_hci(uint8_t mode);
#endif

#ifdef __cplusplus
}
#endif

/** @} */

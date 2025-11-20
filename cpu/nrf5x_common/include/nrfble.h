/*
 * SPDX-FileCopyrightText: 2017-2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_nrf5x_nrfble NRF BLE radio driver
 * @ingroup     drivers_netdev
 * @brief       Radio driver for nRF5x SoCs for using the radio in BLE mode
 * @{
 *
 * @file
 * @brief       Interface definition for the nrfble radio driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   nrfble channel configuration
 * @{
 */
#define NRFBLE_CHAN_MIN             (0U)
#define NRFBLE_CHAN_MAX             (39U)
/** @} */

/**
 * @brief   Default transmission power used
 */
#define NRFBLE_TXPOWER_DEFAULT      (0)                 /* 0dBm */

/**
 * @brief   Setup the device driver's data structures
 *
 * @return  pointer to the device's netdev struct
 */
netdev_t *nrfble_setup(void);

#ifdef __cplusplus
}
#endif

/** @} */

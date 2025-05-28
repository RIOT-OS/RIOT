/*
 * SPDX-FileCopyrightText: 2022 Inria
 * SPDX-FileCopyrightText: 2020-2022 Université Grenoble Alpes
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_sx1280
 * @{
 * @file
 * @brief       Netdev driver definitions for LoRa SX1280 Driver driver
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Aymeric Brochier <aymeric.brochier@univ-grenoble-alpes.fr>
 *
 */

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t sx1280_driver;

#ifdef __cplusplus
}
#endif

/** @} */

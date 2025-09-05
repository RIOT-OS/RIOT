/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_sx126x
 * @{
 * @file
 * @brief       Netdev driver definitions for SX1261/2/8 and LLCC68 driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t sx126x_driver;

#ifdef __cplusplus
}
#endif

/** @} */

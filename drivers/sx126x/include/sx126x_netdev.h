/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx126x
 * @{
 * @file
 * @brief       Netdev driver definitions for SX1261/2/8 and LLCC68 driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef SX126X_NETDEV_H
#define SX126X_NETDEV_H

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
#endif /* SX126X_NETDEV_H */

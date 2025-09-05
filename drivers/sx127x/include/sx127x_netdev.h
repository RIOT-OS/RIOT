/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-FileCopyrightText: 2017 Inria Chile
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_sx127x
 * @{
 * @file
 * @brief       Netdev driver definitions for SX127X driver
 *
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t sx127x_driver;

#ifdef __cplusplus
}
#endif

/** @} */

/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup drivers_netdev
 * @{
 */

/**
 * @file
 * @brief  Default configuration for the netdev_tap driver
 * @author Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NETDEV_TAP_PARAMS_H
#define NETDEV_TAP_PARAMS_H

#include "netdev_tap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of allocated parameters at @ref netdev_tap_params
 *
 * @note    This was decided to only be configurable on compile-time to be
 *          more similar to actual boards
 */
#ifndef NETDEV_TAP_MAX
#define NETDEV_TAP_MAX              (1)
#endif

/**
 * @brief   Configuration parameters for @ref netdev_tap_t
 *
 * @note    This variable is set on native start-up based on arguments provided
 */
extern netdev_tap_params_t netdev_tap_params[NETDEV_TAP_MAX];

#ifdef __cplusplus
}
#endif

#endif /* NETDEV_TAP_PARAMS_H */
/** @} */

/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Netdev interface to AT86RF2xx drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t at86rf2xx_driver;

#ifdef __cplusplus
}
#endif

/** @} */

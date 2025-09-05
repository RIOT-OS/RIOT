/*
 * SPDX-FileCopyrightText: 2017 Neo Nenaco <neo@nenaco.de>
 * SPDX-FileCopyrightText: 2017 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_mrf24j40
 * @{
 *
 * @file
 * @brief       Netdev interface to MRF24J40 drivers
 *
 * @author      Neo Nenaco <neo@nenaco.de>
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t mrf24j40_driver;

#ifdef __cplusplus
}
#endif

/** @} */

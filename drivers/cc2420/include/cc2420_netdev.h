/*
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup    drivers_cc2420
 * @{
 *
 * @file
 * @brief      Netdev interface for the CC2420
 *
 * @author     Francisco Acosta <francisco.acosta@inria.fr>
 * @author     Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t cc2420_driver;

#ifdef __cplusplus
}
#endif

/** @} */

/*
 * Copyright (C) 2018 Hochschule RheinMain
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_rail
 * @{
 *
 * @file
 * @brief       Netdev interface to Silabs EFR32 radio drivers
 *
 * @author      Kai Beckmann <kai.beckmann@hs-rm.de>
 */

#ifndef RAIL_NETDEV_H
#define RAIL_NETDEV_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t rail_driver;

#ifdef __cplusplus
}
#endif

#endif /* RAIL_NETDEV_H */
/** @} */

/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file        cc110x-netdev.h
 * @brief       Variables for the cc110x netdev base interface
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef CC110X_NETDEV_H
#define CC110X_NETDEV_H

#include "netdev/base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Netdev event types for the CC110x
 * @{
 */
#define CC110X_NETDEV_EVENT_RX  (11001)
/** @} */

/**
 * @brief   Implementation of netdev_driver_t for CC110X device
 */
extern const netdev_driver_t cc110x_net_driver;

/**
 * @brief   CC110X default device
 */
extern netdev_t cc110x_dev;

#ifdef __cplusplus
}
#endif

#endif /* CC110X_NETDEV_H */

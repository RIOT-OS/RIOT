/*
 * Copyright (C) 2016 MUTEX NZ Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file
 * @brief       Netdev interface to CC2538 radio driver
 *
 * @author      Aaron Sowry <aaron@mutex.nz>
 */

#ifndef CC2538_RF_NETDEV_H
#define CC2538_RF_NETDEV_H

#include "net/netdev2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev2_driver_t cc2538_rf_driver;

#ifdef __cplusplus
}
#endif

#endif /* CC2538_RF_NETDEV_H */
/** @} */

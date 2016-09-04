/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mrf24j40
 * @{
 *
 * @file
 * @brief       Netdev interface to MRF24J40 drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef MRF24J40_NETDEV_H_
#define MRF24J40_NETDEV_H_

#include "net/netdev2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev2_driver_t mrf24j40_driver;


#ifdef __cplusplus
}
#endif

#endif /* MRF24J40_NETDEV_H_ */
/** @} */

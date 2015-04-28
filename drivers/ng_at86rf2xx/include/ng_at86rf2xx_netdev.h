/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ng_at86rf2xx
 * @{
 *
 * @file
 * @brief       Netdev interface to AT86RF2xx drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NG_AT86RF2XX_NETDEV_H_
#define NG_AT86RF2XX_NETDEV_H_

#include "net/ng_netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const ng_netdev_driver_t ng_at86rf2xx_driver;

#ifdef __cplusplus
}
#endif

#endif /* NG_AT86RF2XX_NETDEV_H_ */
/** @} */

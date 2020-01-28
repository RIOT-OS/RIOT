/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Netdev interface to AT86RF2xx drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef AT86RF2XX_NETDEV_H
#define AT86RF2XX_NETDEV_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t at86rf2xx_driver;

/**
 * @brief   Reference to the netdev_ieee802154 ops struct
 */
extern const netdev_ieee802154_ops_t at86rf2xx_ieee802154_ops;

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_NETDEV_H */
/** @} */

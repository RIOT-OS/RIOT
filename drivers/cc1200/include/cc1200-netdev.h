/*
 * Copyright (C) 2017 TU Berlin DAI Labor
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc1200
 * @{
 *
 * @file
 * @brief       Variables for the cc1200 netdev interface
 *
 * @author      Anon Mall <anon.mall@gt-arc.com>
 */

#ifndef CC1200_NETDEV_H
#define CC1200_NETDEV_H

#include "periph/gpio.h"
#include "periph/spi.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "cc1200.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Implementation of netdev_driver_t for CC1200 device
 */
extern const netdev_driver_t netdev_cc1200_driver;

/**
 * @brief cc1200 netdev struct
 */
typedef struct netdev_cc1200 {
    netdev_ieee802154_t netdev;   /**< netdev parent struct */
    cc1200_t cc1200;        /**< documentation here */
} netdev_cc1200_t;

/**
 * @brief   Received packet status information for cc1200 radios
 */
typedef struct netdev_radio_rx_info netdev_cc1200_rx_info_t;

/**
 * @brief netdev <-> cc1200 glue code initialization function
 *
 * @param[out]      netdev_cc1200  ptr to netdev_cc1200 struct ti initialize
 * @param[in]       params          cc1200 IO parameter struct to use
 *
 * @return          0               on success
 * @return          -1              on error
 */
int netdev_cc1200_setup(netdev_cc1200_t *netdev_cc1200, const cc1200_params_t *params);

#ifdef __cplusplus
}
#endif

#endif /* CC1200_NETDEV_H */

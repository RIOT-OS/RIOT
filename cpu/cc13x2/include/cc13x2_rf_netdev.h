/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx_definitions
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx IEEE 802.15.4 netdev driver
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC13X2_RF_NETDEV_H
#define CC13X2_RF_NETDEV_H

#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

typedef struct {
    netdev_ieee802154_t netdev;             /**< netdev parent struct */
} cc13x2_rf_t;

void cc13x2_rf_setup(cc13x2_rf_t *dev);

#endif /* CC13X2_RF_NETDEV_H */
/** @} */

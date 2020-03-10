/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2, CC13x2 RF driver
 */

#ifndef CC26X2_CC13X2_RF_NETDEV_H
#define CC26X2_CC13X2_RF_NETDEV_H

#include "net/ieee802154.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device descriptor for CC26x2/CC13x2 transceivers
 *
 * @extends netdev_ieee802154_t
 */
typedef struct {
    netdev_ieee802154_t netdev; /**< netdev parent struct */
    void* rf_handle; /** RF Handle */
} cc26x2_cc13x2_rf_t;

void cc26x2_cc13x2_rf_setup(cc26x2_cc13x2_rf_t* dev);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X2_CC13X2_RF_NETDEV_H */

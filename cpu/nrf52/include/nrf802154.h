/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_nrf52_802154 IEEE802.15.4 Driver for nRF52840 SoCs
 * @ingroup     drivers_netdev
 * @brief       Driver for using the nRF52's radio in IEEE802.15.4 mode
 *
 * @todo        -> Implementation of Clear Channel Assessment Control (CCACTRL)
                -> Implementation of ACKnowledgement and packet retransmission
 *
 * @{
 *
 * @file
 * @brief       Driver interface for using the nRF52 in IEEE802.15.4 mode
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NRF802154_H
#define NRF802154_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Export the netdev device descriptor
 */
extern netdev_ieee802154_t nrf802154_dev;

#endif /* NRF802154_H */
/** @} */

/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * ## Implementation state ##
 * Netdev events supported:
 *
 * - NETDEV_EVENT_RX_COMPLETE
 * - NETDEV_EVENT_TX_COMPLETE
 *
 * Transmission options not yet implemented:
 * - Send acknowledgement for packages
 * - Request acknowledgement
 * - Retransmit unacked packages
 * - Carrier Sense Multiple Access (CSMA) and Implementation of Clear Channel
 *   Assessment Control (CCACTRL)
 *
 * @{
 *
 * @file
 * @brief       Driver interface for using the nRF52 in IEEE802.15.4 mode
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 */

#ifndef NRF802154_H
#define NRF802154_H

#include "net/netdev/ieee802154.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Export the netdev device descriptor
 */
extern netdev_ieee802154_t nrf802154_dev;

/**
 * @brief   IEEE 802.15.4 radio timer configuration
 *
 *          this radio relies on a dedicated hardware timer to maintain IFS
 *          the default timer may be overwritten in the board configuration
 */
#ifndef NRF802154_TIMER
#define NRF802154_TIMER TIMER_DEV(1)
#endif

#endif /* NRF802154_H */
/** @} */

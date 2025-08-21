/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @defgroup     drivers_netdev_ieee802154_submac IEEE802.15.4 SubMAC netdev layer
 * @ingroup      drivers_netdev_api
 * @experimental This API is experimental and in an early state - expect
 *               changes!

 * @brief        This module defines implements the netdev API on top of the
 * IEEE 802.15.4 radio HAL
 *
 * @{
 *
 * @author       José I. Alamos <jose.alamos@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "net/ieee802154/submac.h"
#include "net/ieee802154/radio.h"
#include "ztimer.h"

#include "od.h"
#include "event/thread.h"
#include "event/callback.h"

#define NETDEV_SUBMAC_FLAGS_ACK_TIMEOUT (1 << 0)    /**< Flag for ACK Timeout event */
#define NETDEV_SUBMAC_FLAGS_TX_DONE     (1 << 1)    /**< Flag for TX Done event */
#define NETDEV_SUBMAC_FLAGS_RX_DONE     (1 << 2)    /**< Flag for RX Done event */
#define NETDEV_SUBMAC_FLAGS_CRC_ERROR   (1 << 3)    /**< Flag for CRC ERROR event */
#define NETDEV_SUBMAC_FLAGS_BH_REQUEST  (1 << 4)    /**< Flag for Bottom Half request event */

/**
 * @brief IEEE 802.15.4 SubMAC netdev descriptor
 */
typedef struct {
    netdev_ieee802154_t dev;            /**< IEEE 802.15.4 netdev descriptor */
    ieee802154_submac_t submac;         /**< IEEE 802.15.4 SubMAC descriptor */
    ztimer_t ack_timer;                 /**< ztimer descriptor for the ACK timeout timer */
    int isr_flags;                      /**< netdev submac @ref NETDEV_EVENT_ISR flags */
    int bytes_tx;                       /**< size of the sent frame or tx error */
    int8_t retrans;                     /**< number of frame retransmissions of the last TX */
    bool dispatch;                      /**< whether an event should be dispatched or not */
    netdev_event_t ev;                  /**< event to be dispatched */
#if IS_USED(MODULE_NETDEV_IEEE802154_SUBMAC_LEGACY)
    netdev_t *legacy;                   /**< For drivers that also implement the old netdev API,
                                             this pointer shall be set to the netdev_t inside the driver struct,
                                             to have the old netdev driver's get and set functions,
                                             supporting device specific netopts. */
#endif
} netdev_ieee802154_submac_t;

/**
 * @brief Init the IEEE 802.15.4 SubMAC netdev adoption.
 *
 * @param[in] netdev_submac pointer to the netdev submac descriptor.
 * @param[in] legacy pointer to an old netdev, set up with the old netdev driver.
 *
 * @note @p legacy can be NULL. If not, it should have been set up in *_hal_setup().
 *
 * @return 0 on success.
 * @return negative errno on failure.
 */
int netdev_ieee802154_submac_init(netdev_ieee802154_submac_t *netdev_submac, netdev_t *legacy);

#ifdef __cplusplus
}
#endif

/** @} */

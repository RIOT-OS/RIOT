/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *                    Ell-i open source co-operative
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gnrc_netdev_eth dev_eth as netdev device.
 * @ingroup     net_gnrc_netdev
 *
 * @brief Allows for usage of @p dev_eth as ethernet netdev devices.
 *
 * @{
 *
 * @file
 * @brief Definitions for @ref gnrc_netdev_eth
 *
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef GNRC_NETDEV_ETH_H_
#define GNRC_NETDEV_ETH_H_

#include <inttypes.h>

#include "kernel_types.h"
#include "net/gnrc/netdev.h"
#include "net/ethernet/hdr.h"
#include "net/dev_eth.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Definition of the netdev_eth device
 * @extends gnrc_netdev_t
 * @internal
 */
typedef struct {
    gnrc_netdev_driver_t *driver;   /**< pointer to the devices interface */
    gnrc_netdev_event_cb_t event_cb;/**< netdev event callback */
    kernel_pid_t mac_pid;           /**< the driver's thread's PID */
    dev_eth_t *ethdev;              /**< ptr to low-level device handle */
} gnrc_netdev_eth_t;

/**
 * @brief Reference to the netdev_eth driver interface
 */
extern const gnrc_netdev_driver_t gnrc_netdev_eth_driver;

/**
 * @brief Reference to the netdev_eth device
 */
extern gnrc_netdev_eth_t gnrc_netdev_eth;
/* XXX: this is only here since I do not know how to get the device in the
 * interrupt handler */

/**
 * @brief Initialize a given netdev_eth device
 *
 * @param[out] netdev netdev_eth device to initialize
 * @param[in] ethdev handle dev_eth interface to be used
 *
 * Will initialize ethdev.
 *
 * @return 0 on success
 * @return -ENODEV on invalid device descriptor
 */
int gnrc_netdev_eth_init(gnrc_netdev_eth_t *netdev, dev_eth_t *ethdev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GNRC_NETDEV_ETH_H_ */

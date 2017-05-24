/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netdev
 * @{
 *
 * @file
 * @brief       GNRC to XBee netdev glue code interface
 *
 * Although the XBee devices are IEEE802.15.4 transceivers, use their own
 * proprietary format for communicating between the host CPU and the device over
 * UART. The XBee device driver expects the data to send to be given in this
 * format, hence we need to introduce an XBee specific adaption layer that
 * translates between GNRC and the XBee proprietary header format.
 *
 * For this custom header format, we can not make use of the existing adaption
 * layers for other IEEE802.15.4 devices.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_GNRC_NETDEV_XBEE_ADPT_H
#define NET_GNRC_NETDEV_XBEE_ADPT_H

#include "xbee.h"
#include "net/gnrc/netdev.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize GNRC handler for netdev XBee devices
 *
 * @param[out] gnrc_netdev     gnrc_netdev struct to initialize
 * @param[in]  dev              XBee device to handle
 */
void gnrc_netdev_xbee_init(gnrc_netdev_t *gnrc_netdev, xbee_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETDEV_XBEE_ADPT_H */
/** @} */

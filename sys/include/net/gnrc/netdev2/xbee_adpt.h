/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netdev2
 * @{
 *
 * @file
 * @brief       GNRC to XBee netdev2 glue code interface
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

#ifndef GNRC_NETDEV2_XBEE_ADPT_H
#define GNRC_NETDEV2_XBEE_ADPT_H

#include "xbee.h"
#include "net/gnrc/netdev2.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize GNRC handler for netdev2 XBee devices
 *
 * @param[out] gnrc_netdev2     gnrc_netdev2 struct to initialize
 * @param[in]  dev              XBee device to handle
 */
void gnrc_netdev2_xbee_init(gnrc_netdev2_t *gnrc_netdev2, xbee_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETDEV2_XBEE_ADPT_H */
/** @} */

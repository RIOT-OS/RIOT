/*
 * Copyright (C) 2019 Koen Zandberg
 *               2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for tinyUSB CDC ECM module
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @author  Gunar Schorcht <gunar@schorcht.net>
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include "include/init_devs.h"
#include "log.h"
#include "net/gnrc/netif/ethernet.h"
#include "tinyusb_netdev.h"

/**
 * @brief global cdc ecm object, declared in the usb auto init file
 */
extern tinyusb_netdev_t tinyusb_netdev;

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define TUSB_NETDEV_STACKSIZE   (GNRC_NETIF_STACKSIZE_DEFAULT)
#ifndef TUSB_NETDEV_PRIO
#define TUSB_NETDEV_PRIO        (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev_eth_stack[TUSB_NETDEV_STACKSIZE];
static gnrc_netif_t _netif;

extern void tinyusb_netdev_setup(tinyusb_netdev_t *dev);

void auto_init_tinyusb_netdev(void)
{
    LOG_DEBUG("[auto_init_netif] initializing tinyUSB netdev #0\n");

    tinyusb_netdev_setup(&tinyusb_netdev);
    /* initialize netdev<->gnrc adapter state */
    gnrc_netif_ethernet_create(&_netif, _netdev_eth_stack,
                               TUSB_NETDEV_STACKSIZE, TUSB_NETDEV_PRIO,
                               "tinyusb_netdev", &tinyusb_netdev.netdev);
}
/** @} */

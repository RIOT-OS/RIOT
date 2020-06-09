/*
 * Copyright (C) 2019 Koen Zandberg
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
 * @brief   Auto initialization for USB CDC ECM module
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include "log.h"
#include "usb/usbus/cdc/ecm.h"
#include "net/gnrc/netif/ethernet.h"

/**
 * @brief global cdc ecm object, declared in the usb auto init file
 */
extern usbus_cdcecm_device_t cdcecm;

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define CDCECM_MAC_STACKSIZE (THREAD_STACKSIZE_DEFAULT)
#ifndef CDCECM_MAC_PRIO
#define CDCECM_MAC_PRIO      (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev_eth_stack[CDCECM_MAC_STACKSIZE];
static gnrc_netif_t _netif;
extern void cdcecm_netdev_setup(usbus_cdcecm_device_t *cdcecm);

void auto_init_netdev_cdcecm(void)
{
    LOG_DEBUG("[auto_init_netif] initializing cdc ecm #0\n");

    cdcecm_netdev_setup(&cdcecm);
    /* initialize netdev<->gnrc adapter state */
    gnrc_netif_ethernet_create(&_netif, _netdev_eth_stack, CDCECM_MAC_STACKSIZE,
                               CDCECM_MAC_PRIO, "cdcecm", &cdcecm.netdev);
}
/** @} */

/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for W5100 Ethernet devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "log.h"
#include "w5100.h"
#include "w5100_params.h"
#include "net/gnrc/netif/ethernet.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define MAC_STACKSIZE   (THREAD_STACKSIZE_DEFAULT)
#define MAC_PRIO        (GNRC_NETIF_PRIO)
/*** @} */

/**
 * @brief   Find out how many of these devices we need to care for
 */
#define W5100_NUM    ARRAY_SIZE(w5100_params)

/**
 * @brief   Allocate memory for the device descriptors
 * @{
 */
static w5100_t dev[W5100_NUM];
/** @} */

static gnrc_netif_t _netif[W5100_NUM];

/**
 * @brief   Stacks for the MAC layer threads
 */
static char stack[W5100_NUM][MAC_STACKSIZE];


void auto_init_w5100(void)
{
    for (unsigned i = 0; i < W5100_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing w5100 #%u\n", i);

        /* setup netdev device */
        w5100_setup(&dev[i], &w5100_params[i]);
        /* initialize netdev <-> gnrc adapter state */
        gnrc_netif_ethernet_create(&_netif[i], stack[i], MAC_STACKSIZE, MAC_PRIO, "w5100",
                                   &dev[i].nd);
    }
}
/** @} */

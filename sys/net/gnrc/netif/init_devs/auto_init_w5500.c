/*
 * Copyright (C) 2023 Stefan Schmidt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for W5500 Ethernet devices
 *
 * @author      Stefan Schmidt <stemschmidt@gmail.com>
 */

#include "log.h"
#include "w5500.h"
#include "w5500_params.h"
#include "net/gnrc/netif/ethernet.h"
#include "include/init_devs.h"

/**
 * @brief   Find out how many of these devices we need to care for
 */
#define W5500_NUM    ARRAY_SIZE(w5500_params)

/**
 * @brief   Allocate memory for the device descriptors
 * @{
 */
static w5500_t dev[W5500_NUM];
/** @} */

static gnrc_netif_t _netif[W5500_NUM];

/**
 * @brief   Stacks for the MAC layer threads
 */
static char stack[W5500_NUM][GNRC_NETIF_STACKSIZE_DEFAULT];

void auto_init_w5500(void)
{
    for (unsigned i = 0; i < W5500_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing w5500 #%u\n", i);

        /* setup netdev device */
        w5500_setup(&dev[i], &w5500_params[i], i);
        /* initialize netdev <-> gnrc adapter state */
        gnrc_netif_ethernet_create(&_netif[i], stack[i], GNRC_NETIF_STACKSIZE_DEFAULT,
                                   GNRC_NETIF_PRIO, "w5500", &dev[i].netdev);
    }
}
/** @} */

/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for W5100 Ethernet devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef MODULE_W5100

#include "log.h"
#include "w5100.h"
#include "w5100_params.h"
#include "net/gnrc/netdev2.h"
#include "net/gnrc/netdev2/eth.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define MAC_STACKSIZE   (THREAD_STACKSIZE_DEFAULT)
#define MAC_PRIO        (THREAD_PRIORITY_MAIN - 4)
/*** @} */

/**
 * @brief   Find out how many of these devices we need to care for
 */
#define W5100_NUM    (sizeof(w5100_params) / sizeof(w5100_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 * @{
 */
static w5100_t dev[W5100_NUM];
static gnrc_netdev2_t gnrc_adpt[W5100_NUM];
/** @} */

/**
 * @brief   Stacks for the MAC layer threads
 */
static char stack[W5100_NUM][MAC_STACKSIZE];


void auto_init_w5100(void)
{
    for (unsigned i = 0; i < W5100_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing w5100 #%u\n", i);

        /* setup netdev2 device */
        w5100_setup(&dev[i], &w5100_params[i]);
        /* initialize netdev2 <-> gnrc adapter state */
        gnrc_netdev2_eth_init(&gnrc_adpt[i], (netdev2_t *)&dev[i]);
        /* start gnrc netdev2 thread */
        gnrc_netdev2_init(stack[i], MAC_STACKSIZE, MAC_PRIO,
                          "gnrc_w5100", &gnrc_adpt[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_W5100 */
/** @} */

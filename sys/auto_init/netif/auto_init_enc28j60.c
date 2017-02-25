/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Freie Universität Berlin
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
 * @brief       Auto initialization for ENC28j60 Ethernet devices
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef MODULE_ENC28J60

#include "log.h"
#include "enc28j60.h"
#include "enc28j60_params.h"
#include "net/gnrc/netdev2.h"
#include "net/gnrc/netdev2/eth.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define ENC28J60_MAC_STACKSIZE   (THREAD_STACKSIZE_DEFAULT)
#ifndef ENC28J60_MAC_PRIO
#define ENC28J60_MAC_PRIO        (GNRC_NETDEV2_MAC_PRIO)
#endif
/*** @} */

/**
 * @brief   Find out how many of these devices we need to care for
 */
#define ENC28J60_NUM    (sizeof(enc28j60_params) / sizeof(enc28j60_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 * @{
 */
static enc28j60_t dev[ENC28J60_NUM];
static gnrc_netdev2_t gnrc_adpt[ENC28J60_NUM];
/** @} */

/**
 * @brief   Stacks for the MAC layer threads
 */
static char stack[ENC28J60_NUM][ENC28J60_MAC_STACKSIZE];


void auto_init_enc28j60(void)
{
    for (unsigned i = 0; i < ENC28J60_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing enc28j60 #%u\n", i);

        /* setup netdev2 device */
        enc28j60_setup(&dev[i], &enc28j60_params[i]);
        /* initialize netdev2 <-> gnrc adapter state */
        gnrc_netdev2_eth_init(&gnrc_adpt[i], (netdev2_t *)&dev[i]);
        /* start gnrc netdev2 thread */
        gnrc_netdev2_init(stack[i], ENC28J60_MAC_STACKSIZE, ENC28J60_MAC_PRIO,
                          "gnrc_enc28j60", &gnrc_adpt[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_ENC28J60 */
/** @} */

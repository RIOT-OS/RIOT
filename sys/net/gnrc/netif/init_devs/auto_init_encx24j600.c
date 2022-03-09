/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief   Auto initialization for ENCx24j600 ethernet devices
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "log.h"
#include "debug.h"
#include "encx24j600.h"
#include "encx24j600_params.h"
#include "net/gnrc/netif/ethernet.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define ENCX24J600_MAC_STACKSIZE    (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef ENCX24J600_MAC_PRIO
#define ENCX24J600_MAC_PRIO         (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Find out how many of these devices are present
 */
#define ENCX24J600_NUM    ARRAY_SIZE(encx24j600_params)

/**
 * @brief Allocate device descriptors.
 */
static encx24j600_t encx24j600[ENCX24J600_NUM];

/**
 * @brief Allocate GNRC interfaces.
 */
static gnrc_netif_t _netif[ENCX24J600_NUM];

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev_eth_stack[ENCX24J600_NUM][ENCX24J600_MAC_STACKSIZE];

void auto_init_encx24j600(void)
{
    for (unsigned i = 0; i < ENCX24J600_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing encx24j600 #%u\n", i);

        /* setup netdev device */
        encx24j600_setup(&encx24j600[i], &encx24j600_params[i]);

        /* initialize netdev<->gnrc adapter state */
        gnrc_netif_ethernet_create(&_netif[i], _netdev_eth_stack[i], ENCX24J600_MAC_STACKSIZE,
                                   ENCX24J600_MAC_PRIO, "encx24j600",
                                   &encx24j600[i].netdev);
    }
}
/** @} */

/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for ENCx24j600 ethernet devices
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_ENCX24J600

#include "log.h"
#include "debug.h"
#include "encx24j600.h"
#ifdef MODULE_GNRC_NETIF2
#include "net/gnrc/netif2/ethernet.h"
#else
#include "net/gnrc/netdev.h"
#include "net/gnrc/netdev/eth.h"
#endif

static encx24j600_t encx24j600;

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define ENCX24J600_MAC_STACKSIZE    (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef ENCX24J600_MAC_PRIO
#ifdef MODULE_GNRC_NETIF2
#define ENCX24J600_MAC_PRIO         (GNRC_NETIF2_PRIO)
#else
#define ENCX24J600_MAC_PRIO         (GNRC_NETDEV_MAC_PRIO)
#endif
#endif

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev_eth_stack[ENCX24J600_MAC_STACKSIZE];
#ifndef MODULE_GNRC_NETIF2
static gnrc_netdev_t _gnrc_encx24j600;
#endif

void auto_init_encx24j600(void)
{
    LOG_DEBUG("[auto_init_netif] initializing encx24j600 #0\n");

    /* setup netdev device */
    encx24j600_params_t p;
    p.spi       = ENCX24J600_SPI;
    p.cs_pin    = ENCX24J600_CS;
    p.int_pin   = ENCX24J600_INT;
    encx24j600_setup(&encx24j600, &p);

    /* initialize netdev<->gnrc adapter state */
#ifdef MODULE_GNRC_NETIF2
    gnrc_netif2_ethernet_create(_netdev_eth_stack, ENCX24J600_MAC_STACKSIZE,
                                ENCX24J600_MAC_PRIO, "encx24j600",
                                (netdev_t *)&encx24j600);
#else
    gnrc_netdev_eth_init(&_gnrc_encx24j600, (netdev_t*)&encx24j600);

    /* start gnrc netdev thread */
    gnrc_netdev_init(_netdev_eth_stack, ENCX24J600_MAC_STACKSIZE,
                     ENCX24J600_MAC_PRIO, "gnrc_encx24j600",
                     &_gnrc_encx24j600);
#endif
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_ENCX24J600 */
/** @} */

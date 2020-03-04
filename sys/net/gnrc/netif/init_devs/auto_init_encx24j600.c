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

#ifdef MODULE_ENCX24J600

#include "log.h"
#include "debug.h"
#include "encx24j600.h"
#include "net/gnrc/netif/ethernet.h"

static encx24j600_t encx24j600;

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define ENCX24J600_MAC_STACKSIZE    (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef ENCX24J600_MAC_PRIO
#define ENCX24J600_MAC_PRIO         (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev_eth_stack[ENCX24J600_MAC_STACKSIZE];

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
    gnrc_netif_ethernet_create(_netdev_eth_stack, ENCX24J600_MAC_STACKSIZE,
                               ENCX24J600_MAC_PRIO, "encx24j600",
                               (netdev_t *)&encx24j600);
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_ENCX24J600 */
/** @} */

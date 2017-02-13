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
#include "net/gnrc/netdev2.h"
#include "net/gnrc/netdev2/eth.h"

static encx24j600_t encx24j600;

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define ENCX24J600_MAC_STACKSIZE    (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef ENCX24J600_MAC_PRIO
#define ENCX24J600_MAC_PRIO         (GNRC_NETDEV2_MAC_PRIO)
#endif

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev2_eth_stack[ENCX24J600_MAC_STACKSIZE];
static gnrc_netdev2_t _gnrc_encx24j600;

void auto_init_encx24j600(void)
{
    LOG_DEBUG("[auto_init_netif] initializing encx24j600 #0\n");

    /* setup netdev2 device */
    encx24j600_params_t p;
    p.spi       = ENCX24J600_SPI;
    p.cs_pin    = ENCX24J600_CS;
    p.int_pin   = ENCX24J600_INT;
    encx24j600_setup(&encx24j600, &p);

    /* initialize netdev2<->gnrc adapter state */
    gnrc_netdev2_eth_init(&_gnrc_encx24j600, (netdev2_t*)&encx24j600);

    /* start gnrc netdev2 thread */
    gnrc_netdev2_init(_netdev2_eth_stack, ENCX24J600_MAC_STACKSIZE,
                      ENCX24J600_MAC_PRIO, "gnrc_encx24j600",
                      &_gnrc_encx24j600);
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_ENCX24J600 */
/** @} */

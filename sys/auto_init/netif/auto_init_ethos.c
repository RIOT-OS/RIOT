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
 * @brief   Auto initialization for ethernet-over-serial module
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_ETHOS

#include "log.h"
#include "debug.h"
#include "ethos.h"
#include "periph/uart.h"
#ifdef MODULE_GNRC_NETIF2
#include "net/gnrc/netif2/ethernet.h"
#else
#include "net/gnrc/netdev.h"
#include "net/gnrc/netdev/eth.h"
#endif

/**
 * @brief global ethos object, used by uart_stdio
 */
ethos_t ethos;

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define ETHOS_MAC_STACKSIZE (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef ETHOS_MAC_PRIO
#ifdef MODULE_GNRC_NETIF2
#define ETHOS_MAC_PRIO      (GNRC_NETIF2_PRIO)
#else
#define ETHOS_MAC_PRIO      (GNRC_NETDEV_MAC_PRIO)
#endif
#endif

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev_eth_stack[ETHOS_MAC_STACKSIZE];
#ifndef MODULE_GNRC_NETIF2
static gnrc_netdev_t _gnrc_ethos;
#endif

static uint8_t _inbuf[2048];

void auto_init_ethos(void)
{
    LOG_DEBUG("[auto_init_netif] initializing ethos #0\n");

    /* setup netdev device */
    ethos_params_t p;
    p.uart      = ETHOS_UART;
    p.baudrate  = ETHOS_BAUDRATE;
    p.buf       = _inbuf;
    p.bufsize   = sizeof(_inbuf);
    ethos_setup(&ethos, &p);

    /* initialize netdev<->gnrc adapter state */
#ifdef MODULE_GNRC_NETIF2
    gnrc_netif2_ethernet_create(_netdev_eth_stack, ETHOS_MAC_STACKSIZE,
                                ETHOS_MAC_PRIO, "ethos", (netdev_t *)&ethos);
#else
    gnrc_netdev_eth_init(&_gnrc_ethos, (netdev_t*)&ethos);

    /* start gnrc netdev thread */
    gnrc_netdev_init(_netdev_eth_stack, ETHOS_MAC_STACKSIZE, ETHOS_MAC_PRIO,
                     "gnrc_ethos", &_gnrc_ethos);
#endif
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_ETHOS */
/** @} */

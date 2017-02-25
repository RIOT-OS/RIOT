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
#include "net/gnrc/netdev2.h"
#include "net/gnrc/netdev2/eth.h"

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
#define ETHOS_MAC_PRIO      (GNRC_NETDEV2_MAC_PRIO)
#endif

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev2_eth_stack[ETHOS_MAC_STACKSIZE];
static gnrc_netdev2_t _gnrc_ethos;

static uint8_t _inbuf[2048];

void auto_init_ethos(void)
{
    LOG_DEBUG("[auto_init_netif] initializing ethos #0\n");

    /* setup netdev2 device */
    ethos_params_t p;
    p.uart      = ETHOS_UART;
    p.baudrate  = ETHOS_BAUDRATE;
    p.buf       = _inbuf;
    p.bufsize   = sizeof(_inbuf);
    ethos_setup(&ethos, &p);

    /* initialize netdev2<->gnrc adapter state */
    gnrc_netdev2_eth_init(&_gnrc_ethos, (netdev2_t*)&ethos);

    /* start gnrc netdev2 thread */
    gnrc_netdev2_init(_netdev2_eth_stack, ETHOS_MAC_STACKSIZE, ETHOS_MAC_PRIO,
                      "gnrc_ethos", &_gnrc_ethos);
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_ETHOS */
/** @} */

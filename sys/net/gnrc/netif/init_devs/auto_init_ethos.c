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
 * @brief   Auto initialization for ethernet-over-serial module
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "log.h"
#include "debug.h"
#include "ethos.h"
#include "ethos_params.h"
#include "periph/uart.h"
#include "net/gnrc/netif/ethernet.h"

#define ETHOS_NUM ARRAY_SIZE(ethos_params)

/**
 * @brief global ethos object, used by stdio_uart
 */
ethos_t ethos[ETHOS_NUM];

static gnrc_netif_t _netif[ETHOS_NUM];

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define ETHOS_MAC_STACKSIZE (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef ETHOS_MAC_PRIO
#define ETHOS_MAC_PRIO      (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev_eth_stack[ETHOS_NUM][ETHOS_MAC_STACKSIZE];

static uint8_t _inbuf[ETHOS_NUM][2048];

void auto_init_ethos(void)
{
    for (unsigned i = 0; i < ETHOS_NUM; ++i) {
        LOG_DEBUG("[auto_init_netif] initializing ethos #%u\n", i);

        /* setup netdev device */
        ethos_setup(&ethos[i], &ethos_params[i], i, _inbuf[i], sizeof(_inbuf[i]));

        /* initialize netdev<->gnrc adapter state */
        gnrc_netif_ethernet_create(&_netif[i], _netdev_eth_stack[i], ETHOS_MAC_STACKSIZE,
                                   ETHOS_MAC_PRIO, "ethos", (netdev_t *)&ethos[i]);
    }
}
/** @} */

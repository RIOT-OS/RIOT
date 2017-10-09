/*
 * Copyright (C) 2017 Bumsik Kim <k.bumsik@gmail.com>
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
 * @brief   Auto initialization for WINC1500 Wi-Fi devices
 *
 * @author  Bumsik Kim <k.bumsik@gmail.com>
 */

#ifdef MODULE_WINC1500

#include "log.h"
#include "debug.h"
#include "winc1500.h"
#include "winc1500_params.h"
#include "net/gnrc/netdev.h"
#include "net/gnrc/netdev/eth.h"

/**
* @name   Define stack parameters for the MAC layer thread
* @{
*/
#define WINC1500_MAC_STACKSIZE    (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef WINC1500_MAC_PRIO
#define WINC1500_MAC_PRIO         (GNRC_NETDEV_MAC_PRIO)
#endif
/** @} */

extern winc1500_t winc1500; /** Located in winc1500.c */

static char _netdev_eth_stack[WINC1500_MAC_STACKSIZE];
static gnrc_netdev_t _gnrc_adpt;

void auto_init_winc1500(void)
{
   LOG_DEBUG("[auto_init_netif] initializing winc1500 #0\n");

   /* setup netdev device */
   winc1500_setup(&winc1500, &winc1500_params[0]);

   /* initialize netdev<->gnrc adapter state */
   gnrc_netdev_eth_init(&_gnrc_adpt, (netdev_t*)&winc1500);

   /* start gnrc netdev thread */
   gnrc_netdev_init(_netdev_eth_stack, WINC1500_MAC_STACKSIZE,
                    WINC1500_MAC_PRIO, "gnrc_winc1500",
                    &_gnrc_adpt);
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_WINC1500 */
/** @} */

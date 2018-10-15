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

#if defined(MODULE_WINC1500) && defined(MODULE_NETDEV_ETH)

#include "log.h"
#include "debug.h"
#include "winc1500.h"
#include "winc1500_params.h"
#include "net/gnrc/netif/ethernet.h"

/**
* @name   Define stack parameters for the MAC layer thread
* @{
*/
#define WINC1500_MAC_STACKSIZE    (THREAD_STACKSIZE_DEFAULT + \
                                   DEBUG_EXTRA_STACKSIZE + 2048 * 2)
#ifndef WINC1500_MAC_PRIO
#define WINC1500_MAC_PRIO         (GNRC_NETIF_PRIO)
#endif
/** @} */

#define WINC1500_NUM (sizeof(winc1500_params) / sizeof(winc1500_params[0]))

winc1500_t winc1500_devs[WINC1500_NUM];
static char _netdev_eth_stack[WINC1500_NUM][WINC1500_MAC_STACKSIZE];

void auto_init_winc1500(void)
{
    for (unsigned i = 0; i < WINC1500_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing winc1500 #0\n");

        /* setup netdev device */
        winc1500_setup(&winc1500_devs[i], &winc1500_params[i]);

        /* start gnrc netdev thread */
        gnrc_netif_ethernet_create(_netdev_eth_stack[i],
                                  WINC1500_MAC_STACKSIZE,
                                  WINC1500_MAC_PRIO, "winc1500",
                                  (netdev_t*)&winc1500_devs[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* defined(MODULE_WINC1500) && defined(MODULE_NETDEV_ETH) */
/** @} */

/*
 * Copyright (C) 2016 Freie Universität Berlin
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
 * @brief   Auto initialization for @ref netdev_socket_zep devices
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#ifdef MODULE_SOCKET_ZEP

#include "log.h"
#include "socket_zep.h"
#include "socket_zep_params.h"
#include "net/gnrc/netif/ieee802154.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 */
#define SOCKET_ZEP_MAC_STACKSIZE    (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef SOCKET_ZEP_MAC_PRIO
#define SOCKET_ZEP_MAC_PRIO         (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _socket_zep_stacks[SOCKET_ZEP_MAX][SOCKET_ZEP_MAC_STACKSIZE];
static socket_zep_t _socket_zeps[SOCKET_ZEP_MAX];

void auto_init_socket_zep(void)
{
    for (int i = 0; i < SOCKET_ZEP_MAX; i++) {
        LOG_DEBUG("[auto_init_netif: initializing socket ZEP device #%u\n", i);
        /* setup netdev device */
        socket_zep_setup(&_socket_zeps[i], &socket_zep_params[i]);
        gnrc_netif_ieee802154_create(_socket_zep_stacks[i],
                                     SOCKET_ZEP_MAC_STACKSIZE,
                                     SOCKET_ZEP_MAC_PRIO, "socket_zep",
                                     (netdev_t *)&_socket_zeps[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_SOCKET_ZEP */
/** @} */

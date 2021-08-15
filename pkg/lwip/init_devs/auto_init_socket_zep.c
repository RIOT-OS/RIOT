/*
 * Copyright (C) Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_auto_init_lwip_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for socket_zep network interfaces
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "socket_zep.h"
#include "socket_zep_params.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_SOCKET_ZEP_NUMOF        ARRAY_SIZE(socket_zep_params)

static struct netif netif[NETIF_SOCKET_ZEP_NUMOF];
static socket_zep_t socket_zep_devs[NETIF_SOCKET_ZEP_NUMOF];

void auto_init_socket_zep(void)
{
    for (unsigned i = 0; i < NETIF_SOCKET_ZEP_NUMOF; i++) {
        socket_zep_setup(&socket_zep_devs[i], &socket_zep_params[i], i);
        if (lwip_add_6lowpan(&netif[i], &socket_zep_devs[i].netdev.netdev) == NULL) {
            DEBUG("Could not add socket_zep device\n");
            return;
        }
    }
}
/** @} */

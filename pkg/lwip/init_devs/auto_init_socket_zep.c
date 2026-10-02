/*
 * SPDX-FileCopyrightText: 2021-2022 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "net/netdev/ieee802154_submac.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_SOCKET_ZEP_NUMOF        ARRAY_SIZE(socket_zep_params)

static lwip_netif_t netif[NETIF_SOCKET_ZEP_NUMOF];
static socket_zep_t socket_zep_devs[NETIF_SOCKET_ZEP_NUMOF];
static netdev_ieee802154_submac_t socket_zep_netdev[SOCKET_ZEP_MAX];

static void auto_init_socket_zep(void)
{
    for (unsigned i = 0; i < NETIF_SOCKET_ZEP_NUMOF; i++) {
        netdev_register(&socket_zep_netdev[i].dev.netdev, NETDEV_SOCKET_ZEP, i);
        netdev_ieee802154_submac_init(&socket_zep_netdev[i]);
        socket_zep_hal_setup(&socket_zep_devs[i], &socket_zep_netdev[i].submac.dev);

        socket_zep_setup(&socket_zep_devs[i], &socket_zep_params[i]);

        if (lwip_add_6lowpan(&netif[i], &socket_zep_netdev[i].dev.netdev) == NULL) {
            DEBUG("Could not add socket_zep device\n");
            return;
        }
    }
}

LWIP_INIT_6LOWPAN_NETIF(auto_init_socket_zep);
/** @} */

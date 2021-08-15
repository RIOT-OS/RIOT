/*
 * Copyright (C) Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author Erik Ekman <eekman@google.com>
 */

#include "kernel_defines.h"

#include "lwip/tcpip.h"
#include "lwip/netif/netdev.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "netif/lowpan6.h"

#ifdef MODULE_SOCKET_ZEP
#include "socket_zep.h"
#include "socket_zep_params.h"
#endif

#include "lwip.h"
#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#ifdef MODULE_SOCKET_ZEP
#define LWIP_NETIF_NUMOF        ARRAY_SIZE(socket_zep_params)
#endif

#ifdef LWIP_NETIF_NUMOF
static struct netif netif[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_SOCKET_ZEP
static socket_zep_t socket_zep_devs[LWIP_NETIF_NUMOF];
#endif

void lwip_bootstrap(void)
{
    lwip_netif_init_devs();
    /* TODO: do for every eligible netdev */
#ifdef LWIP_NETIF_NUMOF
#if defined(MODULE_SOCKET_ZEP)
    for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
        socket_zep_setup(&socket_zep_devs[i], &socket_zep_params[i], i);
        if (netif_add_noaddr(&netif[i], &socket_zep_devs[i].netdev.netdev, lwip_netdev_init,
                             tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add socket_zep device\n");
            return;
        }
    }
#endif
#endif
    /* also allow for external interface definition */
    tcpip_init(NULL, NULL);
#if IS_USED(MODULE_LWIP_DHCP_AUTO)
    {
        /* Start DHCP on all supported netifs. Interfaces that support
         * link status events will reset DHCP retries when link comes up. */
        struct netif *n = NULL;
        NETIF_FOREACH(n) {
            if (netif_is_flag_set(n, NETIF_FLAG_ETHERNET)) {
                netifapi_dhcp_start(n);
            }
        }
    }
#endif
}

/** @} */

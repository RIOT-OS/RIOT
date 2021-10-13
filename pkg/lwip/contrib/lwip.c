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

#include "lwip.h"
#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

void lwip_bootstrap(void)
{
    lwip_netif_init_devs();
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

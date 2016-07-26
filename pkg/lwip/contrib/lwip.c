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
 */

#include "lwip/tcpip.h"
#include "lwip/netif/netdev2.h"
#include "lwip/netif.h"
#include "netif/lowpan6.h"

#ifdef MODULE_NETDEV2_TAP
#include "netdev2_tap.h"
#endif

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#include "lwip.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifdef MODULE_NETDEV2_TAP
#define LWIP_NETIF_NUMOF        (1)
#endif

#ifdef MODULE_AT86RF2XX     /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        (sizeof(at86rf2xx_params) / sizeof(at86rf2xx_params[0]))
#endif

#ifdef LWIP_NETIF_NUMOF
static struct netif netif[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_devs[LWIP_NETIF_NUMOF];
#endif

void lwip_bootstrap(void)
{
    /* TODO: do for every eligable netdev2 */
#ifdef LWIP_NETIF_NUMOF
#ifdef MODULE_NETDEV2_TAP
    if (netif_add(&netif[0], &netdev2_tap, lwip_netdev2_init, tcpip_input) == NULL) {
        DEBUG("Could not add netdev2_tap device\n");
        return;
    }
#elif defined(MODULE_AT86RF2XX)
    for (int i = 0; i < LWIP_NETIF_NUMOF; i++) {
        at86rf2xx_setup(&at86rf2xx_devs[i], &at86rf2xx_params[i]);
        if (netif_add(&netif[i], &at86rf2xx_devs[i], lwip_netdev2_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add at86rf2xx device\n");
            return;
        }
    }
#endif
    if (netif[0].state != NULL) {
        /* state is set to a netdev2_t in the netif_add() functions above */
        netif_set_default(&netif[0]);
    }
#endif
    /* also allow for external interface definition */
    tcpip_init(NULL, NULL);
}

/** @} */

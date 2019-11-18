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
#include "lwip/netif/netdev.h"
#include "lwip/netif.h"
#include "netif/lowpan6.h"

#ifdef MODULE_NETDEV_TAP
#include "netdev_tap.h"
#include "netdev_tap_params.h"
#endif

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#ifdef MODULE_MRF24J40
#include "mrf24j40.h"
#include "mrf24j40_params.h"
#endif

#ifdef MODULE_SOCKET_ZEP
#include "socket_zep.h"
#include "socket_zep_params.h"
#endif

#ifdef MODULE_ESP_WIFI
#include "esp-wifi/esp_wifi_netdev.h"
#endif

#include "lwip.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifdef MODULE_NETDEV_TAP
#define LWIP_NETIF_NUMOF        (NETDEV_TAP_MAX)
#endif

#ifdef MODULE_AT86RF2XX     /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        ARRAY_SIZE(at86rf2xx_params)
#endif

#ifdef MODULE_MRF24J40     /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        ARRAY_SIZE(mrf24j40_params)
#endif

#ifdef MODULE_SOCKET_ZEP   /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        ARRAY_SIZE(socket_zep_params)
#endif

#ifdef MODULE_ESP_WIFI     /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        (1)
#endif

#ifdef LWIP_NETIF_NUMOF
static struct netif netif[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_NETDEV_TAP
static netdev_tap_t netdev_taps[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_devs[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_MRF24J40
static mrf24j40_t mrf24j40_devs[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_SOCKET_ZEP
static socket_zep_t socket_zep_devs[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_ESP_WIFI
extern esp_wifi_netdev_t _esp_wifi_dev;
extern void esp_wifi_setup (esp_wifi_netdev_t* dev);
#endif

void lwip_bootstrap(void)
{
    /* TODO: do for every eligible netdev */
#ifdef LWIP_NETIF_NUMOF
#ifdef MODULE_NETDEV_TAP
    for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
        netdev_tap_setup(&netdev_taps[i], &netdev_tap_params[i]);
        if (netif_add(&netif[i], &netdev_taps[i], lwip_netdev_init,
                      tcpip_input) == NULL) {
            DEBUG("Could not add netdev_tap device\n");
            return;
        }
    }
#elif defined(MODULE_MRF24J40)
    for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
        mrf24j40_setup(&mrf24j40_devs[i], &mrf24j40_params[i]);
        if (netif_add(&netif[i], &mrf24j40_devs[i], lwip_netdev_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add mrf24j40 device\n");
            return;
        }
    }
#elif defined(MODULE_AT86RF2XX)
    for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
        at86rf2xx_setup(&at86rf2xx_devs[i], &at86rf2xx_params[i]);
        if (netif_add(&netif[i], &at86rf2xx_devs[i], lwip_netdev_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add at86rf2xx device\n");
            return;
        }
    }
#elif defined(MODULE_SOCKET_ZEP)
    for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
        socket_zep_setup(&socket_zep_devs[i], &socket_zep_params[i]);
        if (netif_add(&netif[i], &socket_zep_devs[i], lwip_netdev_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add socket_zep device\n");
            return;
        }
    }
#elif defined(MODULE_ESP_WIFI)
    esp_wifi_setup(&_esp_wifi_dev);
    if (netif_add(&netif[0], &_esp_wifi_dev, lwip_netdev_init,
                  tcpip_input) == NULL) {
        DEBUG("Could not add esp_wifi device\n");
        return;
    }
#endif
    if (netif[0].state != NULL) {
        /* state is set to a netdev_t in the netif_add() functions above */
        netif_set_default(&netif[0]);
    }
#endif
    /* also allow for external interface definition */
    tcpip_init(NULL, NULL);
}

/** @} */

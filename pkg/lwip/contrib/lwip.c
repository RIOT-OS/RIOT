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

#ifdef MODULE_ENC28J60
#include "enc28j60.h"
#include "enc28j60_params.h"
#endif

#ifdef MODULE_MRF24J40
#include "mrf24j40.h"
#include "mrf24j40_params.h"
#endif

#ifdef MODULE_SOCKET_ZEP
#include "socket_zep.h"
#include "socket_zep_params.h"
#endif

#ifdef MODULE_ESP_ETH
#include "esp-eth/esp_eth_netdev.h"
#endif

#ifdef MODULE_ESP_WIFI
#include "esp-wifi/esp_wifi_netdev.h"
#endif

#ifdef MODULE_STM32_ETH
#include "stm32_eth.h"
#endif

#include "lwip.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifdef MODULE_NETDEV_TAP
#define LWIP_NETIF_NUMOF        (NETDEV_TAP_MAX)
#endif

#ifdef MODULE_AT86RF2XX     /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        AT86RF2XX_NUM
#endif

#ifdef MODULE_ENC28J60    /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        ARRAY_SIZE(enc28j60_params)
#endif

#ifdef MODULE_MRF24J40     /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        ARRAY_SIZE(mrf24j40_params)
#endif

#ifdef MODULE_SOCKET_ZEP   /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        ARRAY_SIZE(socket_zep_params)
#endif

#ifdef MODULE_ESP_ETH      /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        (1)
#endif

#ifdef MODULE_ESP_WIFI     /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        (1)
#endif

#ifdef MODULE_STM32_ETH
#define LWIP_NETIF_NUMOF        (1)
#endif

#ifdef LWIP_NETIF_NUMOF
static struct netif netif[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_NETDEV_TAP
static netdev_tap_t netdev_taps[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_AT86RF212B
static at86rf212b_t at86rf212b_devs[AT86RF212B_NUM_OF];
#endif
#ifdef MODULE_AT86RF231
static at86rf231_t at86rf231_devs[AT86RF231_NUM_OF];
#endif
#ifdef MODULE_AT86RF232
static at86rf232_t at86rf232_devs[AT86RF232_NUM_OF];
#endif
#ifdef MODULE_AT86RF233
static at86rf233_t at86rf233_devs[AT86RF233_NUM_OF];
#endif
#ifdef MODULE_AT86RFA1
static at86rfa1_t at86rfa1_dev;
#elif defined  MODULE_AT86RFR2
static at86rfr2_t at86rfr2_dev;
#endif

#ifdef MODULE_ENC28J60
static enc28j60_t enc28j60_devs[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_MRF24J40
static mrf24j40_t mrf24j40_devs[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_SOCKET_ZEP
static socket_zep_t socket_zep_devs[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_ESP_ETH
extern esp_eth_netdev_t _esp_eth_dev;
extern void esp_eth_setup (esp_eth_netdev_t* dev);
#endif

#ifdef MODULE_ESP_WIFI
extern esp_wifi_netdev_t _esp_wifi_dev;
extern void esp_wifi_setup(esp_wifi_netdev_t *dev);
#endif

#ifdef MODULE_STM32_ETH
static netdev_t stm32_eth;
extern void stm32_eth_netdev_setup(netdev_t *netdev);
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
    unsigned at86rf2xx_num = 0;
#ifdef MODULE_AT86RF212B
    at86rf212b_setup(at86rf212b_devs, at86rf212b_params, AT86RF212B_NUM_OF);
    for (unsigned i = 0;
         i < AT86RF212B_NUM_OF && at86rf2xx_num < LWIP_NETIF_NUMOF;
         i++, at86rf2xx_num++) {
        if (netif_add(&netif[at86rf2xx_num], &at86rf212b_devs[i], lwip_netdev_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add at86rf212b device\n");
            return;
        }
    }
#endif
#ifdef MODULE_AT86RF231
   at86rf231_setup(at86rf231_devs, at86rf231_params, AT86RF231_NUM_OF);
    for (unsigned i = 0;
         i < AT86RF231_NUM_OF && at86rf2xx_num < LWIP_NETIF_NUMOF;
         i++, at86rf2xx_num++) {
        if (netif_add(&netif[at86rf2xx_num], &at86rf231_devs[i], lwip_netdev_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add at86rf231 device\n");
            return;
        }
    }
#endif
#ifdef MODULE_AT86RF232
   at86rf232_setup(at86rf232_devs, at86rf232_params, AT86RF232_NUM_OF);
    for (unsigned i = 0;
         i < AT86RF232_NUM_OF && at86rf2xx_num < LWIP_NETIF_NUMOF;
         i++, at86rf2xx_num++) {
        if (netif_add(&netif[at86rf2xx_num], &at86rf232_devs[i], lwip_netdev_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add at86rf232 device\n");
            return;
        }
    }
#endif
#ifdef MODULE_AT86RF233
    at86rf233_setup(at86rf233_devs, at86rf233_params, AT86RF233_NUM_OF);
    for (unsigned i = 0;
         i < AT86RF233_NUM_OF && at86rf2xx_num < LWIP_NETIF_NUMOF;
         i++, at86rf2xx_num++) {
        if (netif_add(&netif[at86rf2xx_num], &at86rf233_devs[i], lwip_netdev_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add at86rf233 device\n");
            return;
        }
    }
#endif
#ifdef MODULE_AT86RFA1
   at86rfa1_setup(&at86rfa1_dev);
    if (at86rf2xx_num < LWIP_NETIF_NUMOF) {
        if (netif_add(&netif[at86rf2xx_num++], &at86rfa1_dev, lwip_netdev_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add at86rfa1 device\n");
            return;
        }
    }
#elif defined MODULE_AT86RFR2
   at86rfr2_setup(&at86rfr2_dev);
    if (at86rf2xx_num < LWIP_NETIF_NUMOF) {
        if (netif_add(&netif[at86rf2xx_num++], &at86rfr2_dev, lwip_netdev_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add at86rfr2 device\n");
            return;
        }
    }
#endif
#elif defined(MODULE_ENC28J60)
    for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
        enc28j60_setup(&enc28j60_devs[i], &enc28j60_params[i]);
#ifdef MODULE_LWIP_IPV4
        if (netif_add(&netif[0], IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY,
            &enc28j60_devs[i], lwip_netdev_init, tcpip_input) == NULL) {
            DEBUG("Could not add enc28j60 device\n");
            return;
        }
#else /* MODULE_LWIP_IPV4 */
        if (netif_add(&netif[0], &enc28j60_devs[i], lwip_netdev_init,
            tcpip_input) == NULL) {
            DEBUG("Could not add enc28j60 device\n");
            return;
        }
#endif /* MODULE_LWIP_IPV4 */
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
#elif defined(MODULE_ESP_ETH)
    esp_eth_setup(&_esp_eth_dev);
#ifdef MODULE_LWIP_IPV4
    if (netif_add(&netif[0], IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY,
                  &_esp_eth_dev, lwip_netdev_init, tcpip_input) == NULL) {
        DEBUG("Could not add esp_eth device\n");
        return;
    }
#else /* MODULE_LWIP_IPV4 */
    if (netif_add(&netif[0], &_esp_eth_dev, lwip_netdev_init,
                  tcpip_input) == NULL) {
        DEBUG("Could not add esp_eth device\n");
        return;
    }
#endif /* MODULE_LWIP_IPV4 */
#elif defined(MODULE_ESP_WIFI)
    esp_wifi_setup(&_esp_wifi_dev);
#ifdef MODULE_LWIP_IPV4
    if (netif_add(&netif[0], IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY,
                  &_esp_wifi_dev, lwip_netdev_init, tcpip_input) == NULL) {
        DEBUG("Could not add esp_wifi device\n");
        return;
    }
#else /* MODULE_LWIP_IPV4 */
    if (netif_add(&netif[0], &_esp_wifi_dev, lwip_netdev_init,
                  tcpip_input) == NULL) {
        DEBUG("Could not add esp_wifi device\n");
        return;
    }
#endif /* MODULE_LWIP_IPV4 */
#elif defined(MODULE_STM32_ETH)
    stm32_eth_netdev_setup(&stm32_eth);
#ifdef MODULE_LWIP_IPV4
    if (netif_add(&netif[0], IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY,
                  &stm32_eth, lwip_netdev_init, tcpip_input) == NULL) {
        DEBUG("Could not add stm32_eth device\n");
        return;
    }
#else /* MODULE_LWIP_IPV4 */
    if (netif_add(&netif[0], &stm32_eth, lwip_netdev_init,
                  tcpip_input) == NULL) {
        DEBUG("Could not add stm32_eth device\n");
        return;
    }
#endif /* MODULE_LWIP_IPV4 */
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

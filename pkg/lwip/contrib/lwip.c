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

#include "kernel_defines.h"

#if IS_USED(MODULE_LWIP_DHCP_AUTO)
#include "lwip/dhcp.h"
#endif
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

#ifdef MODULE_ATWINC15X0
#include "atwinc15x0.h"
#include "atwinc15x0_params.h"
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

#ifdef MODULE_NRF802154
#include "nrf802154.h"
#endif

#include "lwip.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#ifdef MODULE_NETDEV_TAP
#define LWIP_NETIF_NUMOF        (NETDEV_TAP_MAX)
#endif

#ifdef MODULE_AT86RF2XX     /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        ARRAY_SIZE(at86rf2xx_params)
#endif

#ifdef MODULE_ATWINC15X0    /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        ARRAY_SIZE(atwinc15x0_params)
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

/* is mutual exclusive with above ifdef */
#if IS_USED(MODULE_ESP_ETH) && IS_USED(MODULE_ESP_WIFI)
#define LWIP_NETIF_NUMOF        (2)
#define ESP_WIFI_INDEX          (1)
#elif IS_USED(MODULE_ESP_ETH) || IS_USED(MODULE_ESP_WIFI)
#define LWIP_NETIF_NUMOF        (1)
#define ESP_WIFI_INDEX          (0)
#endif

#ifdef MODULE_STM32_ETH
#define LWIP_NETIF_NUMOF        (1)
#endif

#ifdef MODULE_NRF802154
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

#ifdef MODULE_ATWINC15X0
static atwinc15x0_t atwinc15x0_devs[LWIP_NETIF_NUMOF];
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

#ifdef MODULE_NRF802154
static nrf802154_t nrf802154_dev;
#endif

/**
 * @brief   Add interface with IPv4 or dual stack support.
 */
static struct netif *_netif_add(struct netif *netif, void *state,
                                netif_init_fn init, netif_input_fn input)
{
#if IS_USED(MODULE_LWIP_IPV4)
    return netif_add(netif, ip_2_ip4(IP4_ADDR_ANY), ip_2_ip4(IP4_ADDR_ANY),
                     ip_2_ip4(IP4_ADDR_ANY), state, init, input);
#else /* IS_USED(MODULE_LWIP_IPV4) */
    return netif_add(netif, state, init, input);
#endif  /* IS_USED(MODULE_LWIP_IPV4) */
}

void lwip_bootstrap(void)
{
    (void)_netif_add;   /* in case it is not used */
    /* TODO: do for every eligible netdev */
#ifdef LWIP_NETIF_NUMOF
#ifdef MODULE_NETDEV_TAP
    for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
        netdev_tap_setup(&netdev_taps[i], &netdev_tap_params[i]);
        if (_netif_add(&netif[i], &netdev_taps[i], lwip_netdev_init,
                       tcpip_input) == NULL) {
            DEBUG("Could not add netdev_tap device\n");
            return;
        }
    }
#elif defined(MODULE_MRF24J40)
    for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
        mrf24j40_setup(&mrf24j40_devs[i], &mrf24j40_params[i], i);
        if (netif_add(&netif[i], &mrf24j40_devs[i], lwip_netdev_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add mrf24j40 device\n");
            return;
        }
    }
#elif defined(MODULE_AT86RF2XX)
    for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
        at86rf2xx_setup(&at86rf2xx_devs[i], &at86rf2xx_params[i], i);
        if (netif_add(&netif[i], &at86rf2xx_devs[i], lwip_netdev_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add at86rf2xx device\n");
            return;
        }
    }
#elif defined(MODULE_ATWINC15X0)
    for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
        atwinc15x0_setup(&atwinc15x0_devs[i], &atwinc15x0_params[i]);
        if (_netif_add(&netif[0], &atwinc15x0_devs[i], lwip_netdev_init,
                       tcpip_input) == NULL) {
            DEBUG("Could not add atwinc15x0 device\n");
            return;
        }
    }
#elif defined(MODULE_ENC28J60)
    for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
        enc28j60_setup(&enc28j60_devs[i], &enc28j60_params[i], i);
        if (_netif_add(&netif[0], &enc28j60_devs[i], lwip_netdev_init,
                       tcpip_input) == NULL) {
            DEBUG("Could not add enc28j60 device\n");
            return;
        }
    }
#elif defined(MODULE_SOCKET_ZEP)
    for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
        socket_zep_setup(&socket_zep_devs[i], &socket_zep_params[i], i);
        if (netif_add(&netif[i], &socket_zep_devs[i], lwip_netdev_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add socket_zep device\n");
            return;
        }
    }
#elif (IS_USED(MODULE_ESP_ETH) || IS_USED(MODULE_ESP_WIFI))
#if IS_USED(MODULE_ESP_ETH)
    esp_eth_setup(&_esp_eth_dev);
    if (_netif_add(&netif[0], &_esp_eth_dev, lwip_netdev_init,
                   tcpip_input) == NULL) {
        DEBUG("Could not add esp_eth device\n");
        return;
    }
#endif
#if IS_USED(MODULE_ESP_WIFI)
    esp_wifi_setup(&_esp_wifi_dev);
    if (_netif_add(&netif[ESP_WIFI_INDEX], &_esp_wifi_dev, lwip_netdev_init,
                   tcpip_input) == NULL) {
        DEBUG("Could not add esp_wifi device\n");
        return;
    }
#endif
#elif defined(MODULE_STM32_ETH)
    stm32_eth_netdev_setup(&stm32_eth);
    if (_netif_add(&netif[0], &stm32_eth, lwip_netdev_init,
                   tcpip_input) == NULL) {
        DEBUG("Could not add stm32_eth device\n");
        return;
    }
#elif defined(MODULE_NRF802154)
    nrf802154_setup(&nrf802154_dev);
    if (netif_add(&netif[0], (netdev_ieee802154_t *)&nrf802154_dev, lwip_netdev_init,
                tcpip_6lowpan_input) == NULL) {
        DEBUG("Could not add nrf802154 device\n");
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
#if IS_USED(MODULE_LWIP_DHCP_AUTO) && IS_USED(MODULE_NETDEV_TAP)
    /* XXX: Hack to get DHCP with IPv4 with `netdev_tap`, as it does
     * not emit a `NETDEV_EVENT_LINK_UP` event. Remove, once it does
     * at an appropriate point.
     * (see https://github.com/RIOT-OS/RIOT/pull/14150) */
    dhcp_start(netif);
#endif
}

/** @} */

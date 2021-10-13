/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread main functions
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @}
 */

#include "openthread/platform/alarm-milli.h"
#include "openthread/platform/uart.h"
#include "ot.h"
#include "random.h"
#include "thread.h"
#include "xtimer.h"

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#ifdef MODULE_KW41ZRF
#include "kw41zrf.h"
#endif

#ifdef MODULE_CC2538_RF
#include "cc2538_rf.h"
#endif

#ifdef MODULE_NRF802154
#include "nrf802154.h"
#endif

#if IS_USED(MODULE_NETDEV_IEEE802154_SUBMAC)
#include "net/netdev/ieee802154_submac.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef MODULE_AT86RF2XX     /* is mutual exclusive with above ifdef */
#define OPENTHREAD_NETIF_NUMOF        ARRAY_SIZE(at86rf2xx_params)
#endif

#ifdef MODULE_KW41ZRF
#define OPENTHREAD_NETIF_NUMOF        (1U)
#endif

#ifdef MODULE_CC2538_RF
static netdev_ieee802154_submac_t cc2538_rf_netdev;
#endif

#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_dev;
#endif

#ifdef MODULE_KW41ZRF
static kw41zrf_t kw41z_dev;
#endif

#ifdef MODULE_NRF802154
static netdev_ieee802154_submac_t nrf802154_netdev;
#endif

static uint8_t rx_buf[OPENTHREAD_NETDEV_BUFLEN];
static uint8_t tx_buf[OPENTHREAD_NETDEV_BUFLEN];
static char ot_thread_stack[2 * THREAD_STACKSIZE_MAIN];

void openthread_bootstrap(void)
{
    /* setup netdev modules */
#ifdef MODULE_AT86RF2XX
    at86rf2xx_setup(&at86rf2xx_dev, &at86rf2xx_params[0], 0);
    netdev_t *netdev = &at86rf2xx_dev.netdev.netdev;
#endif
#ifdef MODULE_KW41ZRF
    kw41zrf_setup(&kw41z_dev, 0);
    netdev_t *netdev = &kw41z_dev.netdev.netdev;
#endif
#ifdef MODULE_CC2538_RF
    netdev_register(&cc2538_rf_netdev.dev.netdev, NETDEV_CC2538, 0);
    netdev_ieee802154_submac_init(&cc2538_rf_netdev);
    cc2538_rf_hal_setup(&cc2538_rf_netdev.submac.dev);
    cc2538_init();
    netdev_t *netdev = &cc2538_rf_netdev.dev.netdev;
#endif
#ifdef MODULE_NRF802154
    netdev_register(&nrf802154_netdev.dev.netdev, NETDEV_NRF802154, 0);
    netdev_ieee802154_submac_init(&nrf802154_netdev);
    nrf802154_hal_setup(&nrf802154_netdev.submac.dev);
    nrf802154_init();
    netdev_t *netdev = &nrf802154_netdev.dev.netdev;
#endif

    openthread_radio_init(netdev, tx_buf, rx_buf);
    openthread_netdev_init(ot_thread_stack, sizeof(ot_thread_stack), THREAD_PRIORITY_MAIN - 5, "openthread", netdev);
}

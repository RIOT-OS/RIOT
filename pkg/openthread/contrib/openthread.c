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

#include <assert.h>

#include "openthread/platform/alarm-milli.h"
#include "openthread/platform/uart.h"
#include "ot.h"
#include "random.h"
#include "thread.h"
#include "xtimer.h"

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#define OPENTHREAD_NETIF_NUMOF        AT86RF2XX_NUM
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Only one type of transceiver is expected here */
#ifdef MODULE_AT86RF212B
static at86rf212b_t at86rf212b_dev;
#elif defined  MODULE_AT86RF231
static at86rf231_t at86rf231_dev;
#elif defined MODULE_AT86RF232
static at86rf232_t at86rf232_dev;
#elif defined MODULE_AT86RF233
static at86rf233_t at86rf233_dev;
#elif defined MODULE_AT86RFA1
static at86rfa1_t at86rfa1_dev;
#elif defined MODULE_AT86RFR2
static at86rfr2_t at86rfr2_dev;
#endif

static uint8_t rx_buf[OPENTHREAD_NETDEV_BUFLEN];
static uint8_t tx_buf[OPENTHREAD_NETDEV_BUFLEN];
static char ot_thread_stack[2 * THREAD_STACKSIZE_MAIN];

void openthread_bootstrap(void)
{
    /* init random */
    ot_random_init();

    netdev_t *netdev = NULL;
#ifdef MODULE_AT86RF212B
    at86rf212b_setup(&at86rf212b_dev, at86rf212b_params, 1);
    netdev = (netdev_t *)&at86rf212b_dev.base.netdev;
#elif defined  MODULE_AT86RF231
    at86rf231_setup(&at86rf212b_dev, at86rf231_params, 1);
    netdev = (netdev_t *)&at86rf231_dev.base.netdev;
#elif defined MODULE_AT86RF232
    at86rf232_setup(&at86rf232_dev, at86rf232_params, 1);
    netdev = (netdev_t *)&at86rf232_dev.base.netdev;
#elif defined MODULE_AT86RF233
    at86rf233_setup(&at86rf233_dev, at86rf233_params, 1);
    netdev = (netdev_t *)&at86rf233_dev.base.netdev;
#elif defined MODULE_AT86RFA1
    at86rfa1_setup(&at86rfa1_dev);
    netdev = (netdev_t *)&at86rfa1_dev.base.netdev;
#elif defined MODULE_AT86RFR2
    at86rfr2_setup(&at86rfr2_dev);
    netdev = (netdev_t *)&at86rfr2_dev.base.netdev;
#endif
    assert(netdev);
    openthread_radio_init(netdev, tx_buf, rx_buf);
    openthread_netdev_init(ot_thread_stack, sizeof(ot_thread_stack), THREAD_PRIORITY_MAIN - 5, "openthread", netdev);
}

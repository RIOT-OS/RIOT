/*
 * SPDX-FileCopyrightText: 2017 Fundacion Inria Chile
 * SPDX-FileCopyrightText: 2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread main functions
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @author      Moritz Voigt <moritz.voigt@mailbox.tu-dresden.de>
 * @}
 */

#include "ot.h"
#include "random.h"
#include "thread.h"


#ifdef MODULE_CC2538_RF
#include "cc2538_rf.h"
#endif

#ifdef MODULE_NRF802154
#include "nrf802154.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

static ieee802154_dev_t dev;
static uint8_t tx_is_ack = 0;

static uint8_t rx_buf[OPENTHREAD_NETDEV_BUFLEN];
static uint8_t tx_buf[OPENTHREAD_NETDEV_BUFLEN];
static char ot_thread_stack[2 * THREAD_STACKSIZE_MAIN];

void openthread_bootstrap(void)
{
#ifdef MODULE_CC2538_RF
    cc2538_rf_hal_setup(&dev);
    cc2538_init();
#endif
#ifdef MODULE_NRF802154
    nrf802154_hal_setup(&dev);
    nrf802154_init();
#endif

    if (openthread_radio_init(&dev, tx_buf, rx_buf, &tx_is_ack) < 0) {
        printf("Failed to initialize Radio");
    }
    openthread_hal_init(ot_thread_stack, sizeof(ot_thread_stack), THREAD_PRIORITY_MAIN - 5,
                        "openthread", &dev, &tx_is_ack);
}

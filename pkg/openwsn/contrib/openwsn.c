/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @brief       OpenWSN bootstrapping functions implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "scheduler.h"
#include "openstack.h"
#include "radio.h"

#include "openwsn.h"
#include "openwsn_board.h"
#include "openwsn_radio.h"

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#ifdef MODULE_CC2538_RF
#include "cc2538_rf.h"
#endif

#ifdef MODULE_NRF802154
#include "nrf802154.h"
#endif

#define LOG_LEVEL LOG_NONE
#include "log.h"

#define OPENWSN_SCHED_NAME            "openwsn"
#define OPENWSN_SCHED_PRIO            (THREAD_PRIORITY_MAIN - 4)
#define OPENWSN_SCHED_STACKSIZE       (2048)

#ifdef MODULE_OPENWSN_RADIO_NETDEV
#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_dev;
#endif
#else
#ifdef MODULE_CC2538_RF
static ieee802154_dev_t cc2538_rf_dev;
#endif
#ifdef MODULE_NRF802154
static ieee802154_dev_t nrf802154_hal_dev;
#endif
#endif

static char _stack[OPENWSN_SCHED_STACKSIZE];

static kernel_pid_t _pid = KERNEL_PID_UNDEF;

static void *_event_loop(void *arg);

kernel_pid_t openwsn_get_pid(void)
{
    return _pid;
}

void* _radio_init_dev(void)
{
    void* dev = NULL;
    /* avoid cppcheck style (redundantAssignment)*/
    (void) dev;
#ifdef MODULE_OPENWSN_RADIO_NETDEV
    #ifdef MODULE_AT86RF2XX
        dev = &at86rf2xx_dev.netdev.netdev;
        at86rf2xx_setup(&at86rf2xx_dev, &at86rf2xx_params[0], 0);
    #endif
#else
    #ifdef MODULE_CC2538_RF
        dev = &cc2538_rf_dev;
        cc2538_rf_hal_setup(dev);
        cc2538_init();
    #endif
    #ifdef MODULE_NRF802154
        dev = &nrf802154_hal_dev;
        nrf802154_hal_setup(dev);
        nrf802154_init();
    #endif
#endif
    return dev;
}

int openwsn_bootstrap(void)
{
    LOG_DEBUG("[openwsn]: init RIOT board\n");
    board_init_openwsn();

    LOG_DEBUG("[openwsn]: init radio\n");
    void* dev = _radio_init_dev();
    if (openwsn_radio_init(dev)) {
        LOG_ERROR("[openwsn]: failed to init radio\n");
        return -1;
    }

    LOG_DEBUG("[openwsn]: network thread\n");
    _pid = thread_create(_stack, OPENWSN_SCHED_STACKSIZE, OPENWSN_SCHED_PRIO,
                         0, _event_loop, NULL,
                         OPENWSN_SCHED_NAME);
    if (_pid <= 0) {
        LOG_ERROR("[openwsn]: couldn't create thread\n");
        return -1;
    }

    return _pid;
}

static void *_event_loop(void *arg)
{
    (void)arg;

    LOG_DEBUG("[openwsn]: init scheduler\n");
    scheduler_init();
    LOG_DEBUG("[openwsn]: init openstack\n");
    /* Disable IRQ while scheduler is not ready to start */
    unsigned irq_state = irq_disable();
    openstack_init();
    LOG_DEBUG("[openwsn]: start scheduler loop\n");
    scheduler_start(irq_state);

    return NULL;
}

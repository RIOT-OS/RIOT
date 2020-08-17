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
 * @brief       OpenWSN bootstraping functions implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "scheduler.h"
#include "openstack.h"
#include "radio.h"
#include "idmanager.h"

#include "openwsn.h"
#include "openwsn_board.h"
#include "openwsn_radio.h"

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#define LOG_LEVEL LOG_NONE
#include "log.h"

#define OPENWSN_SCHED_NAME            "openwsn"
#define OPENWSN_SCHED_PRIO            (THREAD_PRIORITY_MAIN - 4)
#define OPENWSN_SCHED_STACKSIZE       (2048)

#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_dev;
#endif

static char _stack[OPENWSN_SCHED_STACKSIZE];

static kernel_pid_t _pid = KERNEL_PID_UNDEF;

static void *_event_loop(void *arg);

kernel_pid_t openwsn_get_pid(void)
{
    return _pid;
}

#ifdef MODULE_OPENWSN_RADIO
void openwsn_set_addr_16b(netdev_t* dev)
{
    uint8_t addr[IEEE802154_SHORT_ADDRESS_LEN];
    dev->driver->get(dev, NETOPT_ADDRESS, addr, IEEE802154_SHORT_ADDRESS_LEN);
    open_addr_t id;
    id.type = ADDR_16B;
    memcpy(&id.addr_16b, addr, IEEE802154_SHORT_ADDRESS_LEN);
    idmanager_setMyID(&id);
}
#endif

int openwsn_bootstrap(void)
{
    LOG_DEBUG("[openwsn]: init RIOT board\n");
    board_init_openwsn();

#ifdef MODULE_AT86RF2XX
    netdev_t *netdev = (netdev_t *)&at86rf2xx_dev.netdev.netdev;
    at86rf2xx_setup(&at86rf2xx_dev, &at86rf2xx_params[0], 0);
    (void) netdev;
#endif

#ifdef MODULE_OPENWSN_RADIO
    LOG_DEBUG("[openwsn]: init radio\n");
    if (openwsn_radio_init(netdev)) {
        LOG_ERROR("[openwsn]: failed to init radio\n");
        return -1;
    }
#endif

    /* Initiate Id manager here and not in `openstack_init` function to allow
       overriding the short id address before additional stack components are
       initiated */
    idmanager_init();

#ifdef MODULE_OPENWSN_RADIO
    /* override 16b address to avoid short address collision */
    openwsn_set_addr_16b(netdev);
#endif

    LOG_DEBUG("[openwsn]: network thread\n");
    _pid = thread_create(_stack, OPENWSN_SCHED_STACKSIZE, OPENWSN_SCHED_PRIO,
                         THREAD_CREATE_STACKTEST, _event_loop, NULL,
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

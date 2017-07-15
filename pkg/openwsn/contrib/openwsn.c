/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */

#include "scheduler.h"
#include "openstack.h"
#include "board_ow.h"
#include "radio.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define OW_SCHED_NAME            "ow_network"
#define OW_SCHED_PRIO            (THREAD_PRIORITY_MAIN - 1)
#define OW_SCHED_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)

static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _stack[OW_SCHED_STACKSIZE];

static void *_event_loop(void *arg);

void openwsn_bootstrap(void)
{
    DEBUG("[openwsn_bootstrap]: init RIOT board\n");
    board_init_ow();

    DEBUG("[openwsn_bootstrap]: radio_init\n");
    /* initializes an own thread for the radio driver */
    radio_init();

    DEBUG("[openwsn_bootstrap]: network thread\n");
    if (_pid <= KERNEL_PID_UNDEF) {
        _pid = thread_create(_stack, OW_SCHED_STACKSIZE, OW_SCHED_PRIO,
                             THREAD_CREATE_STACKTEST, _event_loop, NULL,
                             OW_SCHED_NAME);
        if (_pid <= 0) {
            DEBUG("[openwsn_bootstrap]: couldn't create thread\n");
        }
    }
}

static void *_event_loop(void *arg)
{
    DEBUG("[openwsn_bootstrap]: init openstack\n");
    openstack_init();

    DEBUG("[openwsn_bootstrap]: init scheduler\n");
    scheduler_init();

    DEBUG("[openwsn_bootstrap]: start scheduler");
    /* starts the OpenWSN scheduler which contains a loop */
    scheduler_start();
    return NULL;
}


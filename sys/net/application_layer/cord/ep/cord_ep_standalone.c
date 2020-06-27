/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_cord_ep_standalone
 * @{
 *
 * @file
 * @brief       Standalone extension for the CoRE RD endpoint implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

#include "log.h"
#include "assert.h"
#include "thread.h"
#include "xtimer.h"
#include "net/cord/ep.h"
#include "net/cord/config.h"
#include "net/cord/ep_standalone.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* stack configuration */
#define STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#define PRIO                (THREAD_PRIORITY_MAIN - 1)
#define TNAME               "cord_ep"

#define UPDATE_TIMEOUT      (0xe537)

#define TIMEOUT_US          ((uint64_t)(CORD_UPDATE_INTERVAL * US_PER_SEC))

static char _stack[STACKSIZE];

static xtimer_t _timer;
static kernel_pid_t _runner_pid;
static msg_t _msg;

static cord_ep_cb_t _evt_cb = NULL;

static void _set_timer(void)
{
    xtimer_set_msg64(&_timer, TIMEOUT_US, &_msg, _runner_pid);
}

static void _on_ep_event(uint16_t event)
{
    if ((event == CORD_EP_REGISTERED) || (event == CORD_EP_UPDATE_OK)) {
        _set_timer();
    }
    else {
        xtimer_remove(&_timer);
    }

    if (_evt_cb) {
        _evt_cb(event);
    }
}

static void *_reg_runner(void *arg)
{
    (void)arg;
    msg_t in;

    /* prepare context and message */
    _runner_pid = thread_getpid();
    _msg.type = UPDATE_TIMEOUT;

    /* we want to be notified for every cord ep event */
    cord_ep_event_cb(_on_ep_event);

    while (1) {
        msg_receive(&in);
        cord_ep_update();
    }

    return NULL;    /* should never be reached */
}

void cord_ep_standalone_run(void)
{
    thread_create(_stack, sizeof(_stack), PRIO, THREAD_CREATE_STACKTEST,
                  _reg_runner, NULL, TNAME);
}

void cord_ep_standalone_event_cb(cord_ep_cb_t cb)
{
    _evt_cb = cb;
}

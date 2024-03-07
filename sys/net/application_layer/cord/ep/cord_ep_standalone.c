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
#include "ztimer.h"
#include "net/cord/ep.h"
#include "net/cord/config.h"
#include "net/cord/ep_standalone.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/* stack configuration */
#define STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#define PRIO                (THREAD_PRIORITY_MAIN - 1)
#define TNAME               "cord_ep"

#define UPDATE_TIMEOUT      (0xe537)

#define TIMEOUT_MS          (CONFIG_CORD_UPDATE_INTERVAL * MS_PER_SEC)

static char _stack[STACKSIZE];

static ztimer_t _timer;
static kernel_pid_t _runner_pid;
static msg_t _msg;

static cord_ep_standalone_cb_t _cb = NULL;

static void _set_timer(void)
{
    ztimer_set_msg(ZTIMER_MSEC, &_timer, TIMEOUT_MS, &_msg, _runner_pid);
}

static void _notify(cord_ep_standalone_event_t event)
{
    if (_cb) {
        _cb(event);
    }
}

static void *_reg_runner(void *arg)
{
    (void)arg;
    msg_t in;

    /* prepare context and message */
    _runner_pid = thread_getpid();
    _msg.type = UPDATE_TIMEOUT;

    while (1) {
        msg_receive(&in);
        if (in.type == UPDATE_TIMEOUT) {
            if (cord_ep_update() == CORD_EP_OK) {
                _set_timer();
                _notify(CORD_EP_UPDATED);
            }
            else {
                _notify(CORD_EP_DEREGISTERED);
            }
        }
    }

    return NULL;    /* should never be reached */
}

void cord_ep_standalone_run(void)
{
    thread_create(_stack, sizeof(_stack), PRIO, 0,
                  _reg_runner, NULL, TNAME);
}

void cord_ep_standalone_signal(bool connected)
{
    /* clear timer in any case */
    ztimer_remove(ZTIMER_MSEC, &_timer);
    /* reset the update timer in case a connection was established or updated */
    if (connected) {
        _set_timer();
        _notify(CORD_EP_REGISTERED);
    } else {
        _notify(CORD_EP_DEREGISTERED);
    }
}

void cord_ep_standalone_reg_cb(cord_ep_standalone_cb_t cb)
{
    /* Note: we do not allow re-setting the callback (via passing cb := NULL),
     *       as this would mean additional complexity for synchronizing the
     *       value of `_cb` to prevent concurrency issues... */
    assert(cb);
    _cb = cb;
}

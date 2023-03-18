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
#include "net/sock/util.h"
#include "net/gcoap.h"
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

static cord_ep_standalone_cb_t _cb = NULL;
static cord_ep_ctx_t _cord;

#if 0
static void _notify(cord_ep_standalone_event_t event)
{
    if (_cb) {
        _cb(event);
    }
}
#endif

static void *_reg_runner(void *arg)
{
    (void)arg;
    sock_udp_ep_t remote;
    sock_udp_name2ep(&remote, CORD_EP_STANDALONE_ADDRESS);
    if (remote.port == 0) {
        if (IS_USED(MODULE_GCOAP_DTLS)) {
            remote.port = CONFIG_GCOAPS_PORT;
        }
        else {
            remote.port = CONFIG_GCOAP_PORT;
        }
    }
    event_queue_t queue;
    event_queue_init(&queue);

    cord_ep_init(&_cord, &queue, &remote, NULL);

    event_loop(&queue);
    return NULL;    /* should never be reached */
}

void cord_ep_standalone_run(void)
{
    thread_create(_stack, sizeof(_stack), PRIO, THREAD_CREATE_STACKTEST,
                  _reg_runner, NULL, TNAME);
}

void cord_ep_standalone_signal(bool connected)
{
     (void)connected;
     return;
}

void cord_ep_standalone_reg_cb(cord_ep_standalone_cb_t cb)
{
    /* Note: we do not allow re-setting the callback (via passing cb := NULL),
     *       as this would mean additional complexity for synchronizing the
     *       value of `_cb` to prevent concurrency issues... */
    assert(cb);
    _cb = cb;
}

int cord_ep_standalone_register(const sock_udp_ep_t *remote, const char *regif)
{
    return cord_ep_register(&_cord, remote, regif);
}

void cord_ep_standalone_dump_status(void)
{
    cord_ep_dump_status(&_cord);
}

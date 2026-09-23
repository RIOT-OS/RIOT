/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_mynewt_core
 * @{
 *
 * @file
 * @brief       mynewt-core callout
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <assert.h>

#include "ztimer.h"
#include "os/os.h"
#include "os/os_callout.h"

static void _os_callout_timer_cb(void* arg)
{
    struct os_callout *c = (struct os_callout *) arg;
    assert(c);

    /* post the event if there is a queue, otherwise call the callback
       here */
    if (c->c_evq) {
        os_eventq_put(c->c_evq, &c->c_ev);
    } else {
        c->c_ev.e.callback(&c->c_ev);
    }
}

void os_callout_init(struct os_callout *c, struct os_eventq *q,
                      os_event_fn *e_cb, void *e_arg)
{
    os_event_init(&c->c_ev, e_cb, e_arg);
    c->c_evq = q;
    c->timer.callback = _os_callout_timer_cb;
    c->timer.arg = (void*) c;
}

int os_callout_reset(struct os_callout *c, os_time_t ticks)
{
    ztimer_set(ZTIMER_MSEC, &c->timer, ticks);
    return OS_OK;
}

void os_callout_stop(struct os_callout *c)
{
    ztimer_remove(ZTIMER_MSEC, &(c->timer));
}

/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core DPL (Decawave Porting Layer) callout
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <assert.h>

#include "xtimer.h"
#include "dpl/dpl_callout.h"

static void _dpl_callout_timer_cb(void* arg)
{
    struct dpl_callout *c = (struct dpl_callout *) arg;
    assert(c);

    /* post the event if there is a queue, otherwise call the callback
       here */
    if (c->c_q) {
        dpl_eventq_put(c->c_q, &c->c_e);
    } else {
        c->c_e.e.callback(&c->c_e);
    }
}

void dpl_callout_init(struct dpl_callout *c, struct dpl_eventq *q,
                      dpl_event_fn *e_cb, void *e_arg)
{
    dpl_event_init(&c->c_e, e_cb, e_arg);
    c->c_q = q;
    c->timer.callback = _dpl_callout_timer_cb;
    c->timer.arg = (void*) c;
}

dpl_error_t dpl_callout_reset(struct dpl_callout *c, dpl_time_t ticks)
{
    xtimer_ticks32_t val = {.ticks32 = ticks};
    xtimer_set(&(c->timer), xtimer_usec_from_ticks(val));
    return DPL_OK;
}

void dpl_callout_stop(struct dpl_callout *c)
{
    xtimer_remove(&(c->timer));
}

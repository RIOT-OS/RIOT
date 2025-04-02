/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_event
 * @{
 *
 * @file
 * @brief       Periodic Event Implementation
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */
#include "kernel_defines.h"
#include "ztimer.h"
#include "ztimer/periodic.h"
#include "event/periodic.h"

static bool _event_periodic_callback(void *arg)
{
    event_periodic_t *event_periodic = (event_periodic_t *)arg;
    event_post(event_periodic->queue, event_periodic->event);

    if (event_periodic->count) {
        if (--event_periodic->count == 0) {
            return !ZTIMER_PERIODIC_KEEP_GOING;
        }
    }

    return ZTIMER_PERIODIC_KEEP_GOING;
}

void event_periodic_init(event_periodic_t *event_periodic,
                         ztimer_clock_t *clock,
                         event_queue_t *queue, event_t *event)
{
    ztimer_periodic_init(clock, &event_periodic->timer, _event_periodic_callback,
                         event_periodic, 0);
    event_periodic->count = EVENT_PERIODIC_FOREVER;
    event_periodic->queue = queue;
    event_periodic->event = event;
}

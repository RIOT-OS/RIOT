/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Inria
 *               2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_ztimer
 * @{
 *
 * @file
 * @brief       ztimer periodic timer implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "ztimer.h"
#include "ztimer/periodic.h"

static void _ztimer_periodic_reset(ztimer_periodic_t *timer, ztimer_now_t now)
{
    ztimer_now_t target = timer->last + timer->interval;
    ztimer_now_t offset = target - now;

    if (offset > timer->interval) {
        offset = 0;
    }

    timer->last = target;

    ztimer_set(timer->clock, &timer->timer, offset);
}

static void _ztimer_periodic_callback(void *arg)
{
    ztimer_periodic_t *timer = arg;

    if (timer->callback(timer->arg) == ZTIMER_PERIODIC_KEEP_GOING) {
        ztimer_now_t now = ztimer_now(timer->clock);
        _ztimer_periodic_reset(timer, now);
    }
    else {
        timer->last = timer->last + timer->interval;
    }
}

void ztimer_periodic_init(ztimer_clock_t *clock, ztimer_periodic_t *timer,
                          bool (*callback)(void *), void *arg, uint32_t interval)
{
    /* check if this is a reinit, ensure timer is stopped in case */
    if (timer->timer.callback == _ztimer_periodic_callback) {
        ztimer_periodic_stop(timer);
    }
    *timer = (ztimer_periodic_t){
        .clock = clock, .interval = interval,
        .callback = callback, .arg = arg,
        .timer = {
            .callback = _ztimer_periodic_callback,
            .arg = timer
        }
    };
}

void ztimer_periodic_start(ztimer_periodic_t *timer)
{
    timer->last = ztimer_set(timer->clock, &timer->timer, timer->interval) + timer->interval;
}

void ztimer_periodic_start_now(ztimer_periodic_t *timer)
{
    timer->last = ztimer_set(timer->clock, &timer->timer, 0);
}

void ztimer_periodic_stop(ztimer_periodic_t *timer)
{
    ztimer_remove(timer->clock, &timer->timer);
}

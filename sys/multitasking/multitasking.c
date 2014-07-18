/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     sys
 * @{
 * @file
 * @brief       Simple multitasking features.
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include "kernel.h"
#include "multitasking.h"
#include "sched.h"
#include "vtimer.h"

#ifdef CPU_X86
#include "x86_hwtimer.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

static bool is_active = MULTITASKING_INITIAL;
static bool is_not_idle = false;
static bool is_set = false;

static vtimer_t timer;

static const timex_t interval = { 0, (uint32_t) (1e6 / (double) (MULTITASKING_HZ)) };

static void multitasking_interval_callback(struct vtimer_t *timer);

static void multitasking_do_set(void)
{
    __vtimer_set_callback(&timer, interval, VTIMER_RELATIVE, 0, NULL, multitasking_interval_callback);
}

static void multitasking_interval_callback(struct vtimer_t *t)
{
    (void) t;

    DEBUG("%s: request yield\n", __func__);

    sched_context_switch_request = 1;
    multitasking_do_set();
}

static void multitasking_changed(void)
{
    bool should_set = (is_active && is_not_idle);
    if (is_set == should_set) {
        return;
    }
    is_set = should_set;

    if (should_set) {
        DEBUG("%s: setting interrupt\n", __func__);
        multitasking_do_set();
    }
    else {
        DEBUG("%s: deactivating interrupt\n", __func__);
        vtimer_remove(&timer);
    }
}

void multitasking_active(bool active)
{
    if (active == is_active) {
        return;
    }
    is_active = active;

    DEBUG("%s: changed state: %s\n", __func__, is_active ? "multitasking :-)" : "no multitasking :-(");
    multitasking_changed();
}

void multitasking_sched_ran(void)
{
    bool not_idle = (sched_active_thread->priority != PRIORITY_IDLE);
#ifdef CPU_X86
    not_idle &= (sched_active_thread->pid != x86_hwtimer_pid);
#endif

    if (not_idle == is_not_idle) {
        return;
    }
    is_not_idle = not_idle;

    DEBUG("%s: changed state: %s\n", __func__, not_idle ? "running" : "idling");
    multitasking_changed();
}

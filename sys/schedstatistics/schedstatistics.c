/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Scheduler statistics implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include "sched.h"
#include "schedstatistics.h"
#include "thread.h"
#include "ztimer.h"

/**
 * When core_idle_thread is not active, the KERNEL_PID_UNDEF is used to track
 * the idle time
 */
schedstat_t sched_pidlist[KERNEL_PID_LAST + 1];

void sched_statistics_cb(kernel_pid_t active_thread, kernel_pid_t next_thread)
{
    uint32_t now = ztimer_now(ZTIMER_USEC);

    /* Update active thread stats */
    if (!IS_USED(MODULE_CORE_IDLE_THREAD) || active_thread != KERNEL_PID_UNDEF) {
        schedstat_t *active_stat = &sched_pidlist[active_thread];
        active_stat->runtime_us += now - active_stat->laststart;
    }

    /* Update next_thread stats */
    if (!IS_USED(MODULE_CORE_IDLE_THREAD) || next_thread != KERNEL_PID_UNDEF) {
        schedstat_t *next_stat = &sched_pidlist[next_thread];
        next_stat->laststart = now;
        next_stat->schedules++;
    }
}

void init_schedstatistics(void)
{
    /* Init laststart for the thread starting schedstatistics since the callback
       wasn't registered when it was first scheduled */
    schedstat_t *active_stat = &sched_pidlist[thread_getpid()];
    active_stat->laststart = ztimer_now(ZTIMER_USEC);
    active_stat->schedules = 1;
    sched_register_cb(sched_statistics_cb);
}

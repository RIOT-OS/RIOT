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
#include "xtimer.h"
#include "schedstatistics.h"

schedstat_t sched_pidlist[KERNEL_PID_LAST + 1];

void sched_statistics_cb(kernel_pid_t active_thread, kernel_pid_t next_thread)
{
    uint32_t now = xtimer_now().ticks32;

    /* Update active thread runtime, there is always an active thread since
       first sched_run happens when main_trampoline gets scheduled */
    schedstat_t *active_stat = &sched_pidlist[active_thread];
    active_stat->runtime_ticks += now - active_stat->laststart;

    /* Update next_thread stats */
    schedstat_t *next_stat = &sched_pidlist[next_thread];
    next_stat->laststart = now;
    next_stat->schedules++;
}

void init_schedstatistics(void)
{
    /* Init laststart for the thread starting schedstatistics since the callback
       wasn't registered when it was first scheduled */
    schedstat_t *active_stat = &sched_pidlist[sched_active_pid];
    active_stat->laststart = xtimer_now().ticks32;
    active_stat->schedules = 1;
    sched_register_cb(sched_statistics_cb);
}

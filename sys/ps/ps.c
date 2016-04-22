/**
 * Print thread information.
 *
 * Copyright (C) 2013, INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup ps
 * @{
 * @file
 * @brief   UNIX like ps command
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "sched.h"
#include "thread.h"
#include "kernel_types.h"

#ifdef MODULE_SCHEDSTATISTICS
#include "xtimer.h"
#endif

/* list of states copied from tcb.h */
const char *state_names[] = {
    [STATUS_RUNNING] = "running",
    [STATUS_PENDING] = "pending",
    [STATUS_STOPPED] = "stopped",
    [STATUS_SLEEPING] = "sleeping",
    [STATUS_MUTEX_BLOCKED] = "bl mutex",
    [STATUS_RECEIVE_BLOCKED] = "bl rx",
    [STATUS_SEND_BLOCKED] = "bl send",
    [STATUS_REPLY_BLOCKED] = "bl reply"
};

/**
 * @brief Prints a list of running threads including stack usage to stdout.
 */
void ps(void)
{
    const char queued_name[] = {'_', 'Q'};
#ifdef DEVELHELP
    int overall_stacksz = 0, overall_used = 0;
#endif

    printf("\tpid | "
#ifdef DEVELHELP
            "%-21s| "
#endif
            "%-9sQ | pri "
#ifdef DEVELHELP
           "| stack ( used) | location   "
#endif
#ifdef MODULE_SCHEDSTATISTICS
           "| runtime | switches"
#endif
           "\n",
#ifdef DEVELHELP
           "name",
#endif
           "state");

    for (kernel_pid_t i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; i++) {
        thread_t *p = (thread_t *)sched_threads[i];

        if (p != NULL) {
            int state = p->status;                                                 /* copy state */
            const char *sname = state_names[state];                                /* get state name */
            const char *queued = &queued_name[(int)(state >= STATUS_ON_RUNQUEUE)]; /* get queued flag */
#ifdef DEVELHELP
            int stacksz = p->stack_size;                                           /* get stack size */
            overall_stacksz += stacksz;
            stacksz -= thread_measure_stack_free(p->stack_start);
            overall_used += stacksz;
#endif
#ifdef MODULE_SCHEDSTATISTICS
            double runtime_ticks =  sched_pidlist[i].runtime_ticks / (double) xtimer_now() * 100;
            int switches = sched_pidlist[i].schedules;
#endif
            printf("\t%3" PRIkernel_pid
#ifdef DEVELHELP
                   " | %-20s"
#endif
                   " | %-8s %.1s | %3i"
#ifdef DEVELHELP
                   " | %5i (%5i) | %p "
#endif
#ifdef MODULE_SCHEDSTATISTICS
                   " | %6.3f%% |  %8d"
#endif
                   "\n",
                   p->pid,
#ifdef DEVELHELP
                   p->name,
#endif
                   sname, queued, p->priority
#ifdef DEVELHELP
                   , p->stack_size, stacksz, (void *)p->stack_start
#endif
#ifdef MODULE_SCHEDSTATISTICS
                   , runtime_ticks, switches
#endif
                  );
        }
    }

#ifdef DEVELHELP
    printf("\t%5s %-21s|%13s%6s %5i (%5i)\n", "|", "SUM", "|", "|",
           overall_stacksz, overall_used);
#endif
}

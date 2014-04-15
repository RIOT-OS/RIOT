/**
 * Print thread information.
 *
 * Copyright (C) 2013, INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup ps
 * @{
 * @file    ps.c
 * @brief   UNIX like ps command
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "hwtimer.h"
#include "sched.h"
#include "tcb.h"
#include "irq.h"

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

#define MAX_PRINT_THREADS 32

/**
 * @brief Prints a list of running threads including stack usage to stdout.
 */
void thread_print_all(void)
{
    unsigned old_state = disableIRQ();

    const char queued_name[2] = "_Q";
    int overall_stacksz = 0;

    static tcb_t *threads[MAX_PRINT_THREADS];
    unsigned thread_count = 0;

    for (int prio_level = 0; prio_level < SCHED_PRIO_LEVELS + 1; ++prio_level) {
        clist_node_t *start = runqueues[prio_level], *node = start;
        if (!start) {
            continue;
        }
        do {
            tcb_t *p = container_of(node, tcb_t, rq_entry);
            node = node->next;

            if (thread_count < MAX_PRINT_THREADS) {
                threads[thread_count++] = p;
            }
        } while (node != start);
    }

    printf("\tpid | %-21s| %-9sQ | pri | stack ( used) location  | runtime | switches \n", "name", "state");

    for (unsigned i = 0; i < thread_count; ++i) {
        tcb_t *p = threads[i];
        int state = p->status;                                                 // copy state
        char queued = queued_name[(int)(state >= STATUS_ON_RUNQUEUE)]; // get queued flag
        int stacksz = p->stack_size;                                           // get stack size
        double runtime_ticks = 0 / 0.0;
        int switches = -1;
#if SCHEDSTATISTICS
        runtime_ticks =  p->schedstats.runtime_ticks / (double) hwtimer_now() * 100;
        switches = p->schedstats.schedules;
#endif
        overall_stacksz += stacksz;
        stacksz -= thread_measure_stack_free(p->stack_start);
        printf("%p %c %u %u %u %f %u\n",
               p->stack_start, queued, p->priority, p->stack_size, stacksz, runtime_ticks, switches);
    }

    printf("%-21s|%13s%6s %5i\n", "SUM", "|", "|", overall_stacksz);

    restoreIRQ(old_state);
}

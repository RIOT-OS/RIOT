/**
 * Print thread information.
 *
 * Copyright (C) 2013, INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
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

#include <thread.h>
#include <thread.h>
#include <hwtimer.h>
#include <sched.h>
#include <stdio.h>

/* list of states copied from tcb.h */
const char *state_names[] = {
    "running",
    "pending",
    "stopped",
    "sleeping",
    "bl mutex",
    "bl rx",
    "bl send",
    "bl reply"
};

/**
 * @brief Prints a list of running threads including stack usage to stdout.
 */
void thread_print_all(void)
{
    extern unsigned long hwtimer_now(void);
    const char queued_name[] = {'_', 'Q'};
    int i;
    int overall_stacksz = 0;

    printf("\tpid | %-21s| %-9sQ | pri | stack ( used) location   | runtime | switches \n", "name", "state");

    for(i = 0; i < MAXTHREADS; i++) {
        tcb_t *p = (tcb_t *)sched_threads[i];

        if(p != NULL) {
            int state = p->status;                                          // copy state
            int statebit = number_of_highest_bit(state >> 1);               // get state index
            const char *sname = state_names[statebit];                      // get state name
            const char *queued = queued_name + (state & BIT0);              // get queued flag
            int stacksz = p->stack_size;                                    // get max used stack
            double runtime = 0 / 0.0;
            int switches = -1;
#if SCHEDSTATISTICS
            runtime =  pidlist[i].runtime / (double) hwtimer_now() * 100;
            switches = pidlist[i].schedules;
#endif
            overall_stacksz += stacksz;
            stacksz -= thread_measure_stack_usage(p->stack_start);
            printf("\t%3u | %-21s| %-8s %.1s | %3i | %5i (%5i) %p | %6.3f%% | %8i\n",
                   p->pid, p->name, sname, queued, p->priority, p->stack_size, stacksz, p->stack_start, runtime, switches);
        }
    }

    printf("\t%5s %-21s|%13s%6s %5i\n", "|", "SUM", "|", "|", overall_stacksz);
}

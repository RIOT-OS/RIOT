/**
 * The µkleos scheduler implementation
 *
 * Copyright (C) 2010 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup kernel
 * @{
 * @file
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @}
 */

#include <stdint.h>
#include <malloc.h>
#include "hwtimer.h"
#include "scheduler.h"
#include "kernel.h"
#include "kernel_intern.h"
#include "clist.h"
#include <bitarithm.h>

//#define ENABLE_DEBUG
#include "debug.h"

volatile int num_tasks = 0;

uint8_t last_pid = 0xFF;

clist_node_t *runqueues[SCHED_PRIO_LEVELS];
static uint32_t runqueue_bitcache = 0;

void sched_register_cb(void (*callback)(uint32_t, uint32_t));


#if SCHEDSTATISTICS
    static void (*sched_cb)(uint32_t timestamp, uint32_t value) = NULL;
    schedstat pidlist[MAXTHREADS];
#endif

void scheduler_init() {
    printf("Scheduler...");
    int i;
    for (i=0; i<MAXTHREADS; i++) {
        fk_threads[i] = NULL;
#if SCHEDSTATISTICS
        pidlist[i].laststart = 0;
        pidlist[i].runtime = 0;
        pidlist[i].schedules = 0;
#endif
    }

    fk_thread = NULL;
    fk_pid = -1;
    for (i = 0; i < SCHED_PRIO_LEVELS; i++) {
        runqueues[i] = NULL;
    }

    printf("[OK]\n");
}

void fk_schedule() {
    fk_context_switch_request = 0;

    tcb *my_fk_thread = (tcb*)fk_thread;

    if (my_fk_thread) {
        if( my_fk_thread->status ==  STATUS_RUNNING) {
            my_fk_thread->status =  STATUS_PENDING;
        }

#ifdef SCHED_TEST_STACK
        if (*((unsigned int*)my_fk_thread->stack_start) != (unsigned int) my_fk_thread->stack_start) {
            printf("scheduler(): stack overflow detected, task=%s pid=%u\n", my_fk_thread->name, my_fk_thread->pid);
        }
#endif

    }

#if SCHEDSTATISTICS
    extern unsigned long hwtimer_now(void);
    unsigned int time = hwtimer_now();
    if (my_fk_thread && (pidlist[my_fk_thread->pid].laststart)) {
        pidlist[my_fk_thread->pid].runtime += time - pidlist[my_fk_thread->pid].laststart;
    }
#endif

    DEBUG("\nscheduler: previous task: %s\n", ( my_fk_thread == NULL) ? "none" : my_fk_thread->name );

    if (num_tasks == 0) {
        DEBUG("scheduler: no tasks left.\n");
        while(! num_tasks);
        DEBUG("scheduler: new task created.\n");
    }

    my_fk_thread = NULL;
    while(! my_fk_thread) {

        //        for (int i = 0; i < SCHED_PRIO_LEVELS; i++) { /* TODO: introduce bitfield cache */
        //            if (runqueues[i]) {
        int nextrq = number_of_lowest_bit(runqueue_bitcache);
        clist_node_t next = *(runqueues[nextrq]);
        DEBUG("scheduler: first in queue: %s\n", ((tcb*)next.data)->name);
        clist_advance(&(runqueues[nextrq]));
        my_fk_thread = (tcb*)next.data;
        fk_pid = (volatile int) my_fk_thread->pid;
#if SCHEDSTATISTICS
        pidlist[my_fk_thread->pid].laststart = time;
        pidlist[my_fk_thread->pid].schedules ++;
#endif
        //                break;
        //            }
        //        }
#if SCHEDSTATISTICS
        if ((sched_cb) && (my_fk_thread->pid != last_pid)) {
            sched_cb(hwtimer_now(), my_fk_thread->pid);
            last_pid = my_fk_thread->pid;
        }
#endif
    }

    DEBUG("scheduler: next task: %s\n", my_fk_thread->name);

    if (my_fk_thread != fk_thread) {
        if (fk_thread != NULL) { //TODO: necessary?
            if (fk_thread->status ==  STATUS_RUNNING) {
                fk_thread->status =  STATUS_PENDING ;
            }
        }
        sched_set_status((tcb*)my_fk_thread,  STATUS_RUNNING);
    }

    fk_thread = (volatile tcb*) my_fk_thread;

    DEBUG("scheduler: done.\n");
}

#if SCHEDSTATISTICS
void sched_register_cb(void (*callback)(uint32_t, uint32_t)) {
    sched_cb = callback;    
}
#endif

void sched_set_status(tcb *process, unsigned int status) {
    if (status &  STATUS_ON_RUNQUEUE) {
        if (! (process->status &  STATUS_ON_RUNQUEUE)) {
            DEBUG("adding process %s to runqueue %u.\n", process->name, process->priority);
            clist_add(&runqueues[process->priority], &(process->rq_entry));
            runqueue_bitcache |= 1 << process->priority;
        }
    } else {
        if (process->status & STATUS_ON_RUNQUEUE) {
            DEBUG("removing process %s from runqueue %u.\n", process->name, process->priority);
            clist_remove(&runqueues[process->priority], &(process->rq_entry));
            if (! runqueues[process->priority] ) {
                runqueue_bitcache &= ~(1 << process->priority);
            }
        }
    }
    process->status = status;
}

extern void fk_switch_context_exit(void);

void fk_task_exit(void) {
    DEBUG("fk_task_exit(): ending task %s...\n", fk_thread->name);

    dINT();
    fk_threads[fk_thread->pid] = NULL;
    num_tasks--;
    sched_set_status((tcb*)fk_thread,  STATUS_STOPPED);

    free(((tcb*)fk_thread)->stack_start);
    fk_thread = NULL;
    fk_switch_context_exit();
}


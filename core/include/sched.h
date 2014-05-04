/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    core_sched Scheduler
 * @ingroup     core
 * @brief       The RIOT scheduler
 * @{
 *
 * @file        sched.h
 * @brief       Scheduler API definion
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <stddef.h>
#include "bitarithm.h"
#include "tcb.h"

#ifndef MAXTHREADS
#   define MAXTHREADS 32
#endif

#define SCHED_PRIO_LEVELS 16
#define PRIORITY_IDLE     15

#define PRIORITY_LEAST    13
#define PRIORITY_LOW      10
#define PRIORITY_MAIN     7
#define PRIORITY_HIGH     4
#define PRIORITY_HIGHEST  1

#define PRIORITY_UP       (-1)
#define PRIORITY_DOWN     (+1)

/**
 * @brief   Triggers the scheduler to schedule the next task
 */
void sched_run(void);

/**
 * @brief   Set the status of the specified process
 *
 * @param[in]   process     Pointer to the thread control block of the
 *                          targeted process
 * @param[in]   status      The new status of this thread
 */
void sched_set_status(tcb_t *process, unsigned int status);

/**
 * @brief   Compare thread priorities and yield() (or set
 *          sched_context_switch_request if in_isr) when other_prio is higher
 *          (has a lower value) than current_prio
 *
 * @param[in]   current_prio    The priority of the current thread
 * @param[in]   other_prio      The priority of the target thread
 */
void sched_switch(uint16_t current_prio, uint16_t other_prio);

/**
 * @brief   Call context switching at task exit
 */
void cpu_switch_context_exit(void);

/**
 * Flag indicating whether a context switch is necessary after handling an
 * interrupt. Supposed to be set in an ISR.
 */
extern volatile unsigned int sched_context_switch_request;

/**
 *  Thread table
 */
extern volatile tcb_t *sched_threads[MAXTHREADS];

/**
 *  Currently active thread
 */
extern volatile tcb_t *active_thread;

/**
 *  Number of running (non-terminated) threads
 */
extern volatile int num_tasks;

/**
 *  Process ID of active thread
 */
extern volatile int thread_pid;

#if SCHEDSTATISTICS
/**
 *  Scheduler statistics
 */
typedef struct {
    unsigned int laststart; /*< Time stamp of the last time this thread was
                                scheduled to run */
    unsigned int schedules; /*< How often the thread was scheduled to run */
    unsigned long runtime_ticks;   /*< The total runtime of this thread in ticks */
} schedstat;

/**
 *  Thread statistics table
 */
extern schedstat pidlist[MAXTHREADS];

/**
 *  Register a callback that will be called on every scheduler run
 */
void sched_register_cb(void (*callback)(uint32_t, uint32_t));
#endif

/** @} */
#endif // _SCHEDULER_H

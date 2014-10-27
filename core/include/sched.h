/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_sched Scheduler
 * @ingroup     core
 * @brief       The RIOT scheduler
 * @details
 *
 * RIOT features a tickless, preemptive, priority based scheduler.
 * Context switches can occur either preemptively (i.e. on interrupts),
 * voluntarily, or when a blocking operation (like `msg_receive()`) is
 * executed.
 * Being tickless means it does not have a timer that fires
 * periodically in order to emulate concurrent execution by switching
 * threads continuously.
 *
 * ## Priorities:
 *
 * Every thread is given a priority on creation. The priority values
 * are "order" or "nice" values, i.e. a higher value means a lower
 * priority.
 *
 * ### Example:
 *
 * Given threads with priorities A=6, B=1, and C=3, B has the highest
 * priority.
 * 
 * A higher priority means that the scheduler will run this thread
 * whenever it becomes runnable instead of a thread with a lower
 * priority.
 * In case of equal priorities, the threads are scheduled in a
 * semi-cooperative fashion. That means that unless an interrupt
 * happens, threads with the same priority will only switch due to
 * voluntary or implicit context switches.
 *
 * ## Interrupts:
 *
 * When an interrupt occurs, e.g. because a timer fired or a network
 * packet was received, the active context is saved and an interrupt
 * service routine (ISR) that handles the interrupt is executed in
 * another context.
 * When the ISR is finished, the `::sched_context_switch_request` flag
 * can be checked. In case it is set, the `sched_run()` function is
 * called to determine the next active thread. (In the special case
 * that the ISR knows that it can not enable a thread switch, this
 * check can of course be omitted.)
 * If the flag is not set, the original context is being restored and
 * the thread resumes immediately.
 *
 * ## Voluntary Context Switches:
 *
 * There are two function calls that can lead to a voluntary context
 * switch: `thread_yield()` and `thread_sleep()`.
 * While the latter disables (think blocks) the thread until it is
 * woken (think unblocked) again via `thread_wakeup()`, the former only
 * leads to a context switch in case there is another runnable thread
 * with at least the same priority.
 *
 * ## Implicit Context Switches:
 *
 * Some functions that unblock another thread, e.g. `msg_send()` or
 * `mutex_unlock()`, can cause a thread switch, if the target had a
 * higher priority.
 *
 *
 * @{
 *
 * @file        sched.h
 * @brief       Scheduler API definition
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <stddef.h>
#include "bitarithm.h"
#include "tcb.h"
#include "attributes.h"
#include "kernel_types.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @def SCHED_PRIO_LEVELS
 * @brief The number of thread priority levels
 */
#ifndef SCHED_PRIO_LEVELS
#define SCHED_PRIO_LEVELS 16
#endif

/**
 * @brief   Triggers the scheduler to schedule the next thread
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
 *          sched_context_switch_request if inISR()) when other_prio is higher
 *          (has a lower value) than the current thread's priority
 *
 * @param[in]   other_prio      The priority of the target thread
 */
void sched_switch(uint16_t other_prio);

/**
 * @brief   Call context switching at thread exit
 */
NORETURN void cpu_switch_context_exit(void);

/**
 * Flag indicating whether a context switch is necessary after handling an
 * interrupt. Supposed to be set in an ISR.
 */
extern volatile unsigned int sched_context_switch_request;

/**
 *  Thread table
 */
extern volatile tcb_t *sched_threads[KERNEL_PID_LAST + 1];

/**
 *  Currently active thread
 */
extern volatile tcb_t *sched_active_thread;

/**
 *  Number of running (non-terminated) threads
 */
extern volatile int sched_num_threads;

/**
 *  Process ID of active thread
 */
extern volatile kernel_pid_t sched_active_pid;

/**
 * List of runqueues per priority level
 */
extern clist_node_t *sched_runqueues[SCHED_PRIO_LEVELS];

#if SCHEDSTATISTICS
/**
 *  Scheduler statistics
 */
typedef struct {
    unsigned int laststart;         /**< Time stamp of the last time this thread was
                                         scheduled to run */
    unsigned int schedules;         /**< How often the thread was scheduled to run */
    unsigned long runtime_ticks;    /**< The total runtime of this thread in ticks */
} schedstat;

/**
 *  Thread statistics table
 */
extern schedstat sched_pidlist[KERNEL_PID_LAST + 1];

/**
 *  @brief  Register a callback that will be called on every scheduler run
 *
 *  @param[in] callback The callback functions the will be called
 */
void sched_register_cb(void (*callback)(uint32_t, uint32_t));

#endif

#ifdef __cplusplus
}
#endif

#endif // _SCHEDULER_H
/** @} */

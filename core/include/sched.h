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
 * @file
 * @brief       Scheduler API definition
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef SCHED_H
#define SCHED_H

#include <stddef.h>
#include <inttypes.h>

#include "kernel_defines.h"
#include "native_sched.h"
#include "clist.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def MAXTHREADS
 * @brief The maximum number of threads to be scheduled
 */
#ifndef MAXTHREADS
#define MAXTHREADS 32
#endif

/**
 * Canonical identifier for an invalid PID.
 */
#define KERNEL_PID_UNDEF 0

/**
 * The first valid PID (inclusive).
 */
#define KERNEL_PID_FIRST (KERNEL_PID_UNDEF + 1)

/**
 * The last valid PID (inclusive).
 */
#define KERNEL_PID_LAST (KERNEL_PID_FIRST + MAXTHREADS - 1)

/**
 * Macro for printing formatter
 */
#define PRIkernel_pid PRIi16

#if defined(DEVELHELP) || defined(DOXYGEN)
/**
 * Enables detection of stack overflows and measures stack usage when != 0
 */
#ifndef SCHED_TEST_STACK
#define SCHED_TEST_STACK    1
#endif  /* SCHED_TEST_STACK */
#endif  /* DEVELHELP */

/**
 * Unique process identifier
 */
typedef int16_t kernel_pid_t;

/**
 * @brief   Determine if the given pid is valid
 *
 * @param[in]   pid     The pid to check
 *
 * @return      true if the pid is valid, false otherwise
 */
static inline int pid_is_valid(kernel_pid_t pid)
{
    return ((KERNEL_PID_FIRST <= pid) && (pid <= KERNEL_PID_LAST));
}
/**
 * @brief forward declaration for thread_t, defined in thread.h
 */
typedef struct _thread thread_t;

/**
 * @name Thread states supported by RIOT
 *
 *       Keep in sync with OpenOCD src/rtos/riot.c
 * @{
 */
typedef enum {
    STATUS_STOPPED,                 /**< has terminated                           */
    STATUS_ZOMBIE,                  /**< has terminated & keeps thread's thread_t */
    STATUS_SLEEPING,                /**< sleeping                                 */
    STATUS_MUTEX_BLOCKED,           /**< waiting for a locked mutex               */
    STATUS_RECEIVE_BLOCKED,         /**< waiting for a message                    */
    STATUS_SEND_BLOCKED,            /**< waiting for message to be delivered      */
    STATUS_REPLY_BLOCKED,           /**< waiting for a message response           */
    STATUS_FLAG_BLOCKED_ANY,        /**< waiting for any flag from flag_mask      */
    STATUS_FLAG_BLOCKED_ALL,        /**< waiting for all flags in flag_mask       */
    STATUS_MBOX_BLOCKED,            /**< waiting for get/put on mbox              */
    STATUS_COND_BLOCKED,            /**< waiting for a condition variable         */
    STATUS_RUNNING,                 /**< currently running                        */
    STATUS_PENDING,                 /**< waiting to be scheduled to run           */
    STATUS_NUMOF                    /**< number of supported thread states        */
} thread_status_t;
/** @} */

/**
 * @name Helpers to work with thread states
 * @{
 */
#define STATUS_ON_RUNQUEUE      STATUS_RUNNING  /**< to check if on run queue:
                                                   `st >= STATUS_ON_RUNQUEUE`   */
#define STATUS_NOT_FOUND ((thread_status_t)-1)  /**< Describes an illegal thread status */
/** @} */
/**
 * @def SCHED_PRIO_LEVELS
 * @brief The number of thread priority levels
 */
#ifndef SCHED_PRIO_LEVELS
#define SCHED_PRIO_LEVELS 16
#endif

/**
 * @brief   Triggers the scheduler to schedule the next thread
 *
 * @returns     The new thread to schedule if sched_active_thread/sched_active_pid
 *              was changed,
 * @returns     NULL if the active thread was not changed.
 */
thread_t *sched_run(void);

/**
 * @brief   Set the status of the specified process
 *
 * @param[in]   process     Pointer to the thread control block of the
 *                          targeted process
 * @param[in]   status      The new status of this thread
 */
void sched_set_status(thread_t *process, thread_status_t status);

/**
 * @brief       Yield if appropriate.
 *
 * @details     Either yield if other_prio is higher than the current priority,
 *              or if the current thread is not on the runqueue.
 *
 *              Depending on whether the current execution is in an ISR (irq_is_in()),
 *              thread_yield_higher() is called or @ref sched_context_switch_request is set,
 *              respectively.
 *
 * @param[in]   other_prio      The priority of the target thread.
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
extern volatile thread_t *sched_threads[KERNEL_PID_LAST + 1];

/**
 *  Number of running (non-terminated) threads
 */
extern volatile int sched_num_threads;

/**
 * List of runqueues per priority level
 */
extern clist_node_t sched_runqueues[SCHED_PRIO_LEVELS];

/**
 * @brief  Removes thread from scheduler and set status to #STATUS_STOPPED
 */
NORETURN void sched_task_exit(void);

/**
 * @brief  Set CPU to idle mode (CPU dependent)
 *
 * Only used when there's no idle thread.
 *
 * This function will be called by the scheduler when there's no runnable thread.
 * It will be called from ISR context, and *must* allow other ISR handlers to be run.
 * E.g., on Cortex-M, the PendSV priority is temporarily lowered (set to higher
 * value) in order to enable other exceptions to be run.
 *
 * This function should also invoke setting a low power mode, e.g., by calling
 * 'pm_set_lowest()'.
 */
void sched_arch_idle(void);

#if IS_USED(MODULE_SCHED_CB) || defined(DOXYGEN)
/**
 * @brief   Scheduler run callback
 *
 * @note Both @p active and @p next can be KERNEL_PID_UNDEF, but not at the same
 * time.
 *
 * @param   active      Pid of the active thread pid
 * @param   next        Pid of the next scheduled thread
 */
typedef void (*sched_callback_t)(kernel_pid_t active, kernel_pid_t next);

/**
 * @brief  Register a callback that will be called on every scheduler run
 *
 * @param[in] callback The callback functions that will be called
 */
void sched_register_cb(sched_callback_t callback);
#endif /* MODULE_SCHED_CB */

/**
 * @brief   Advance a runqueue
 *
 *  Advances the runqueue of that priority by one step to the next thread in
 *  that priority.
 *  Next time that priority is scheduled the now first thread will get activated.
 *  Calling this will not start the scheduler.
 *
 * @warning This API is not intended for out of tree users.
 *          Breaking API changes will be done without notice and
 *          without deprecation. Consider yourself warned!
 *
 * @param   prio      The priority of the runqueue to advance
 *
 */
static inline void sched_runq_advance(uint8_t prio)
{
    clist_lpoprpush(&sched_runqueues[prio]);
}

#if (IS_USED(MODULE_SCHED_RUNQ_CALLBACK)) || defined(DOXYGEN)
/**
 * @brief   Scheduler runqueue (change) callback
 *
 * @details Function has to be provided by the user of this API.
 *          It will be called:
 *          - when the scheduler is run,
 *          - when a thread enters the active queue or
 *          - when the last thread leaves a queue
 *
 * @warning This API is not intended for out of tree users.
 *          Breaking API changes will be done without notice and
 *          without deprecation. Consider yourself warned!
 *
 * @param   prio      the priority of the runqueue that changed
 *
 */
extern void sched_runq_callback(uint8_t prio);
#endif

/**
 * @brief   Tell if the number of threads in a runqueue is 0
 *
 * @param[in]   prio      The priority of the runqueue to get information of
 * @return      Truth value for that information
 * @warning     This API is not intended for out of tree users.
 */
static inline int sched_runq_is_empty(uint8_t prio)
{
    return clist_is_empty(&sched_runqueues[prio]);
}

/**
 * @brief   Tell if the number of threads in a runqueue is 1
 *
 * @param[in]   prio      The priority of the runqueue to get information of
 * @return      Truth value for that information
 * @warning     This API is not intended for out of tree users.
 */
static inline int sched_runq_exactly_one(uint8_t prio)
{
    return clist_exactly_one(&sched_runqueues[prio]);
}

/**
 * @brief   Tell if the number of threads in a runqueue greater than 1
 *
 * @param[in]   prio      The priority of the runqueue to get information of
 * @return      Truth value for that information
 * @warning     This API is not intended for out of tree users.
 */
static inline int sched_runq_more_than_one(uint8_t prio)
{
    return clist_more_than_one(&sched_runqueues[prio]);
}

#ifdef __cplusplus
}
#endif

#endif /* SCHED_H */
/** @} */

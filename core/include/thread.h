/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_thread Threading
 * @ingroup     core
 * @brief       Support for multi-threading
 *
 * @{
 *
 * @file        thread.h
 * @brief       Threading API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __THREAD_H
#define __THREAD_H

#include "kernel.h"
#include "tcb.h"
#include "arch/thread_arch.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Describes an illegal thread status
 */
#define STATUS_NOT_FOUND (-1)

/**
 * @brief Minimum stack size
 */
#ifndef MINIMUM_STACK_SIZE
#define MINIMUM_STACK_SIZE  (sizeof(tcb_t))
#endif

/**
 * @brief Creates a new thread
 *
 * Creating a new thread is done in two steps:
 * 1. the new thread's stack is initialized depending on the platform
 * 2. the new thread is added to the scheduler to be run
 *
 * As RIOT is using a fixed priority scheduling algorithm, threads
 * are scheduled base on their priority. The priority is fixed for every thread
 * and specified during the threads creation by the *priority* parameter.
 *
 * A low value for *priority* number means the thread having a high priority
 * with 0 being the highest possible priority.
 *
 * The lowest possible priority is *PRIORITY_IDLE - 1*. The value is depending
 * on the platforms architecture, e.g. 30 in 32-bit systems, 14 in 16-bit systems.
 *
 *
 * In addition to the priority, the *flags* argument can be used to alter the
 * newly created threads behavior after creation. The following flags are available:
 *  - CREATE_SLEEPING       the newly created thread will be put to sleeping state and
 *                          must be waken up manually
 *  - CREATE_WOUT_YIELD     the newly created thread will not run immediately after creation
 *  - CREATE_STACKTEST      write markers into the thread's stack to measure the stack's memory
 *                          usage (for debugging and profiling purposes)
 *
 * @param[out] stack    start address of the preallocated stack memory
 * @param[in] stacksize the size of the thread's stack in bytes
 * @param[in] priority  priority of the new thread, lower mean higher priority
 * @param[in] flags     optional flags for the creation of the new thread
 * @param[in] function  pointer to the code that is executed in the new thread
 * @param[in] arg       the argument to the function
 * @param[in] name      a human readable descriptor for the thread
 *
 * @return              value ``<0`` on error
 * @return              pid of newly created task, otherwise
*/
kernel_pid_t thread_create(char *stack,
                  int stacksize,
                  char priority,
                  int flags,
                  thread_task_func_t task_func,
                  void *arg,
                  const char *name);

/**
 * @brief       Retreive a thread control block by PID.
 * @details     This is a bound-checked variant of accessing `sched_threads[pid]` directly.
 *              If you know that the PID is valid, then don't use this function.
 * @param[in]   pid   Thread to retreive.
 * @return      `NULL` if the PID is invalid or there is no such thread.
 */
volatile tcb_t *thread_get(kernel_pid_t pid);

/**
 * @brief Returns the status of a process
 *
 * @param[in] pid   the PID of the thread to get the status from
 *
 * @return          status of the thread
 * @return          `STATUS_NOT_FOUND` if pid is unknown
 */
int thread_getstatus(kernel_pid_t pid);

/**
 * @brief Puts the current thread into sleep mode. Has to be woken up externally.
 */
void thread_sleep(void);

/**
 * @brief   Lets current thread yield.
 *
 * @details The current thread will resume operation immediately,
 *          if there is no other ready thread with the same or a higher priority.
 *
 *          Differently from thread_yield_higher() the current thread will be put to the
 *          end of the threads in its priority class.
 *
 * @see     thread_yield_higher()
 */
void thread_yield(void);

/**
 * @brief   Lets current thread yield in favor of a higher prioritized thread.
 *
 * @details The current thread will resume operation immediately,
 *          if there is no other ready thread with a higher priority.
 *
 *          Differently from thread_yield() the current thread will be scheduled next
 *          in its own priority class, i.e. it stays the first thread in its
 *          priority class.
 *
 * @see     thread_yield()
 */
void thread_yield_higher(void);

/**
 * @brief Wakes up a sleeping thread.
 *
 * @param[in] pid   the PID of the thread to be woken up
 *
 * @return          `1` on success
 * @return          `STATUS_NOT_FOUND` if pid is unknown or not sleeping
 */
int thread_wakeup(kernel_pid_t pid);


/**
 * @brief Returns the process ID of the currently running thread
 *
 * @return          obviously you are not a golfer.
 */
static inline kernel_pid_t thread_getpid(void)
{
    return sched_active_pid;
}

#ifdef DEVELHELP
/**
 * @brief Returns the name of a process
 *
 * @param[in] pid   the PID of the thread to get the name from
 *
 * @return          the threads name
 * @return          `NULL` if pid is unknown
 */
const char *thread_getname(kernel_pid_t pid);

/**
 * @brief Measures the stack usage of a stack
 *
 * Only works if the thread was created with the flag CREATE_STACKTEST.
 *
 * @param[in] stack the stack you want to measure. try `sched_active_thread->stack_start`
 *
 * @return          the amount of unused space of the thread's stack
 */
uintptr_t thread_measure_stack_free(char *stack);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __THREAD_H */

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
 * @file
 * @brief       Threading API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef THREAD_H
#define THREAD_H

#include "priority_queue.h"
#include "clist.h"
#include "cib.h"
#include "msg.h"
#include "arch/thread_arch.h"
#include "cpu_conf.h"
#include "sched.h"
#include "clist.h"

#ifdef MODULE_CORE_THREAD_FLAGS
#include "thread_flags.h"
#endif

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Thread status list
 * @{
 */
#define STATUS_NOT_FOUND        (-1)            /**< Describes an illegal thread status */

/**
 * @brief Blocked states.
 * @{
 */
#define STATUS_STOPPED              0   /**< has terminated                     */
#define STATUS_SLEEPING             1   /**< sleeping                           */
#define STATUS_MUTEX_BLOCKED        2   /**< waiting for a locked mutex         */
#define STATUS_RECEIVE_BLOCKED      3   /**< waiting for a message              */
#define STATUS_SEND_BLOCKED         4   /**< waiting for message to be delivered*/
#define STATUS_REPLY_BLOCKED        5   /**< waiting for a message response     */
#define STATUS_FLAG_BLOCKED_ANY     6   /**< waiting for any flag from flag_mask*/
#define STATUS_FLAG_BLOCKED_ALL     7   /**< waiting for all flags in flag_mask */
/** @} */

/**
 * @brief These have to be on a run queue.
 * @{*/
#define STATUS_ON_RUNQUEUE      STATUS_RUNNING  /**< to check if on run queue:
                                                 `st >= STATUS_ON_RUNQUEUE`             */
#define STATUS_RUNNING          8               /**< currently running                  */
#define STATUS_PENDING          9               /**< waiting to be scheduled to run     */
/** @} */
/** @} */

/**
 * @brief @c thread_t holds thread's context data.
 */
struct _thread {
    char *sp;                       /**< thread's stack pointer         */
    uint8_t status;                 /**< thread's status                */
    uint8_t priority;               /**< thread's priority              */

    kernel_pid_t pid;               /**< thread's process id            */

#ifdef MODULE_CORE_THREAD_FLAGS
    thread_flags_t flags;           /**< currently set flags            */
#endif

    clist_node_t rq_entry;          /**< run queue entry                */

#if defined(MODULE_CORE_MSG) || defined(MODULE_CORE_THREAD_FLAGS)
    void *wait_data;                /**< used by msg and thread flags   */
#endif
#if defined(MODULE_CORE_MSG)
    priority_queue_t msg_waiters;   /**< threads waiting on message     */
    cib_t msg_queue;                /**< message queue                  */
    msg_t *msg_array;               /**< memory holding messages        */
#endif

#if defined DEVELHELP || defined(SCHED_TEST_STACK)
    char *stack_start;              /**< thread's stack start address   */
#endif
#ifdef DEVELHELP
    const char *name;               /**< thread's name                  */
    int stack_size;                 /**< thread's stack size            */
#endif
};

 /**
 * @def THREAD_STACKSIZE_DEFAULT
 * @brief A reasonable default stack size that will suffice most smaller tasks
 */
#ifndef THREAD_STACKSIZE_DEFAULT
#error THREAD_STACKSIZE_DEFAULT must be defined per CPU
#endif

/**
 * @def THREAD_STACKSIZE_IDLE
 * @brief Size of the idle task's stack in bytes
 */
#ifndef THREAD_STACKSIZE_IDLE
#error THREAD_STACKSIZE_IDLE must be defined per CPU
#endif

/**
 * @def THREAD_EXTRA_STACKSIZE_PRINTF
 * @ingroup conf
 * @brief Size of the task's printf stack in bytes
 */
#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#error THREAD_EXTRA_STACKSIZE_PRINTF must be defined per CPU
#endif

/**
 * @def THREAD_STACKSIZE_MAIN
 * @brief Size of the main task's stack in bytes
 */
#ifndef THREAD_STACKSIZE_MAIN
#define THREAD_STACKSIZE_MAIN      (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

/**
 * @brief Minimum stack size
 */
#ifndef THREAD_STACKSIZE_MINIMUM
#define THREAD_STACKSIZE_MINIMUM  (sizeof(thread_t))
#endif

/**
 * @def THREAD_PRIORITY_MIN
 * @brief Least priority a thread can have
 */
#define THREAD_PRIORITY_MIN            (SCHED_PRIO_LEVELS-1)

/**
 * @def THREAD_PRIORITY_IDLE
 * @brief Priority of the idle thread
 */
#define THREAD_PRIORITY_IDLE           (THREAD_PRIORITY_MIN)

/**
 * @def THREAD_PRIORITY_MAIN
 * @brief Priority of the main thread
 */
#define THREAD_PRIORITY_MAIN           (THREAD_PRIORITY_MIN - (SCHED_PRIO_LEVELS/2))

/**
 * @name Optional flags for controlling a threads initial state
 * @{
 */
/**
 * @brief Set the new thread to sleeping
 **/
#define THREAD_CREATE_SLEEPING          (1)

/**
 * @brief Currently not implemented
 */
#define THREAD_AUTO_FREE                (2)

/**
 * @brief Do not automatically call thread_yield() after creation
 */
#define THREAD_CREATE_WOUT_YIELD        (4)

 /**
  * @brief Write markers into the thread's stack to measure stack usage (for
  *        debugging)
  */
#define THREAD_CREATE_STACKTEST         (8)
/** @} */

/**
 * @brief Creates a new thread
 *
 * Creating a new thread is done in two steps:
 * 1. the new thread's stack is initialized depending on the platform
 * 2. the new thread is added to the scheduler to be run
 *
 * As RIOT is using a fixed priority scheduling algorithm, threads are
 * scheduled based on their priority. The priority is fixed for every thread
 * and specified during the threads creation by the *priority* parameter.
 *
 * A low value for *priority* number means the thread having a high priority
 * with 0 being the highest possible priority.
 *
 * The lowest possible priority is *THREAD_PRIORITY_IDLE - 1*. The value is depending
 * on the platforms architecture, e.g. 30 in 32-bit systems, 14 in 16-bit systems.
 *
 *
 * In addition to the priority, the *flags* argument can be used to alter the
 * newly created threads behavior after creation. The following flags are available:
 *  - THREAD_CREATE_SLEEPING    the newly created thread will be put to sleeping
 *                              state and must be woken up manually
 *  - THREAD_CREATE_WOUT_YIELD  the newly created thread will not run
 *                              immediately after creation
 *  - THREAD_CREATE_STACKTEST   write markers into the thread's stack to measure
 *                              the stack's memory usage (for debugging and
 *                              profiling purposes)
 *
 * @note Currently we support creating threads from within an ISR, however it
 *       is considered to be a bad programming practice and we strongly discourage
 *       it.
 *
 * @param[out] stack    start address of the preallocated stack memory
 * @param[in] stacksize the size of the thread's stack in bytes
 * @param[in] priority  priority of the new thread, lower mean higher priority
 * @param[in] flags     optional flags for the creation of the new thread
 * @param[in] task_func pointer to the code that is executed in the new thread
 * @param[in] arg       the argument to the function
 * @param[in] name      a human readable descriptor for the thread
 *
 * @return              PID of newly created task on success
 * @return              -EINVAL, if @p priority is greater than or equal to
 *                      @ref SCHED_PRIO_LEVELS
 * @return              -EOVERFLOW, if there are too many threads running already
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
volatile thread_t *thread_get(kernel_pid_t pid);

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
    extern volatile kernel_pid_t sched_active_pid;
    return sched_active_pid;
}

/**
 * @brief   Gets called upon thread creation to set CPU registers
 *
 * @param[in] task_func     First function to call within the thread
 * @param[in] arg           Argument to supply to task_func
 * @param[in] stack_start   Start address of the stack
 * @param[in] stack_size    Stack size
 *
 * @return stack pointer
 */
char *thread_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stack_size);

/**
 * @brief Add thread to list, sorted by priority (internal)
 *
 * This will add @p thread to @p list sorted by the thread priority.
 * It reuses the thread's rq_entry field.
 * Used internally by msg and mutex implementations.
 *
 * @note Only use for threads *not on any runqueue* and with interrupts
 *       disabled.
 *
 * @param[in] list      ptr to list root node
 * @param[in] thread    thread to add
 */
void thread_add_to_list(list_node_t *list, thread_t *thread);

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
 * Only works if the thread was created with the flag THREAD_CREATE_STACKTEST.
 *
 * @param[in] stack the stack you want to measure. try `sched_active_thread->stack_start`
 *
 * @return          the amount of unused space of the thread's stack
 */
uintptr_t thread_measure_stack_free(char *stack);
#endif /* DEVELHELP */

/**
 * @brief   Prints human readable, ps-like thread information for debugging purposes
 */
void thread_print_stack(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* THREAD_H */

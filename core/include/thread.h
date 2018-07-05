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
 * Priorities
 * ==========
 *
 * As RIOT is using a fixed priority @ref core_sched "scheduling algorithm",
 * threads are scheduled based on their priority. The priority is fixed for
 * every thread and specified during the thread's creation by the `priority`
 * parameter.
 *
 * The lower the priority value, the higher the priority of the thread,
 * with 0 being the highest possible priority.
 *
 * The lowest possible priority is @ref THREAD_PRIORITY_IDLE - 1.
 *
 * @note Assigning the same priority to two or more threads is usually not a
 *       good idea. A thread in RIOT may run until it yields (@ref
 *       thread_yield) or another thread with higher priority is runnable (@ref
 *       STATUS_ON_RUNQUEUE) again. Multiple threads with the same priority
 *       will therefore be scheduled cooperatively: when one of them is running,
 *       all others with the same priority depend on it to yield (or be interrupted
 *       by a thread with higher priority).
 *       This may make it difficult to determine when which of them gets
 *       scheduled and how much CPU time they will get. In most applications,
 *       the number of threads in application is significantly smaller than the
 *       number of available priorities, so assigning distinct priorities per
 *       thread should not be a problem. Only assign the same priority to
 *       multiple threads if you know what you are doing!
 *
 * Thread Behavior
 * ===============
 * In addition to the priority, flags can be used when creating a thread to
 * alter the thread's behavior after creation. The following flags are available:
 *
 *  Flags                         | Description
 *  ----------------------------- | --------------------------------------------------
 *  @ref THREAD_CREATE_SLEEPING   | the thread will sleep until woken up manually
 *  @ref THREAD_CREATE_WOUT_YIELD | the thread might not run immediately after creation
 *  @ref THREAD_CREATE_STACKTEST  | measures the stack's memory usage
 *
 * Thread creation
 * ===============
 * Creating a new thread is internally done in two steps:
 * 1. the new thread's stack is initialized depending on the platform
 * 2. the new thread is added to the scheduler and the scheduler is run (if not
 *    indicated otherwise)
 *
 * @note Creating threads from within an ISR is currently supported, however it
 *       is considered to be a bad programming practice and we strongly
 *       discourage you from doing so.
 *
 * Usage
 * -----
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "thread.h"
 *
 * char rcv_thread_stack[THREAD_STACKSIZE_MAIN];
 *
 * void *rcv_thread(void *arg)
 * {
 *     (void) arg;
 *     msg_t m;
 *
 *     while (1) {
 *         msg_receive(&m);
 *         printf("Got msg from %" PRIkernel_pid "\n", m.sender_pid);
 *     }
 *
 *     return NULL;
 * }
 *
 * int main(void)
 * {
 *     thread_create(rcv_thread_stack, sizeof(rcv_thread_stack),
 *                   THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
 *                   rcv_thread, NULL, "rcv_thread");
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Reading from the top down, you can see that first, stack memory for our thread
 * `rcv_thread` is preallocated, followed by an implementation of the thread's
 * function. Communication between threads is done using @ref core_msg. In this
 * case, `rcv_thread` will print the process id of each thread that sent a
 * message to `rcv_thread`.
 *
 * After it has been properly defined, `rcv_thread` is created with a call to
 * @ref thread_create() in `main()`. It is assigned a priority of
 * `THREAD_PRIORITY_MAIN - 1`, i.e. a slightly *higher* priority than the main
 * thread. Since neither the `THREAD_CREATE_SLEEPING` nor the
 * `THREAD_CREATE_WOUT_YIELD` flag is set, `rcv_thread` will be executed
 * immediately.
 *
 * @note If the messages to the thread are sent using @ref msg_try_send() or
 *       from an ISR, activate your thread's message queue by calling
 *       @ref msg_init_queue() to prevent messages from being dropped when
 *       they can't be handled right away. The same applies if you'd like
 *       msg_send() to your thread to be non-blocking. For more details, see
 *       @ref core_msg "the Messaging documentation".
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

#include "clist.h"
#include "cib.h"
#include "msg.h"
#include "cpu_conf.h"
#include "sched.h"

#ifdef MODULE_CORE_THREAD_FLAGS
#include "thread_flags.h"
#endif

#ifdef __cplusplus
 extern "C" {
#endif

/* Thread states */
/**
 * @name Special meaning thread states
 * @{
 */
#define STATUS_NOT_FOUND        (-1)    /**< Describes an illegal thread status */
/** @} */

/**
 * @name Blocked thread states
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
#define STATUS_MBOX_BLOCKED         8   /**< waiting for get/put on mbox        */
/** @} */

/**
 * @name Queued thread states
 * @{
 */
#define STATUS_ON_RUNQUEUE      STATUS_RUNNING  /**< to check if on run queue:
                                                 `st >= STATUS_ON_RUNQUEUE`             */
#define STATUS_RUNNING          9               /**< currently running                  */
#define STATUS_PENDING         10               /**< waiting to be scheduled to run     */
/** @} */

/**
 * @brief Prototype for a thread entry function
 */
typedef void *(*thread_task_func_t)(void *arg);

/**
 * @brief @c thread_t holds thread's context data.
 */
struct _thread {
    char *sp;                       /**< thread's stack pointer         */
    uint8_t status;                 /**< thread's status                */
    uint8_t priority;               /**< thread's priority              */

    kernel_pid_t pid;               /**< thread's process id            */

#if defined(MODULE_CORE_THREAD_FLAGS) || defined(DOXYGEN)
    thread_flags_t flags;           /**< currently set flags            */
#endif

    clist_node_t rq_entry;          /**< run queue entry                */

#if defined(MODULE_CORE_MSG) || defined(MODULE_CORE_THREAD_FLAGS) \
    || defined(MODULE_CORE_MBOX) || defined(DOXYGEN)
    void *wait_data;                /**< used by msg, mbox and thread
                                         flags                          */
#endif
#if defined(MODULE_CORE_MSG) || defined(DOXYGEN)
    list_node_t msg_waiters;        /**< threads waiting for their message
                                         to be delivered to this thread
                                         (i.e. all blocked sends)       */
    cib_t msg_queue;                /**< index of this [thread's message queue]
                                         (thread_t::msg_array), if any  */
    msg_t *msg_array;               /**< memory holding messages sent
                                         to this thread's message queue */
#endif
#if defined(DEVELHELP) || defined(SCHED_TEST_STACK) \
    || defined(MODULE_MPU_STACK_GUARD) || defined(DOXYGEN)
    char *stack_start;              /**< thread's stack start address   */
#endif
#if defined(DEVELHELP) || defined(DOXYGEN)
    const char *name;               /**< thread's name                  */
    int stack_size;                 /**< thread's stack size            */
#endif
#ifdef HAVE_THREAD_ARCH_T
    thread_arch_t arch;             /**< architecture dependent part    */
#endif
};

/**
 * @def THREAD_STACKSIZE_DEFAULT
 * @brief A reasonable default stack size that will suffice most smaller tasks
 *
 * @note This value must be defined by the CPU specific implementation, please
 *       take a look at @c cpu/$CPU/include/cpu_conf.h
 */
#ifndef THREAD_STACKSIZE_DEFAULT
#error THREAD_STACKSIZE_DEFAULT must be defined per CPU
#endif
#ifdef DOXYGEN
#define THREAD_STACKSIZE_DEFAULT
#endif

/**
 * @def THREAD_STACKSIZE_IDLE
 * @brief Size of the idle task's stack in bytes
 *
 * @note This value must be defined by the CPU specific implementation, please
 *       take a look at @c cpu/$CPU/include/cpu_conf.h
 */
#ifndef THREAD_STACKSIZE_IDLE
#error THREAD_STACKSIZE_IDLE must be defined per CPU
#endif
#ifdef DOXYGEN
#define THREAD_STACKSIZE_IDLE
#endif

/**
 * @def THREAD_EXTRA_STACKSIZE_PRINTF
 * @brief Size of the task's printf stack in bytes
 *
 * @note This value must be defined by the CPU specific implementation, please
 *       take a look at @c cpu/$CPU/include/cpu_conf.h
 */
#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#error THREAD_EXTRA_STACKSIZE_PRINTF must be defined per CPU
#endif
#ifdef DOXYGEN
#define THREAD_EXTRA_STACKSIZE_PRINTF
#endif

/**
 * @def THREAD_STACKSIZE_MAIN
 * @brief Size of the main task's stack in bytes
 */
#ifndef THREAD_STACKSIZE_MAIN
#define THREAD_STACKSIZE_MAIN      (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

/**
 * @brief Large stack size
 */
#ifndef THREAD_STACKSIZE_LARGE
#define THREAD_STACKSIZE_LARGE (THREAD_STACKSIZE_MEDIUM * 2)
#endif

/**
 * @brief Medium stack size
 */
#ifndef THREAD_STACKSIZE_MEDIUM
#define THREAD_STACKSIZE_MEDIUM THREAD_STACKSIZE_DEFAULT
#endif

/**
 * @brief Small stack size
 */
#ifndef THREAD_STACKSIZE_SMALL
#define THREAD_STACKSIZE_SMALL (THREAD_STACKSIZE_MEDIUM / 2)
#endif

/**
 * @brief Tiny stack size
 */
#ifndef THREAD_STACKSIZE_TINY
#define THREAD_STACKSIZE_TINY (THREAD_STACKSIZE_MEDIUM / 4)
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
 * @brief Set the new thread to sleeping. It must be woken up manually.
 **/
#define THREAD_CREATE_SLEEPING          (1)

/**
 * @brief Currently not implemented
 */
#define THREAD_AUTO_FREE                (2)

/**
 * @brief Do not automatically call thread_yield() after creation: the newly
 *        created thread might not run immediately. Purely for optimization.
 *        Any other context switch (i.e. an interrupt) can still start the
 *        thread at any time!
 */
#define THREAD_CREATE_WOUT_YIELD        (4)

 /**
  * @brief Write markers into the thread's stack to measure stack usage (for
  *        debugging and profiling purposes)
  */
#define THREAD_CREATE_STACKTEST         (8)
/** @} */

/**
 * @brief Creates a new thread.
 *
 * For an in-depth discussion of thread priorities, behavior and and flags,
 * see @ref core_thread.
 *
 * @note Avoid assigning the same priority to two or more threads.
 * @note Creating threads from within an ISR is currently supported, however it
 *       is considered to be a bad programming practice and we strongly
 *       discourage you from doing so.
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
 *          end of the thread's in its priority class.
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

/**
 * @brief Returns the name of a process
 *
 * @note when compiling without DEVELHELP, this *always* returns NULL!
 *
 * @param[in] pid   the PID of the thread to get the name from
 *
 * @return          the threads name
 * @return          `NULL` if pid is unknown
 */
const char *thread_getname(kernel_pid_t pid);

#ifdef DEVELHELP
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
 * @brief   Get the number of bytes used on the ISR stack
 */
int thread_isr_stack_usage(void);

/**
 * @brief   Get the current ISR stack pointer
 */
void *thread_isr_stack_pointer(void);

/**
 * @brief   Get the start of the ISR stack
 */
void *thread_isr_stack_start(void);

/**
 * @brief Print the current stack to stdout
 */
void thread_stack_print(void);

/**
 * @brief   Prints human readable, ps-like thread information for debugging purposes
 */
void thread_print_stack(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* THREAD_H */

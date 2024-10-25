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
 *  Flags                          | Description
 *  ------------------------------ | --------------------------------------------------
 *  @ref THREAD_CREATE_SLEEPING    | the thread will sleep until woken up manually
 *  @ref THREAD_CREATE_WOUT_YIELD  | the thread might not run immediately after creation
 *  @ref THREAD_CREATE_NO_STACKTEST| never measure the stack's memory usage
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
 *                   THREAD_PRIORITY_MAIN - 1, 0,
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
#include "irq.h"
#include "msg.h"
#include "sched.h"
#include "thread_config.h"

#ifdef MODULE_CORE_THREAD_FLAGS
#include "thread_flags.h"
#endif

#include "thread_arch.h" /* IWYU pragma: export */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Macro definition to inline some of the platform specific
 *        implementations
 *
 * Should be enabled when advantageous by CPU's in their thread_arch.h header
 */
#ifdef THREAD_API_INLINED
#define THREAD_MAYBE_INLINE static inline __attribute__((always_inline))
#else
#define THREAD_MAYBE_INLINE
#endif /* THREAD_API_INLINED */

#if defined(DEVELHELP) && !defined(CONFIG_THREAD_NAMES)
/**
 * @brief   This global macro enable storage of thread names to help developers.
 *
 *          To activate it set environment variable `THREAD_NAMES=1`, or use Kconfig.
 *          It is automatically enabled if `DEVELHELP` is.
 */
#define CONFIG_THREAD_NAMES
#endif

/**
 * @brief Prototype for a thread entry function
 */
typedef void *(*thread_task_func_t)(void *arg);

/**
 * @brief @c thread_t holds thread's context data.
 */
struct _thread {
    char *sp;                       /**< thread's stack pointer         */
    thread_status_t status;         /**< thread's status                */
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
#if defined(DEVELHELP) || IS_ACTIVE(SCHED_TEST_STACK) \
    || defined(MODULE_MPU_STACK_GUARD) || defined(DOXYGEN)
    char *stack_start;              /**< thread's stack start address   */
#endif
#if defined(CONFIG_THREAD_NAMES) || defined(DOXYGEN)
    const char *name;               /**< thread's name                  */
#endif
#if defined(DEVELHELP) || defined(DOXYGEN)
    int stack_size;                 /**< thread's stack size            */
#endif
/* enable TLS only when Picolibc is compiled with TLS enabled */
#ifdef PICOLIBC_TLS
    void *tls;                      /**< thread local storage ptr */
#endif
};

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
 * @brief Never write markers into the thread's stack to measure stack usage
 *
 * This flag is ignored when DEVELHELP or SCHED_TEST_STACK is not enabled
 */
#define THREAD_CREATE_NO_STACKTEST      (8)

/**
 * @brief Legacy flag kept for compatibility.
 *
 * @deprecated will be removed after 2025.07 release
 *
 * This is always enabled with `DEVELHELP=1` or `SCHED_TEST_STACK`.
 */
#define THREAD_CREATE_STACKTEST         (0)
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
                           uint8_t priority,
                           int flags,
                           thread_task_func_t task_func,
                           void *arg,
                           const char *name);

/**
 * @brief       Retrieve a thread control block by PID.
 * @pre         @p pid is valid
 * @param[in]   pid   Thread to retrieve.
 * @return      `NULL` if the PID is invalid or there is no such thread.
 */
static inline thread_t *thread_get_unchecked(kernel_pid_t pid)
{
    return (thread_t *)sched_threads[pid];
}

/**
 * @brief       Retrieve a thread control block by PID.
 * @details     This is a bound-checked variant of accessing `sched_threads[pid]` directly.
 *              If you know that the PID is valid, then don't use this function.
 * @param[in]   pid   Thread to retrieve.
 * @return      `NULL` if the PID is invalid or there is no such thread.
 */
static inline thread_t *thread_get(kernel_pid_t pid)
{
    if (pid_is_valid(pid)) {
        return thread_get_unchecked(pid);
    }
    return NULL;
}

/**
 * @brief Returns the status of a process
 *
 * @param[in] pid   the PID of the thread to get the status from
 *
 * @return          status of the thread
 * @return          `STATUS_NOT_FOUND` if pid is unknown
 */
thread_status_t thread_getstatus(kernel_pid_t pid);

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
#if defined(MODULE_CORE_THREAD) || DOXYGEN
void thread_yield(void);
#else
static inline void thread_yield(void)
{
    /* NO-OP */
}
#endif

/**
 * @brief Arch-specific implementation of @ref thread_yield_higher()
 */
THREAD_MAYBE_INLINE void thread_yield_higher_arch(void);

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
#ifdef DEBUG_THREAD_YIELD_HIGHER
/* A failed assertion will reveal where this was called at the expense of
 * increased binary size. */
#define thread_yield_higher() do {                  \
    assume(irq_is_in() || irq_is_enabled());        \
    thread_yield_higher_arch();                     \
} while (0)

#else
static inline void thread_yield_higher(void)
{
    assume(irq_is_in() || irq_is_enabled());
    thread_yield_higher_arch();
}
#endif

/**
 * @brief   Puts the current thread into zombie state.
 *
 * @details Does nothing when in ISR.
 *          A thread in zombie state will never be scheduled again,
 *          but its scheduler entry and stack will be kept.
 *          A zombie state thread is supposed to be cleaned up
 *          by @ref thread_kill_zombie().
 */
void thread_zombify(void);

/**
 * @brief Terminates zombie thread.
 *
 * @param[in] pid   the PID of the thread to terminate
 *
 * @return          `1` on success
 * @return          `STATUS_NOT_FOUND` if pid is unknown or not a zombie
 */
int thread_kill_zombie(kernel_pid_t pid);

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
 * @brief   Returns a pointer to the Thread Control Block of the currently
 *          running thread
 *
 * @return  Pointer to the TCB of the currently running thread, or `NULL` if
 *          no thread is running
 */
static inline thread_t *thread_get_active(void)
{
    extern volatile thread_t *sched_active_thread;

    return (thread_t *)sched_active_thread;
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
char *thread_stack_init(thread_task_func_t task_func, void *arg,
                        void *stack_start, int stack_size);

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
#if defined(MODULE_CORE_THREAD) || DOXYGEN
const char *thread_getname(kernel_pid_t pid);
#else
static inline const char *thread_getname(kernel_pid_t pid)
{
    (void)pid;
    return "(none)";
}
#endif

/**
 * @brief       Measures the stack usage of a stack
 * @internal    Should not be used externally
 *
 * Only works if the stack is filled with canaries
 * (`*((uintptr_t *)ptr) == (uintptr_t)ptr` for naturally aligned `ptr` within
 * the stack).
 * This is enabled if `DEVELHELP` or `SCHED_TEST_STACK` is set.
 *
 * @param[in] stack     the stack you want to measure. Try
 *                      `thread_get_stackstart(thread_get_active())`
 * @param[in] size      size of @p stack in bytes
 *
 * @return              the amount of unused space of the thread's stack
 */
uintptr_t measure_stack_free_internal(const char *stack, size_t size);

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

/**
 * @brief   Checks if a thread has an initialized message queue
 *
 * @see @ref msg_init_queue()
 *
 * @param[in] thread    The thread to check for
 *
 * @return  `== 0`, if @p thread has no initialized message queue
 * @return  `!= 0`, if @p thread has its message queue initialized
 */
static inline int thread_has_msg_queue(const volatile struct _thread *thread)
{
#if defined(MODULE_CORE_MSG) || defined(DOXYGEN)
    return (thread->msg_array != NULL);
#else
    (void)thread;
    return 0;
#endif
}

/**
 * Get a thread's status
 *
 * @param   thread   thread to work on
 * @returns status of thread
 */
static inline thread_status_t thread_get_status(const thread_t *thread)
{
    return thread->status;
}

/**
 * Get a thread's priority
 *
 * @param   thread   thread to work on
 * @returns priority of thread
 */
static inline uint8_t thread_get_priority(const thread_t *thread)
{
    return thread->priority;
}

/**
 * Returns if a thread is active (currently running or waiting to be scheduled)
 *
 * @param   thread   thread to work on
 * @returns true if thread is active, false otherwise
 */
static inline bool thread_is_active(const thread_t *thread)
{
    return thread->status >= STATUS_ON_RUNQUEUE;
}

/**
 * Convert a thread state code to a human readable string.
 *
 * @param   state   thread state to convert
 * @returns ptr to string representation of thread state (or to "unknown")
 */
const char *thread_state_to_string(thread_status_t state);

/**
 * Get start address (lowest) of a thread's stack.
 *
 * @param   thread thread to work on
 * @returns current stack pointer, or NULL if not available
 */
static inline void *thread_get_stackstart(const thread_t *thread)
{
#if defined(DEVELHELP) || IS_ACTIVE(SCHED_TEST_STACK) \
    || defined(MODULE_MPU_STACK_GUARD)
    return thread->stack_start;
#else
    (void)thread;
    return NULL;
#endif
}

/**
 * Get stored Stack Pointer of thread.
 *
 * *Only provides meaningful value if the thread is not currently running!*.
 *
 * @param   thread thread to work on
 * @returns current stack pointer
 */
static inline void *thread_get_sp(const thread_t *thread)
{
    return thread->sp;
}

/**
 * Get size of a thread's stack.
 *
 * @param   thread thread to work on
 * @returns thread stack size, or 0 if not available
 */
static inline size_t thread_get_stacksize(const thread_t *thread)
{
#if defined(DEVELHELP)
    return thread->stack_size;
#else
    (void)thread;
    return 0;
#endif
}

/**
 * Get PID of thread.
 *
 * This is a simple getter for thread->pid.
 *
 * @param   thread thread to work on
 * @returns thread pid
 */
static inline kernel_pid_t thread_getpid_of(const thread_t *thread)
{
    return thread->pid;
}

/**
 * Get name of thread.
 *
 * @param   thread thread to work on
 * @returns thread name or NULL if not available
 */
static inline const char *thread_get_name(const thread_t *thread)
{
#if defined(CONFIG_THREAD_NAMES)
    return thread->name;
#else
    (void)thread;
    return NULL;
#endif
}

/**
 * @brief       Measures the stack usage of a stack
 *
 * @pre         Only works if the thread was created with the flag
 *              `THREAD_CREATE_STACKTEST`.
 *
 * @param[in] thread    The thread to measure the stack of
 *
 * @return              the amount of unused space of the thread's stack
 */
static inline uintptr_t thread_measure_stack_free(const thread_t *thread)
{
    /* explicitly casting void pointers is bad code style, but needed for C++
     * compatibility */
    return measure_stack_free_internal((const char *)thread_get_stackstart(thread),
                                       thread_get_stacksize(thread));
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* THREAD_H */

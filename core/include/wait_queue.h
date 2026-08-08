/*
 * SPDX-FileCopyrightText: 2025 Mihai Renea <mihai.renea@ml-pa.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    core_wait_queue Wait Queue
 * @ingroup     core
 * @brief       Linux-like wait queue based on thread flags group
 *
 *
 * This API is a thin wrapper around thread flags group and requires
 * "core_thread_flags_group" to be enabled in USEMODULE.
 *
 * Wait queues enable lock-free, IRQ-safe condition signaling. This API is
 * inspired by the Linux Kernel.
 *
 * Wait queues have similar semantics to condition variables, but don't require
 * setting the condition + signaling to be atomic, hence no mutex is required.
 * In turn, one may safely call queue_wake() from an ISR. Note, while
 * cond_signal() and cond_broadcast() are safe to call from an ISR context too,
 * doing so will probably cause a race condition elsewhere. Consider the
 * following scenario using condition variables:
 *
 * @code{c}
 * static uint64_t measurement;
 * mutex_t cond_lock = MUTEX_INIT;
 * cond_t cond = COND_INIT;
 *
 * void measurement_irq(void)
 * {
 *     measurement = measure();
 *     cond_broadcast(&cond);
 * }
 *
 * void wait_for_critical_value(void)
 * {
 *     mutex_lock(&cond_lock);
 *     while (atomic_load_u64(&measurement) < THRESHOLD) {
 *         cond_wait(&cond, cond_lock);
 *     }
 *     mutex_unlock(&cond_lock);
 * }
 * @endcode
 *
 * Note, the mutex is there only for the cond_wait() API call, as we're not
 * allowed to call mutex_lock() inside the ISR. This alone is a hint that
 * something isn't right and indeed, the following sequence of events is
 * possible:
 *  1. thread sees measurement < THRESHOLD, and is about to call cond_wait()
 *  2. ISR fires, sets measurement = THRESHOLD and signals the condition
 *  3. thread calls cond_wait() and goes to sleep, possibly forever
 *
 * Using a wait queue, we can do this:
 *
 * @code{c}
 * static uint64_t measurement;
 * wq_t wq = WAIT_QUEUE_INIT;
 *
 * void measurement_irq(void)
 * {
 *     measurement = measure();
 *     queue_wake_all(&wq);
 * }
 *
 * void wait_for_critical_value(void)
 * {
 *    wait_queue_join(&wq);
 *    while (atomic_load_u64(&measurement) < THRESHOLD) {
 *        queue_wait(&wq);
 *    }
 *    wait_queue_leave(&wq);
 * }
 * @endcode
 *
 * This is free of the race condition above because if the ISR fires between
 * the condition check and the queue_wait() call, queue_wait() will return
 * because the @ref THREAD_FLAG_WAIT_QUEUE thread flag used in the
 * implementation must also have been set. In other words, if the condition is
 * true, then the thread flag is also set.
 *
 * If you have a simple condition check expression, the waiter boiler plate
 * code can be eliminated by using the @ref QUEUE_WAIT() macro, which combines
 * the join, wait and leave operations:
 *
 * @code{c}
 * void wait_for_critical_value(void)
 * {
 *     QUEUE_WAIT(&wq, atomic_load_u64(&measurement) >= THRESHOLD);
 * }
 * @endcode
 *
 * If you don't want to wait indefinitely, you can do it with a timeout:
 *
 * @code{c}
 * void wait_for_critical_value(void)
 * {
 *     if (QUEUE_WAIT_ZTIMER(ZTIMER_MSEC, 10, &wq, atomic_load_u64(&measurement) >= THRESHOLD)) {
 *         puts("condition met!");
 *     }
 *     else {
 *         puts("timeout!");
 *     }
 * }
 * @endcode
 *
 * ### Limitations
 *
 * Be aware that the condition checking is fenced but not atomic w.r. to
 * signaling, so you have to ensure that by other means. E.g. in the example
 * code above this is enforced by @ref atomic_load_u64().
 *
 * ### When to use?
 *
 * If you know for sure you're synchronizing between threads only (no ISR),
 * then the condition variable has the advantage of implicit condition
 * setting/checking atomicity through the mutex. Otherwise go for the wait
 * queue, as it's more flexible by allowing the waker to be in ISR context.
 *
 * @{
 *
 * @file
 * @brief       Linux-like wait queue based on thread flags group
 *
 * @author      Mihai Renea <mihai.renea@ml-pa.com>
 */

#include "thread_flags_group.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Thread flag for signaling a wait queue.
 *
 * Internal use only.
 */
#define THREAD_FLAG_WAIT_QUEUE  (1u << 13)

/**
 * @brief Init value for a wait queue.
 */
#define WAIT_QUEUE_INIT { .group = THREAD_FLAGS_GROUP_INIT }

/**
 * @brief Wait queue struct.
 */
typedef struct {
    thread_flags_group_t group; /**< thread flags group used for signaling */
} wait_queue_t;

/**
 * @brief Join a wait queue.
 *
 * Thin wrapper around @ref thread_flags_group_join().
 *
 * @param[in] wait_queue    wait queue to join
 */
static inline void wait_queue_join(wait_queue_t *wait_queue)
{
    thread_flags_group_join(&wait_queue->group);
}

/**
 * @brief Leave a wait queue.
 *
 * Thin wrapper around @ref thread_flags_group_leave().
 *
 * @param[in] wait_queue    wait queue to leave
 */
static inline void wait_queue_leave(wait_queue_t *wait_queue)
{
    thread_flags_group_leave(&wait_queue->group);
}

/**
 * @brief Wait on a wait queue.
 *
 * Thin wrapper around @ref thread_flags_wait_any(), waiting on @ref
 * THREAD_FLAG_WAIT_QUEUE and @ref THREAD_FLAG_TIMEOUT flags.
 *
 * @param[in] wait_queue    wait queue to wait on
 */
static inline void queue_wait(wait_queue_t *wait_queue)
{
    /* @p wait_queue isn't required because the thread flags are global within
     * a thread, but this is an implementation detail. We keep the queue in the
     * parameter list to explicitly associate a call with the queue passed to
     * @ref queue_wake_all(). This makes code navigation easier, e.g. grepping
     * for the wait queue variable name. */
    (void)wait_queue;
    thread_flags_wait_any(THREAD_FLAG_WAIT_QUEUE | THREAD_FLAG_TIMEOUT);
}

/**
 * @brief Wake all threads waiting on a wait queue.
 *
 * Thin wrapper around @ref thread_flags_group_set().
 *
 * @param[in] wait_queue    wait queue to wake
 */
static inline void queue_wake_all(wait_queue_t *wait_queue)
{
    thread_flags_group_set(&wait_queue->group, THREAD_FLAG_WAIT_QUEUE);
}

/**
 * @brief Convenience macro for waiting for a condition.
 *
 * This macro combines the join, wait and leave operations of the wait queue.
 *
 * @param[in] wq            wait queue to wait on
 * @param[in] cond_expr     condition expression to wait for
 */
#define QUEUE_WAIT(wq, cond_expr)   \
    do {                            \
        wait_queue_join(wq);        \
        while (!(cond_expr)) {      \
            queue_wait(wq);         \
        }                           \
        wait_queue_leave(wq);       \
    } while (0)

#if IS_USED(MODULE_ZTIMER) || DOXYGEN
#  include "ztimer.h"

/**
 * @brief Convenience macro for waiting for a condition with a timeout.
 *
 * This macro combines the join, wait and leave operations of the wait queue
 * and breaks the loop in case of timeout.
 *
 * @param[in] clock         ztimer clock to use
 * @param[in] timeout       timeout in ticks
 * @param[in] wq            wait queue to wait on
 * @param[in] cond_expr     condition expression to wait for
 *
 * @retval true   if the condition was met before the timeout expired
 * @retval false  if the timeout expired
 */
#  define QUEUE_WAIT_ZTIMER(clock, timeout, wq, cond_expr)                  \
    ({                                                                      \
        ztimer_t _wq_timer;                                                 \
        ztimer_set_timeout_flag(clock, &_wq_timer, timeout);                \
        QUEUE_WAIT(wq, (cond_expr) || !ztimer_is_set(clock, &_wq_timer));   \
        ztimer_remove(clock, &_wq_timer);                                   \
    })
#endif /* IS_USED(MODULE_ZTIMER) */

#ifdef __cplusplus
}
#endif
/** @} */

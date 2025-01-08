/*
 * Copyright (C) 2025 Mihai Renea <mihai.renea@ml-pa.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_sync_wait_queue Wait Queue
 * @ingroup     core_sync
 * @brief       Linux-like wait queue for condition signaling
 *
 * Wait queues enable lock-free, IRQ-safe condition signaling. This
 * implementation is inspired from the Linux Kernel.
 *
 * Wait queues have similar semantics to condition variables, but don't require
 * setting the condition + signaling to be atomic, hence no mutex is needed. In
 * turn, one may safely call queue_wake() from an ISR. Note, while cond_signal()
 * and cond_broadcast() are safe to call from an ISR context too, doing so will
 * very probably cause a race condition elsewhere. Consider the following
 * scenario using condition variables:
 *
 * ```
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
 * ```
 *
 * Note, the mutex is there only for the cond_wait() API call, as we're not
 * allowed to call mutex_lock() inside the ISR. This alone is a hint that
 * something isn't right and indeed, the following sequence of events is
 * possible:
 *  1. thread sees measurement < THRESHOLD, and is about to call cond_wait()
 *  2. ISR fires, sets measurement = THRESHOLD, and signals the condition
 *  3. thread calls cond_wait() and goes to sleep, possibly forever
 *
 * Using a wait queue, we can do this:
 *
 * ```
 * static uint64_t measurement;
 * wait_queue_t wq = WAIT_QUEUE_INIT;
 *
 * void measurement_irq(void)
 * {
 *     measurement = measure();
 *     queue_wake(&wq);
 * }
 *
 * void wait_for_critical_value(void)
 * {
 *     QUEUE_WAIT(&wq, atomic_load_u64(&measurement) >= THRESHOLD);
 * }
 * ```
 *
 * This is free of the race condition above because QUEUE_WAIT() is a macro
 * that checks the condition AFTER queueing the current thread to be waken up.
 *
 * When to use?
 *
 * QUEUE_WAIT() is a macro and might come with some additional code size cost
 * due to inlining. If you're not synchronizing with an ISR and care very much
 * about code size then go for condition variables, otherwise there is no
 * reason not to use the wait queue.
 *
 * Can't I just use a mutex?
 *
 * You can abuse a mutex by locking it in a loop in the thread context and
 * unlocking from the ISR context. But this will only work for a single waiter
 * and makes improper use of the mutex semantics.
 *
 * @{
 *
 * @file        wait_queue.h
 * @brief       Linux-like wait queue for condition signaling
 *
 * @author      Mihai Renea <mihai.renea@ml-pa.com>
 */

#ifndef WAIT_QUEUE_H
#define WAIT_QUEUE_H

#include "sched.h"

/* Wait queue entry, which is always allocated on the stack. We can't use the
 * linked list node in thread_t because while evaluating the condition
 * expression the thread may:
 *  - queue on some other wait queue
 *  - block on something else, e.g. a mutex */
typedef struct wait_queue_entry wait_queue_entry_t;
struct wait_queue_entry {
    wait_queue_entry_t *next;
    thread_t *thread;
};

typedef struct {
    wait_queue_entry_t *list;
} wait_queue_t;

#define WAIT_QUEUE_TAIL ((void *)(-1))

/**
 * @brief Init a wait queue
 */
#define WAIT_QUEUE_INIT { .list = WAIT_QUEUE_TAIL }

/**
 * @brief Enable @ref QUEUE_WAIT() early exit optimization if the condition
 *        evaluates true.
 *
 * This optimization is turned off to facilitate the testing of some edge cases.
 * There's no good reason to disable this other than to save a few bytes.
 */
#ifndef CONFIG_QUEUE_WAIT_EARLY_EXIT
#  define CONFIG_QUEUE_WAIT_EARLY_EXIT 1
#endif

#if CONFIG_QUEUE_WAIT_EARLY_EXIT
#  define BREAK_IF_TRUE(cond)                                                  \
    if (cond) {                                                                \
        break;                                                                 \
    }
#else
#  define BREAK_IF_TRUE(cond) (void)(0)
#endif

/* For internal use within the @ref QUEUE_WAIT() macro only. Not the cleanest
 * decomposition, but we want to keep the macro tight. */
void _prepare_to_wait(wait_queue_t *wq, wait_queue_entry_t *entry);
void _maybe_yield_and_enqueue(wait_queue_t *wq, wait_queue_entry_t *entry);
void _wait_dequeue(wait_queue_t *wq, wait_queue_entry_t *entry);

/**
 * @brief Wait for a condition to become true.
 *
 * Will not return for as long as the condition expression @p cond evaluates to
 * false.
 *
 * @note @p cond may get evaluated mutiple times.
 *
 * @note The interrupt state at the moment of calling this macro will be
 *       restored before executing the condition expression and before
 *       returning, but interrupts MAY get enabled if the condition evaluates
 *       false, as the thread MAY have to go to sleep.
 *
 * @warning @p cond is NOT executed atomically. If that is a requirement, you
 *          can:
 *           - lock within the expression, e.g. lock a mutex, disable interrupts.
 *           - call this with interrupts disabled. Interrupts will be kept
 *             disabled during the condition evaluation.
 *
 * @param[in] wq    wait queue to wait on
 * @param[in] cond  condition expression to be evaluated
 */
#define QUEUE_WAIT(wq, cond)                                                   \
    do {                                                                       \
        BREAK_IF_TRUE(cond);                                                   \
                                                                               \
        wait_queue_entry_t me;                                                 \
        _prepare_to_wait(wq, &me);                                             \
        while (!(cond)) {                                                      \
            _maybe_yield_and_enqueue(wq, &me);                                 \
        }                                                                      \
        _wait_dequeue(wq, &me);                                                \
    } while (0)

/* For internal use only. */
void _queue_wake_common(wait_queue_t *wq, bool all);

/**
 * @brief Wake one thread queued on the wait queue.
 *
 * @param wq wait queue to signal
 */
static inline void queue_wake_exclusive(wait_queue_t *wq)
{
    _queue_wake_common(wq, false);
}

/**
 * @brief Wake all threads queued on the wait queue.
 *
 * @param wq wait queue to signal
 */
static inline void queue_wake(wait_queue_t *wq)
{
    _queue_wake_common(wq, true);
}

#endif /* WAIT_QUEUE_H */
/** @} */

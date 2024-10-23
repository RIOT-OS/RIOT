/*
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
 * setting the condition + signaling to be atomic, hence no mutex is needed to
 * function properly. In turn, one may safely call queue_wake() from an ISR.
 * Note, while cond_signal() and cond_broadcast() are safe to call from an ISR
 * context too, doing so will very probably cause a race condition elsewhere.
 * Consider the following scenario using condition variables:
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
 *  2. ISR fires, sets measurement = THRESHOLD
 *  3. thread calls cond_wait() and goes to sleep, possibly forever
 *
 * The above example can be fixed by using a initially-locked mutex instead
 * of a condition variable:
 *
 * ```
 * static uint64_t measurement;
 * mutex_t cond_mutex = MUTEX_INIT_LOCKED;
 *
 * void measurement_irq(void)
 * {
 *     measurement = measure();
 *     mutex_unlock(&cond_mutex);
 * }
 *
 * void wait_for_critical_value(void)
 * {
 *     while (atomic_load_u64(&measurement) < THRESHOLD) {
 *         mutex_lock(&cond_mutex);
 *     }
 *     mutex_unlock(&cond_mutex);
 * }
 * ```
 *
 * The mutex_unlock() at the end of wait_for_critical_value() is there to wake
 * up any other waiters that might be queued, as mutex_unlock() signals only
 * one. At this point it's pretty obvious that we're abusing the mutex
 * semantics.
 *
 * Using a wait queue, we can do this:
 *
 * ```
 * static uint8_t measurement;
 * mutex_t wq = WAIT_QUEUE_INIT;
 *
 * void measurement_irq(void)
 * {
 *     measurement = measure();
 *     queue_broadcast(&wq);
 * }
 *
 * void wait_for_critical_value(void)
 * {
 *     queue_wait(&wq, atomic_load_u64(&measurement) >= THRESHOLD);
 * }
 * ```
 *
 * This is free of the race condition above because queue_wait() is a macro
 * that checks the condition AFTER queueing the current thread to be waken up:
 *
 * ```
 * queue_wait(wq, cond):
 *     loop:
 *        enqueue_current_thread(wq)
 *        if (cond):
 *            break
 *
 *        thread_yield()
 * ```
 *
 * This way, if cond evaluates to false then we know that we could not have
 * possibly missed the wake-up event, as the wake-up event is triggered AFTER
 * setting the condition true, which we just checked.
 *
 * Limitations
 *
 * There are two main drawbacks when using the wait queue:
 *  1. Setting/checking the wake-up condition is not atomic. While the condition
 *     variable semantics force the use of a mutex, in the case of wait queues
 *     the locking is left to the user. This is why int the example above the
 *     condition is checked with atomic_load_u64().
 *  2. queue_wait() is a macro and comes with the additional code size cost of
 *     inlining.
 *
 * So when to use?
 *
 * If you're not synchronizing with an ISR then go for condition variables as
 * they're enforcing atomic condition setting/checking through their semantics
 * and should produce smaller code. Wait queues only make sense if the signaler
 * (or one of multiple) is in ISR context.
 *
 * @{
 *
 * @file
 * @brief       Linux-like wait queue for condition signaling
 *
 * @author      Mihai Renea <mihairenea@gmail.com>
 */

#ifndef WAIT_QUEUE_H
#define WAIT_QUEUE_H

#include "cond.h"
#include "irq.h"

typedef struct {
    cond_t cond;
} wait_queue_t;

#define WAIT_QUEUE_INIT { .cond = COND_INIT }

static inline void _wait_enqueue(wait_queue_t *wq, thread_t *me)
{
    irq_disable();
    sched_set_status(me, STATUS_COND_BLOCKED);
    thread_add_to_list(&wq->cond.queue, me);
    irq_enable();
}

static inline void _wait_dequeue(wait_queue_t *wq, thread_t *me)
{
    irq_disable();
    list_remove(&wq->cond.queue, &me->rq_entry);
    sched_set_status(me, STATUS_RUNNING);
    irq_enable();
}

#define queue_wait(wq, cond) do {                   \
    if (cond) {                                     \
        break;                                      \
    }                                               \
    thread_t *me = thread_get_active();             \
    while (1) {                                     \
        _wait_enqueue(wq, me);                      \
        if (cond) {                                 \
            break;                                  \
        }                                           \
        thread_yield_higher();                      \
    }                                               \
    _wait_dequeue(wq, me);                          \
} while (0)

static inline void queue_wake(wait_queue_t *wq)
{
    cond_signal(&wq->cond);
}

static inline void queue_broadcast(wait_queue_t *wq)
{
    cond_broadcast(&wq->cond);
}

#endif /* WAIT_QUEUE_H */
/** @} */

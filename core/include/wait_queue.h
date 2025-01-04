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
 * Using a wait queue, we can do this:
 *
 * ```
 * static uint8_t measurement;
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
 * This way, if the condition expression evaluates to false then we know that we
 * could not have possibly missed the wake-up event, as the wake-up event is
 * triggered AFTER setting the condition true, which we just checked.
 *
 * When to use?
 *
 * queue_wait() is a macro and comes with the additional code size cost of
 * inlining. If you're not synchronizing with an ISR then go for condition
 * variables. Wait queues only make sense if the signaler (or one of multiple)
 * might be in ISR context.
 *
 * @{
 *
 * @file        wait_queue.h
 * @brief       Linux-like wait queue for condition signaling
 *
 * @author      Mihai Renea <mihairenea@gmail.com>
 */

#ifndef WAIT_QUEUE_H
#define WAIT_QUEUE_H

#include "irq.h"
#include "thread.h"

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

/* For internal use only */
void _wait_enqueue(wait_queue_t *wq, wait_queue_entry_t *entry, int irq_state);
void _wait_dequeue(wait_queue_t *wq, wait_queue_entry_t *entry, int irq_state);
void _maybe_yield(wait_queue_entry_t *entry);
void _queue_wake_common(wait_queue_t *wq, bool all);

/**
 * @brief Wait for a condition to become true.
 *
 * @note
 *
 * @param[in] wq    wait queue to wait on
 * @param[in] cond  condition expression to be evaluated
 */
#define queue_wait(wq, cond)                                                   \
  do {                                                                         \
    if (cond) {                                                                \
      break;                                                                   \
    }                                                                          \
                                                                               \
    wait_queue_entry_t me = {.thread = thread_get_active()};                   \
    int irq_state = irq_disable();                                             \
    _wait_enqueue(wq, &me, irq_state);                                         \
    while (!(cond)) {                                                          \
      _maybe_yield(&me);                                                       \
      _wait_enqueue(wq, &me, irq_state);                                       \
    }                                                                          \
                                                                               \
    _wait_dequeue(wq, &me, irq_state);                                         \
  } while (0)

static inline void queue_wake_exclusive(wait_queue_t *wq)
{
    _queue_wake_common(wq, false);
}

static inline void queue_wake(wait_queue_t *wq)
{
    _queue_wake_common(wq, true);
}

#endif /* WAIT_QUEUE_H */
/** @} */

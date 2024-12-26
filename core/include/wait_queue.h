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

#include "irq.h"
#include "sched.h"
#include "thread.h"

#define ENABLE_DEBUG 1
#include "debug.h"

typedef struct wait_queue_entry wait_queue_entry_t;
struct wait_queue_entry {
    wait_queue_entry_t *next;
    thread_t *thread;
};

typedef struct {
    wait_queue_entry_t *list;
} wait_queue_t;

#define WAIT_QUEUE_TAIL ((void *)(-1))

#define WAIT_QUEUE_INIT { .list = WAIT_QUEUE_TAIL }

static inline bool _is_in_wq(wait_queue_entry_t *entry)
{
    return entry->next != NULL;
}

static inline void _wq_add_to_list(wait_queue_t *wq, wait_queue_entry_t *entry)
{
    if (_is_in_wq(entry)) {
        DEBUG("wq: thread %d already queued\n", entry->thread->pid);
        /* This can happen if the condition expression blocks. The thread
         * is then waken up, so thread_yield_higher() in QUEUE_WAIT() will
         * return immediately. The thread is then added back into the wait
         * queue, although it was never removed. */
        return;
    }

    wait_queue_entry_t **curr_pp = &wq->list;
    while ((*curr_pp != WAIT_QUEUE_TAIL) &&
           (*curr_pp)->thread->priority <= entry->thread->priority) {
        curr_pp = &(*curr_pp)->next;
    }

    entry->next = *curr_pp;
    *curr_pp = entry;
}

static inline void _wq_del_from_list(wait_queue_t *wq, wait_queue_entry_t *entry)
{
    wait_queue_entry_t **curr_pp = &wq->list;
    while (*curr_pp != WAIT_QUEUE_TAIL) {
        if (*curr_pp == entry) {
            *curr_pp = (*curr_pp)->next;
            entry->next = NULL;
            break;
        }
        curr_pp = &(*curr_pp)->next;
    }
}

static inline void _wait_enqueue(wait_queue_t *wq, wait_queue_entry_t *entry, int irq_state)
{
    _wq_add_to_list(wq, entry);
    irq_restore(irq_state);
}

static inline void _wait_dequeue(wait_queue_t *wq, wait_queue_entry_t *entry, int irq_state)
{
    irq_disable();
    _wq_del_from_list(wq, entry);
    irq_restore(irq_state);
}

static inline void _maybe_yield(wait_queue_entry_t *entry)
{
    irq_disable();
    if (!_is_in_wq(entry)) {
        /* In the rare case the thread will get woken up just between the
         * condition check and yielding, don't go to sleep but do another
         * condition check. */
        return;
    }

    sched_set_status(entry->thread, STATUS_WQ_BLOCKED);

    irq_enable();
    thread_yield_higher();
    irq_disable();
}

#define queue_wait(wq, cond)                                                   \
  do {                                                                         \
    if (cond) {                                                                \
      break;                                                                   \
    }                                                                          \
    wait_queue_entry_t me = {.thread = thread_get_active()};                   \
    int irq_state = irq_disable();                                             \
    _wait_enqueue(wq, &me, irq_state);                                         \
    while (!(cond)) {                                                          \
      _maybe_yield(&me);                                                        \
      _wait_enqueue(wq, &me, irq_state);                                       \
    }                                                                          \
    _wait_dequeue(wq, &me, irq_state);                                         \
  } while (0)

static inline void _queue_wake_common(wait_queue_t *wq, bool const all)
{
    int irq_state = irq_disable();
    wait_queue_entry_t *head;
    while ((head = wq->list) != WAIT_QUEUE_TAIL) {
        /* Wake the thread only if it blocks on the queue. Otherwise it blocks
         * on something else during the condition check, or is already running.
         * We remove the thread from the wait queue but we don't wake it up,
         * as this would conflict with the current blocking. Once the thread
         * leaves the condition check, it will exit the wait loop. */
        if (head->thread->status == STATUS_WQ_BLOCKED) {
            sched_set_status(head->thread, STATUS_PENDING);
            DEBUG("wq: woke up thread %d\n", head->thread->pid);
        }
        else {
            DEBUG("wq: won't wake thread %d in state `%s`\n",
                  head->thread->pid,
                  thread_state_to_string(head->thread->status));

        }

        wq->list = head->next;
        head->next = NULL;

        if (!all) {
            break;
        }
    }
    irq_restore(irq_state);
}

static inline void queue_wake(wait_queue_t *wq)
{
    _queue_wake_common(wq, false);
}

static inline void queue_broadcast(wait_queue_t *wq)
{
    _queue_wake_common(wq, true);
}

#endif /* WAIT_QUEUE_H */
/** @} */

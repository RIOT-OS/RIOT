#include "wait_queue.h"

#define ENABLE_DEBUG 1
#include "debug.h"

static inline bool _is_in_wq(wait_queue_entry_t *entry)
{
    /* A queued entry will either point to the next or to QUEUE_WAIT_TAIL. */
    return entry->next != NULL;
}

/* @pre  interrupts disabled, de-queued
 * @post interrupts restored to @p irq_state */
static void _wait_enqueue(wait_queue_t *wq, wait_queue_entry_t *entry, int irq_state)
{
    assert(!_is_in_wq(entry));

    wait_queue_entry_t **curr_pp = &wq->list;
    while ((*curr_pp != WAIT_QUEUE_TAIL) &&
           (*curr_pp)->thread->priority <= entry->thread->priority) {
        curr_pp = &(*curr_pp)->next;
    }

    entry->next = *curr_pp;
    *curr_pp = entry;

    irq_restore(irq_state);
}

void _prepare_to_wait(wait_queue_t *wq, wait_queue_entry_t *entry)
{
    int irq_state = irq_disable();

    *entry = (wait_queue_entry_t) {
        .thread = thread_get_active(),
        .next   = NULL,
    };

    _wait_enqueue(wq, entry, irq_state);
}

void _wait_dequeue(wait_queue_t *wq, wait_queue_entry_t *entry)
{
    int irq_state = irq_disable();

    wait_queue_entry_t **curr_pp = &wq->list;
    while (*curr_pp != WAIT_QUEUE_TAIL) {
        if (*curr_pp == entry) {
            *curr_pp = (*curr_pp)->next;
            /* mark as not queued */
            entry->next = NULL;
            break;
        }
        curr_pp = &(*curr_pp)->next;
    }

    irq_restore(irq_state);
}

void _maybe_yield_and_enqueue(wait_queue_t *wq, wait_queue_entry_t *entry)
{
    int irq_state = irq_disable();
    if (!_is_in_wq(entry)) {
        /* Queue got signaled while evaluating the condition expression. Don't
         * go to sleep but re-evaluate the condition. */
        _wait_enqueue(wq, entry, irq_state);
        return;
    }

    sched_set_status(entry->thread, STATUS_WQ_BLOCKED);
    /* _queue_wake_common(wq) can't tell whether the thread is sleeping on wq
     * or on some other queue, which is possible during the condition expression
     * evaluation. But we can "mark" the thread with the queue it's actually
     * sleeping on by having the thread's linked list slot (which we don't use
     * otherwise) point back to this entry. */
    entry->thread->rq_entry.next = (void *)entry;

    irq_enable();
    thread_yield_higher();
    irq_disable();

    _wait_enqueue(wq, entry, irq_state);
}

void _queue_wake_common(wait_queue_t *wq, bool all)
{
    int irq_state = irq_disable();

    int waiter_prio = THREAD_PRIORITY_MIN + 1;

    wait_queue_entry_t *head;
    while ((head = wq->list) != WAIT_QUEUE_TAIL) {
        thread_t *thread = head->thread;
        /* Wake the thread only if it blocks on THIS queue, otherwise:
         *  - it is already on the run queue, in which case there is nothing
         *    to be done, or
         *  - it blocks on something else (e.g. a mutex) while evaluating the
         *    condition expression, in which case we may not wake it up, as
         *    RIOTs locking primitives don't expect spurious wake-ups. This can
         *    also be another wait queue, which is why we check if the thread
         *    points back to this wait-queue entry (see _maybe_yield()) */
        if (thread->status == STATUS_WQ_BLOCKED &&
            (wait_queue_entry_t *)thread->rq_entry.next == head) {
            sched_set_status(thread, STATUS_PENDING);
            if (waiter_prio == THREAD_PRIORITY_MIN + 1) {
                /* First thread to be waken up. We don't care about the
                 * priorities of subsequent threads - they must be equal or
                 * lower. */
                waiter_prio = thread->priority;
            }

            DEBUG("wq: woke up thread %d\n", head->thread->pid);
        }
        else {
            DEBUG("wq: won't wake thread %d in state `%s`\n",
                  head->thread->pid,
                  thread_state_to_string(head->thread->status));

        }
        /* We remove the thread from the wait queue in all cases s.t.
         * _maybe_yield() sees the state change and forces another condition
         * check instead of going to sleep. */
        wq->list = head->next;
        head->next = NULL;

        if (!all) {
            break;
        }
    }

    irq_enable();

    sched_switch(waiter_prio);

    irq_restore(irq_state);
}

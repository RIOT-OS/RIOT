#include "wait_queue.h"

static inline bool _is_in_wq(wait_queue_entry_t *entry)
{
    return entry->next != NULL;
}

/* @pre  interrupts disabled
 * @post interrupts restored to @p irq_state */
void _wait_enqueue(wait_queue_t *wq, wait_queue_entry_t *entry, int irq_state)
{
    if (_is_in_wq(entry)) {
        /* This can happen for nested wait queues, i.e. the condition
         * expression for a wait queue blocks on some other wait queue. We are
         * now enqueuing for the inner queue while executing the condition
         * expression for the outer wait queue, because that outer wait queue
         * got signaled. We got woken up "by mistake", as _queue_wake_common()
         * sees STATUS_WQ_BLOCKED but doesn't know on which queue the thread is
         * blocked. But this is fine, as this will only force another round of
         * condition expression check. */
        irq_restore(irq_state);
        return;
    }

    wait_queue_entry_t **curr_pp = &wq->list;
    while ((*curr_pp != WAIT_QUEUE_TAIL) &&
           (*curr_pp)->thread->priority <= entry->thread->priority) {
        curr_pp = &(*curr_pp)->next;
    }

    entry->next = *curr_pp;
    *curr_pp = entry;

    irq_restore(irq_state);
}

/* @pre  interrupts disabled
 * @post interrupts restored to @p irq_state */
void _wait_dequeue(wait_queue_t *wq, wait_queue_entry_t *entry, int irq_state)
{
    irq_disable();

    wait_queue_entry_t **curr_pp = &wq->list;
    while (*curr_pp != WAIT_QUEUE_TAIL) {
        if (*curr_pp == entry) {
            *curr_pp = (*curr_pp)->next;
            entry->next = NULL;
            break;
        }
        curr_pp = &(*curr_pp)->next;
    }

    irq_restore(irq_state);
}

void _maybe_yield(wait_queue_entry_t *entry)
{
    irq_disable();
    if (!_is_in_wq(entry)) {
        /* Queue got signaled while evaluating the condition expression or right
         * afterwards. Don't go to sleep but do another condition check. */
        return;
    }

    sched_set_status(entry->thread, STATUS_WQ_BLOCKED);

    irq_enable();
    thread_yield_higher();
    irq_disable();
}

void _queue_wake_common(wait_queue_t *wq, bool all)
{
    int irq_state = irq_disable();

    wait_queue_entry_t *head;
    while ((head = wq->list) != WAIT_QUEUE_TAIL) {
        /* Wake the thread only if it blocks on some queue, otherwise:
         *  - it is already on the run queue, in which case there is nothing
         *    to be done, or
         *  - it blocks on something else (e.g. a mutex) while evaluating the
         *    condition expression, in which case we may not wake it up, as
         *    RIOT's locking primitives don't expect spurious wake-ups. */
        if (head->thread->status == STATUS_WQ_BLOCKED) {
            /* It is possible that the thread is blocked on some queue other
             * than the one we're signaling right now. This can happen when
             * the condition expression itself blocks on a wait queue. But
             * this is fine and will only force another condition check. */
            sched_set_status(head->thread, STATUS_PENDING);
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

    irq_restore(irq_state);
}

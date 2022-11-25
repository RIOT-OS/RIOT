/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_thread
 * @{
 *
 * @file
 * @brief       thread flags implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "thread_flags.h"
#include "irq.h"
#include "list.h"
#include "sched.h"
#include "thread.h"

#define ENABLE_DEBUG 0
#include "debug.h"

struct flags_any_or_mbox_data {
    mbox_t *mbox;
    thread_flags_t flags;
};

static inline int __attribute__((always_inline)) _thread_flags_wake(
    thread_t *thread)
{
    unsigned wakeup;
    thread_flags_t mask = (uint16_t)(unsigned)thread->wait_data;

    switch (thread->status) {
    case STATUS_FLAG_BLOCKED_ANY:
        wakeup = (thread->flags & mask);
        break;
    case STATUS_FLAG_BLOCKED_ALL:
        wakeup = ((thread->flags & mask) == mask);
        break;
#ifdef MODULE_CORE_MBOX
    case STATUS_FLAG_MBOX_BLOCKED:
    {
        struct flags_any_or_mbox_data *data = thread->wait_data;
        wakeup = thread->flags & data->flags;
        if (wakeup) {
            list_remove(&data->mbox->readers,
                        (list_node_t *)&thread->rq_entry);
        }
        break;
    }
#endif
    default:
        wakeup = 0;
        break;
    }

    if (wakeup) {
        DEBUG("_thread_flags_wake(): waking up pid %" PRIkernel_pid "\n",
              thread->pid);
        sched_set_status(thread, STATUS_PENDING);
        sched_context_switch_request = 1;
    }

    return wakeup;
}

int thread_flags_wake(thread_t *thread)
{
    return _thread_flags_wake(thread);
}

static thread_flags_t _thread_flags_clear_atomic(thread_t *thread,
                                                 thread_flags_t mask)
{
    unsigned state = irq_disable();

    mask &= thread->flags;
    thread->flags &= ~mask;
    irq_restore(state);
    return mask;
}

static void _thread_flags_wait(thread_flags_t mask, thread_t *thread,
                               unsigned threadstate, unsigned irqstate)
{
    DEBUG(
        "_thread_flags_wait: me->flags=0x%08x me->mask=0x%08x. going blocked.\n",
        (unsigned)thread->flags, (unsigned)mask);

    thread->wait_data = (void *)(unsigned)mask;
    sched_set_status(thread, threadstate);
    irq_restore(irqstate);
    thread_yield_higher();
}

thread_flags_t thread_flags_clear(thread_flags_t mask)
{
    thread_t *me = thread_get_active();

    mask = _thread_flags_clear_atomic(me, mask);
    DEBUG("thread_flags_clear(): pid %" PRIkernel_pid " clearing 0x%08x\n",
          thread_getpid(), mask);
    return mask;
}

static void _thread_flags_wait_any(thread_flags_t mask)
{
    thread_t *me = thread_get_active();
    unsigned state = irq_disable();

    if (!(me->flags & mask)) {
        _thread_flags_wait(mask, me, STATUS_FLAG_BLOCKED_ANY, state);
    }
    else {
        irq_restore(state);
    }
}

thread_flags_t thread_flags_wait_any(thread_flags_t mask)
{
    thread_t *me = thread_get_active();

    _thread_flags_wait_any(mask);
    return _thread_flags_clear_atomic(me, mask);
}

thread_flags_t thread_flags_wait_one(thread_flags_t mask)
{
    _thread_flags_wait_any(mask);
    thread_t *me = thread_get_active();
    thread_flags_t tmp = me->flags & mask;

    /* clear all but least significant bit */
    tmp &= (~tmp + 1);
    return _thread_flags_clear_atomic(me, tmp);
}

thread_flags_t thread_flags_wait_all(thread_flags_t mask)
{
    unsigned state = irq_disable();
    thread_t *me = thread_get_active();

    if (!((me->flags & mask) == mask)) {
        DEBUG(
            "thread_flags_wait_all(): pid %" PRIkernel_pid " waiting for %08x\n",
            thread_getpid(), (unsigned)mask);
        _thread_flags_wait(mask, me, STATUS_FLAG_BLOCKED_ALL, state);
    }
    else {
        irq_restore(state);
    }

    return _thread_flags_clear_atomic(me, mask);
}

void thread_flags_set(thread_t *thread, thread_flags_t mask)
{
    DEBUG("thread_flags_set(): setting 0x%08x for pid %" PRIkernel_pid "\n",
          mask, thread->pid);
    unsigned state = irq_disable();

    thread->flags |= mask;
    if (_thread_flags_wake(thread)) {
        irq_restore(state);
        thread_yield_higher();
    }
    else {
        irq_restore(state);
    }
}

#ifdef MODULE_CORE_MBOX
thread_flags_t thread_flags_wait_any_or_mbox(mbox_t *mbox, msg_t *msg,
                                             thread_flags_t mask)
{
    thread_t *me = thread_get_active();
    thread_flags_t flags;

    struct flags_any_or_mbox_data wait_data = {
        .mbox = mbox,
        .flags = mask | THREAD_FLAG_MBOX_READY,
    };

    while (1) {
        if (mbox_try_get(mbox, msg)) {
            thread_flags_clear(THREAD_FLAG_MBOX_READY);
            return 0;
        }

        flags = _thread_flags_clear_atomic(me, mask);
        if (flags) {
            return flags;
        }

        unsigned irqstate = irq_disable();
        me->wait_data = &wait_data;
        sched_set_status(me, STATUS_FLAG_MBOX_BLOCKED);
        thread_add_to_list(&mbox->readers, me);
        irq_restore(irqstate);
        thread_yield_higher();
        irqstate = irq_disable();
        sched_set_status(me, STATUS_RUNNING);
        irq_restore(irqstate);
    }
}
#endif

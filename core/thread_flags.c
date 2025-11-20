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
#include "thread.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static inline int __attribute__((always_inline)) _thread_flags_wake(
    thread_t *thread)
{
    unsigned wakeup;
    thread_flags_t mask = (uint16_t)(uintptr_t)thread->wait_data;

    switch (thread->status) {
    case STATUS_FLAG_BLOCKED_ANY:
        wakeup = (thread->flags & mask);
        break;
    case STATUS_FLAG_BLOCKED_ALL:
        wakeup = ((thread->flags & mask) == mask);
        break;
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

    thread->wait_data = (void *)(uintptr_t)mask;
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

bool thread_flags_set_internal(thread_t *thread, thread_flags_t mask)
{
    thread->flags |= mask;
    return _thread_flags_wake(thread);
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

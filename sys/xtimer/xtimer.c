/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_xtimer
 *
 * @{
 * @file
 * @brief xtimer convenience functionality
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @author Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "xtimer.h"
#include "mutex.h"
#include "thread.h"
#include "irq.h"
#include "div.h"
#include "list.h"

#include "timex.h"

#ifdef MODULE_CORE_THREAD_FLAGS
#include "thread_flags.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

/*
 * @brief: struct for mutex lock with timeout
 * xtimer_mutex_lock_timeout() uses it to give information to the timer callback function
 */
typedef struct {
    mutex_t *mutex;
    thread_t *thread;
    volatile int timeout;
} mutex_thread_t;

static void _callback_unlock_mutex(void* arg)
{
    mutex_t *mutex = (mutex_t *) arg;
    mutex_unlock(mutex);
}

void _xtimer_tsleep(uint32_t offset, uint32_t long_offset)
{
    if (irq_is_in()) {
        assert(!long_offset);
        _xtimer_spin(offset);
        return;
    }

    xtimer_t timer;
    mutex_t mutex = MUTEX_INIT;

    timer.callback = _callback_unlock_mutex;
    timer.arg = (void*) &mutex;

    mutex_lock(&mutex);
    _xtimer_set64(&timer, offset, long_offset);
    mutex_lock(&mutex);
}

void _xtimer_periodic_wakeup(uint32_t *last_wakeup, uint32_t period) {
    xtimer_t timer;
    mutex_t mutex = MUTEX_INIT;

    timer.callback = _callback_unlock_mutex;
    timer.arg = (void*) &mutex;

    /* time sensitive until setting offset */
    unsigned int state = irq_disable();
    uint32_t now = _xtimer_now();
    uint32_t elapsed = now - (*last_wakeup);
    uint32_t offset = (*last_wakeup) + period - now;
    irq_restore(state);

    if (elapsed >= period) {
        /* timer should be fired right now (some time drift might happen) */
        *last_wakeup = now;
        return;
    }

    mutex_lock(&mutex);
    _xtimer_set64(&timer, offset, 0);
    mutex_lock(&mutex);

    *last_wakeup = now + offset;
}

#ifdef MODULE_CORE_MSG
/* Waits for incoming message or timeout flag. */
static int _msg_wait(msg_t *m, xtimer_t *t)
{
    int res;

    thread_flags_t flags =
        thread_flags_wait_any(THREAD_FLAG_TIMEOUT | THREAD_FLAG_MSG_WAITING);

    if (flags & THREAD_FLAG_MSG_WAITING) {
        res = 0;
        msg_receive(m);
    }
    else {
        res = -1;
    }

    if (! (flags & THREAD_FLAG_TIMEOUT)) {
        xtimer_remove(t);
    }

    return res;
}

int _xtimer_msg_receive_timeout64(msg_t *m, uint64_t timeout_us) {
    xtimer_t t;
    xtimer_set_timeout_flag64(&t, timeout_us);
    return _msg_wait(m, &t);
}

int _xtimer_msg_receive_timeout(msg_t *m, uint32_t timeout_us)
{
    xtimer_t t;
    xtimer_set_timeout_flag(&t, timeout_us);
    return _msg_wait(m, &t);
}
#endif /* MODULE_CORE_MSG */

static void _callback_wakeup(void* arg)
{
    thread_wakeup((kernel_pid_t)((intptr_t)arg));
}

void _xtimer_set_wakeup(xtimer_t *timer, uint32_t offset, kernel_pid_t pid)
{
    timer->callback = _callback_wakeup;
    timer->arg = (void*) ((intptr_t)pid);

    _xtimer_set64(timer, offset, 0);
}

void _xtimer_set_wakeup64(xtimer_t *timer, uint64_t offset, kernel_pid_t pid)
{
    timer->callback = _callback_wakeup;
    timer->arg = (void*) ((intptr_t)pid);

    _xtimer_set64(timer, offset, offset >> 32);
}

void xtimer_now_timex(timex_t *out)
{
    uint64_t now = xtimer_usec_from_ticks64(xtimer_now64());

    out->seconds = div_u64_by_1000000(now);
    out->microseconds = now - (out->seconds * US_PER_SEC);
}

static void _mutex_timeout(void *arg)
{
    /* interrupts a disabled because xtimer can spin
     * if xtimer_set spins the callback is executed
     * in the thread context
     *
     * If the xtimer spin is fixed in the future
     * interups disable/restore can be removed
     */
    unsigned int irqstate = irq_disable();

    mutex_thread_t *mt = (mutex_thread_t *)arg;

    if (mt->mutex->queue.next != MUTEX_LOCKED &&
        mt->mutex->queue.next != NULL) {
        mt->timeout = 1;
        list_node_t *node = list_remove(&mt->mutex->queue,
                                        (list_node_t *)&mt->thread->rq_entry);

        /* if thread was removed from the list */
        if (node != NULL) {
            if (mt->mutex->queue.next == NULL) {
                mt->mutex->queue.next = MUTEX_LOCKED;
            }
            sched_set_status(mt->thread, STATUS_PENDING);
            irq_restore(irqstate);
            sched_switch(mt->thread->priority);
            return;
        }
    }
    irq_restore(irqstate);
}

int xtimer_mutex_lock_timeout(mutex_t *mutex, uint64_t timeout)
{
    xtimer_t t;
    mutex_thread_t mt = { mutex, (thread_t *)sched_active_thread, 0 };

    if (timeout != 0) {
        t.callback = _mutex_timeout;
        t.arg = (void *)((mutex_thread_t *)&mt);
        xtimer_set64(&t, timeout);
    }

    mutex_lock(mutex);
    xtimer_remove(&t);
    return -mt.timeout;
}

#ifdef MODULE_CORE_THREAD_FLAGS
static void _set_timeout_flag_callback(void* arg)
{
    thread_flags_set(arg, THREAD_FLAG_TIMEOUT);
}

static void _set_timeout_flag_prepare(xtimer_t *t)
{
    t->callback = _set_timeout_flag_callback;
    t->arg = (thread_t *)sched_active_thread;
    thread_flags_clear(THREAD_FLAG_TIMEOUT);
}

void xtimer_set_timeout_flag(xtimer_t *t, uint32_t timeout)
{
    _set_timeout_flag_prepare(t);
    xtimer_set(t, timeout);
}

void xtimer_set_timeout_flag64(xtimer_t *t, uint64_t timeout)
{
    _set_timeout_flag_prepare(t);
    xtimer_set64(t, timeout);
}
#endif

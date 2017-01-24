/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup xtimer
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

#define ENABLE_DEBUG 0
#include "debug.h"

typedef struct {
    mutex_t *mutex;
    thread_t *thread;
    int timeout;
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
    timer.target = timer.long_target = 0;

    mutex_lock(&mutex);
    _xtimer_set64(&timer, offset, long_offset);
    mutex_lock(&mutex);
}

void _xtimer_periodic_wakeup(uint32_t *last_wakeup, uint32_t period) {
    xtimer_t timer;
    mutex_t mutex = MUTEX_INIT;

    timer.callback = _callback_unlock_mutex;
    timer.arg = (void*) &mutex;

    uint32_t target = (*last_wakeup) + period;
    uint32_t now = _xtimer_now();
    /* make sure we're not setting a value in the past */
    if (now < (*last_wakeup)) {
        /* base timer overflowed between last_wakeup and now */
        if (!((now < target) && (target < (*last_wakeup)))) {
            /* target time has already passed */
            goto out;
        }
    }
    else {
        /* base timer did not overflow */
        if ((((*last_wakeup) <= target) && (target <= now))) {
            /* target time has already passed */
            goto out;
        }
    }

    /*
     * For large offsets, set an absolute target time.
     * As that might cause an underflow, for small offsets, set a relative
     * target time.
     * For very small offsets, spin.
     */
    /*
     * Note: last_wakeup _must never_ specify a time in the future after
     * _xtimer_periodic_sleep returns.
     * If this happens, last_wakeup may specify a time in the future when the
     * next call to _xtimer_periodic_sleep is made, which in turn will trigger
     * the overflow logic above and make the next timer fire too early, causing
     * last_wakeup to point even further into the future, leading to a chain
     * reaction.
     *
     * tl;dr Don't return too early!
     */
    uint32_t offset = target - now;
    DEBUG("xps, now: %9" PRIu32 ", tgt: %9" PRIu32 ", off: %9" PRIu32 "\n", now, target, offset);
    if (offset < XTIMER_PERIODIC_SPIN) {
        _xtimer_spin(offset);
    }
    else {
        if (offset < XTIMER_PERIODIC_RELATIVE) {
            /* NB: This will overshoot the target by the amount of time it took
             * to get here from the beginning of xtimer_periodic_wakeup()
             *
             * Since interrupts are normally enabled inside this function, this time may
             * be undeterministic. */
            target = _xtimer_now() + offset;
        }
        mutex_lock(&mutex);
        DEBUG("xps, abs: %" PRIu32 "\n", target);
        _xtimer_set_absolute(&timer, target);
        mutex_lock(&mutex);
    }
out:
    *last_wakeup = target;
}

static void _callback_msg(void* arg)
{
    msg_t *msg = (msg_t*)arg;
    msg_send_int(msg, msg->sender_pid);
}

static inline void _setup_msg(xtimer_t *timer, msg_t *msg, kernel_pid_t target_pid)
{
    timer->callback = _callback_msg;
    timer->arg = (void*) msg;

    /* use sender_pid field to get target_pid into callback function */
    msg->sender_pid = target_pid;
}

void _xtimer_set_msg(xtimer_t *timer, uint32_t offset, msg_t *msg, kernel_pid_t target_pid)
{
    _setup_msg(timer, msg, target_pid);
    _xtimer_set(timer, offset);
}

void _xtimer_set_msg64(xtimer_t *timer, uint64_t offset, msg_t *msg, kernel_pid_t target_pid)
{
    _setup_msg(timer, msg, target_pid);
    _xtimer_set64(timer, offset, offset >> 32);
}

static void _callback_wakeup(void* arg)
{
    thread_wakeup((kernel_pid_t)((intptr_t)arg));
}

void _xtimer_set_wakeup(xtimer_t *timer, uint32_t offset, kernel_pid_t pid)
{
    timer->callback = _callback_wakeup;
    timer->arg = (void*) ((intptr_t)pid);

    _xtimer_set(timer, offset);
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

/* Prepares the message to trigger the timeout.
 * Additionally, the xtimer_t struct gets initialized.
 */
static void _setup_timer_msg(msg_t *m, xtimer_t *t)
{
    m->type = MSG_XTIMER;
    m->content.ptr = m;

    t->target = t->long_target = 0;
}

/* Waits for incoming message or timeout. */
static int _msg_wait(msg_t *m, msg_t *tmsg, xtimer_t *t)
{
    msg_receive(m);
    if (m->type == MSG_XTIMER && m->content.ptr == tmsg) {
        /* we hit the timeout */
        return -1;
    }
    else {
        xtimer_remove(t);
        return 1;
    }
}

int _xtimer_msg_receive_timeout64(msg_t *m, uint64_t timeout_ticks) {
    msg_t tmsg;
    xtimer_t t;
    _setup_timer_msg(&tmsg, &t);
    _xtimer_set_msg64(&t, timeout_ticks, &tmsg, sched_active_pid);
    return _msg_wait(m, &tmsg, &t);
}

int _xtimer_msg_receive_timeout(msg_t *msg, uint32_t timeout_ticks)
{
    msg_t tmsg;
    xtimer_t t;
    _setup_timer_msg(&tmsg, &t);
    _xtimer_set_msg(&t, timeout_ticks, &tmsg, sched_active_pid);
    return _msg_wait(msg, &tmsg, &t);
}

static void _mutex_timeout(void *arg)
{
    mutex_thread_t *mt = (mutex_thread_t *)arg;

    mt->timeout = 1;
    list_node_t *node = list_remove(&mt->mutex->queue,
                                    (list_node_t *)&mt->thread->rq_entry);
    if ((node != NULL) && (mt->mutex->queue.next == NULL)) {
        mt->mutex->queue.next = MUTEX_LOCKED;
    }
    sched_set_status(mt->thread, STATUS_PENDING);
    thread_yield_higher();
}

int xtimer_mutex_lock_timeout(mutex_t *mutex, uint64_t timeout)
{
    xtimer_t t;
    mutex_thread_t mt = { mutex, (thread_t *)sched_active_thread, 0 };

    if (timeout != 0) {
        t.callback = _mutex_timeout;
        t.arg = (void *)((mutex_thread_t *)&mt);
        _xtimer_set64(&t, timeout, timeout >> 32);
    }

    mutex_lock(mutex);
    xtimer_remove(&t);
    return -mt.timeout;
}

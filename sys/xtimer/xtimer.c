/**
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup xtimer
 * @{
 * @file
 * @brief xtimer convenience functionality
 * @author Kaspar Schleiser <kaspar@schleiser.de>
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

#include "timex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _callback_unlock_mutex(void* arg)
{
    mutex_t *mutex = (mutex_t *) arg;
    mutex_unlock(mutex);
}

void _xtimer_sleep(uint32_t offset, uint32_t long_offset)
{
    if (irq_is_in()) {
        assert(!long_offset);
        xtimer_spin(offset);
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

void xtimer_usleep_until(uint32_t *last_wakeup, uint32_t interval) {
    xtimer_t timer;
    mutex_t mutex = MUTEX_INIT;

    timer.callback = _callback_unlock_mutex;
    timer.arg = (void*) &mutex;

    uint32_t target = *last_wakeup + interval;

    uint32_t now = xtimer_now();
    /* make sure we're not setting a value in the past */
    if (now < *last_wakeup) {
        /* base timer overflowed */
        if (!((target < *last_wakeup) && (target > now))) {
            goto out;
        }
    }
    else if (! ((target < *last_wakeup) || (target > now))) {
        goto out;
    }

    /* For large offsets, set an absolute target time, as
     * it is more exact.
     *
     * As that might cause an underflow, for small offsets,
     * use a relative offset, as that can never underflow.
     *
     * For very small offsets, spin.
     */
    uint32_t offset = target - now;

    if (offset > (XTIMER_BACKOFF * 2)) {
        mutex_lock(&mutex);
        if (offset >> 9) { /* >= 512 */
            offset = target;
        }
        else {
            offset += xtimer_now();
        }
        _xtimer_set_absolute(&timer, offset);
        mutex_lock(&mutex);
    }
    else {
        xtimer_spin(offset);
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

void xtimer_set_msg(xtimer_t *timer, uint32_t offset, msg_t *msg, kernel_pid_t target_pid)
{
    _setup_msg(timer, msg, target_pid);
    xtimer_set(timer, offset);
}

void xtimer_set_msg64(xtimer_t *timer, uint64_t offset, msg_t *msg, kernel_pid_t target_pid)
{
    _setup_msg(timer, msg, target_pid);
    _xtimer_set64(timer, offset, offset >> 32);
}

static void _callback_wakeup(void* arg)
{
    thread_wakeup((kernel_pid_t)((intptr_t)arg));
}

void xtimer_set_wakeup(xtimer_t *timer, uint32_t offset, kernel_pid_t pid)
{
    timer->callback = _callback_wakeup;
    timer->arg = (void*) ((intptr_t)pid);

    xtimer_set(timer, offset);
}

void xtimer_set_wakeup64(xtimer_t *timer, uint64_t offset, kernel_pid_t pid)
{
    timer->callback = _callback_wakeup;
    timer->arg = (void*) ((intptr_t)pid);

    _xtimer_set64(timer, offset, offset >> 32);
}

void xtimer_now_timex(timex_t *out)
{
    uint64_t now = xtimer_now64();

    out->seconds = div_u64_by_1000000(now);
    out->microseconds = now - (out->seconds * SEC_IN_USEC);
}

/* Prepares the message to trigger the timeout.
 * Additionally, the xtimer_t struct gets initialized.
 */
static void _setup_timer_msg(msg_t *m, xtimer_t *t)
{
    m->type = MSG_XTIMER;
    m->content.ptr = (char *) m;

    t->target = t->long_target = 0;
}

/* Waits for incoming message or timeout. */
static int _msg_wait(msg_t *m, msg_t *tmsg, xtimer_t *t)
{
    msg_receive(m);
    if (m->type == MSG_XTIMER && m->content.ptr == (char *) tmsg) {
        /* we hit the timeout */
        return -1;
    }
    else {
        xtimer_remove(t);
        return 1;
    }
}

int xtimer_msg_receive_timeout64(msg_t *m, uint64_t timeout) {
    msg_t tmsg;
    xtimer_t t;
    _setup_timer_msg(&tmsg, &t);
    xtimer_set_msg64(&t, timeout, &tmsg, sched_active_pid);
    return _msg_wait(m, &tmsg, &t);
}

int xtimer_msg_receive_timeout(msg_t *msg, uint32_t us)
{
    msg_t tmsg;
    xtimer_t t;
    _setup_timer_msg(&tmsg, &t);
    xtimer_set_msg(&t, us, &tmsg, sched_active_pid);
    return _msg_wait(msg, &tmsg, &t);
}

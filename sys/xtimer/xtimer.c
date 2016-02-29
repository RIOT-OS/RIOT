/**
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup xtimer
 * @{
 * @file
 * @brief    xtimer convenience functionality
 *
 * @author   Kaspar Schleiser <kaspar@schleiser.de>
 * @author   Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
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
#if ENABLE_DEBUG
#include <inttypes.h>
#endif

void _xtimer_set_ticks(xtimer_t *timer, uint32_t offset);
void _xtimer_set_ticks64(xtimer_t *timer, uint32_t offset, uint32_t long_offset);

static void _callback_unlock_mutex(void* arg)
{
    mutex_t *mutex = (mutex_t *) arg;
    mutex_unlock(mutex);
}

void _xtimer_sleep_ticks(uint32_t offset, uint32_t long_offset)
{
    if (inISR()) {
        assert(long_offset == 0);
        _xtimer_spin_ticks(offset);
    }

    xtimer_t timer;
    mutex_t mutex = MUTEX_INIT;

    timer.callback = _callback_unlock_mutex;
    timer.arg = (void*) &mutex;
    timer.target = timer.long_target = 0;

    mutex_lock(&mutex);
    _xtimer_set_ticks64(&timer, offset, long_offset);
    mutex_lock(&mutex);
}

void xtimer_usleep_until(uint32_t *last_wakeup, uint32_t interval) {
    xtimer_t timer;
    mutex_t mutex = MUTEX_INIT;
    timer.callback = _callback_unlock_mutex;
    timer.arg = (void*) &mutex;

    uint32_t last_wakeup_us = *last_wakeup;

    uint32_t target_us = last_wakeup_us + interval;
    uint32_t now_short = 0;
    uint32_t now_long = 0;
    _xtimer_now_ticks64(&now_short, &now_long);
    uint64_t now_ticks = ((uint64_t)now_long << 32) | now_short;
    uint32_t now_us = _xtimer_ticks_to_us64(now_ticks);
    /* make sure we're not setting a value in the past */
    if (now_us < last_wakeup_us) {
        /* base timer overflowed between last_wakeup and now */
        if (!((now_us < target_us) && (target_us < last_wakeup_us))) {
            /* target time has already passed */
            goto out;
        }
    }
    else {
        /* base timer did not overflow */
        if (((last_wakeup_us < target_us) && (target_us < now_us))) {
            /* target time has already passed */
            goto out;
        }
    }

    /*
     * For large offsets, set an absolute target time.
     * As that might cause an underflow, for small offsets, spin.
     *
     * Note: last_wakeup must never specify a time in the future after
     * xtimer_usleep_until returns.
     * If this happens, last_wakeup may specify a time in the future when the
     * next call to xtimer_usleep_until is made, which in turn will trigger the
     * overflow logic above and make the next timer fire too early, causing
     * last_wakeup to point even further into the future, leading to a chain
     * reaction.
     *
     * To ensure that last_wakeup does not point to the future we compute the
     * offset rounding up to nearest tick past the target.
     *
     * Observed on:
     *  - mulle in tests/xtimer_drift with XTIMER_HZ set to 32768, and 31250
     */
    uint32_t offset_ticks = _xtimer_us_to_ticks_ceil(target_us - now_us);
    DEBUG("xuu, now: %9" PRIu32 ", tgt: %9" PRIu32 ", off: %9" PRIu32 "\n", now_us, target_us, offset_ticks);
    if (offset_ticks > XTIMER_BACKOFF) {
        uint32_t target_ticks = now_ticks + offset_ticks;
        mutex_lock(&mutex);
        DEBUG("xuu, abs: %" PRIu32 "\n", target_ticks);
        _xtimer_set_absolute_ticks(&timer, target_ticks);
        mutex_lock(&mutex);
    }
    else {
        _xtimer_spin_ticks(offset_ticks);
    }
out:
    *last_wakeup = target_us;
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
    offset = _xtimer_us_to_ticks(offset);
    _xtimer_set_ticks(timer, offset);
}

void xtimer_set_msg64(xtimer_t *timer, uint64_t offset, msg_t *msg, kernel_pid_t target_pid)
{
    _setup_msg(timer, msg, target_pid);
    offset = _xtimer_us_to_ticks64(offset);
    _xtimer_set_ticks64(timer, offset, offset >> 32);
}

static void _callback_wakeup(void* arg)
{
    thread_wakeup((kernel_pid_t)((intptr_t)arg));
}

void xtimer_set_wakeup(xtimer_t *timer, uint32_t offset, kernel_pid_t pid)
{
    timer->callback = _callback_wakeup;
    timer->arg = (void*) ((intptr_t)pid);
    offset = _xtimer_us_to_ticks(offset);
    _xtimer_set_ticks(timer, offset);
}

void xtimer_set_wakeup64(xtimer_t *timer, uint64_t offset, kernel_pid_t pid)
{
    timer->callback = _callback_wakeup;
    timer->arg = (void*) ((intptr_t)pid);
    offset = _xtimer_us_to_ticks64(offset);
    _xtimer_set_ticks64(timer, offset, offset >> 32);
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

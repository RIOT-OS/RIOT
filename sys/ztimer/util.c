/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Inria
 *               2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_ztimer_util ztimer utility functions
 * @ingroup     sys_ztimer
 * @{
 *
 * @file
 * @brief       ztimer high-level utility function implementations
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include <assert.h>
#include <errno.h>

#include "atomic_utils.h"
#include "irq.h"
#include "mutex.h"
#include "rmutex.h"
#include "thread.h"
#include "ztimer.h"

#define ENABLE_DEBUG    0
#include "debug.h"

static void _callback_unlock_mutex(void *arg)
{
    mutex_t *mutex = (mutex_t *)arg;

    mutex_unlock(mutex);
}

void ztimer_sleep(ztimer_clock_t *clock, uint32_t duration)
{
    assert(!irq_is_in());
    mutex_t mutex = MUTEX_INIT_LOCKED;

    ztimer_t timer = {
        .callback = _callback_unlock_mutex,
        .arg = (void *)&mutex,
    };

    /* correct board / MCU specific overhead */
    if (duration > clock->adjust_sleep) {
        duration -= clock->adjust_sleep;
    }
    else {
        duration = 0;
    }

    ztimer_set(clock, &timer, duration);
    mutex_lock(&mutex);
}

void ztimer_periodic_wakeup(ztimer_clock_t *clock, uint32_t *last_wakeup,
                            uint32_t period)
{
    unsigned state = irq_disable();
    uint32_t now = ztimer_now(clock);
    uint32_t target = *last_wakeup + period;
    uint32_t offset = target - now;

    irq_restore(state);

    if (offset <= period) {
        ztimer_sleep(clock, offset);
        *last_wakeup = target;
    }
    else {
        *last_wakeup = now;
    }
}

#ifdef MODULE_CORE_MSG
static void _callback_msg(void *arg)
{
    msg_t *msg = (msg_t *)arg;

    msg_send_int(msg, msg->sender_pid);
}

static inline void _setup_msg(ztimer_t *timer, msg_t *msg,
                              kernel_pid_t target_pid)
{
    timer->callback = _callback_msg;
    timer->arg = (void *)msg;

    /* use sender_pid field to get target_pid into callback function */
    msg->sender_pid = target_pid;
}

void ztimer_set_msg(ztimer_clock_t *clock, ztimer_t *timer, uint32_t offset,
                    msg_t *msg, kernel_pid_t target_pid)
{
    _setup_msg(timer, msg, target_pid);
    ztimer_set(clock, timer, offset);
}

int ztimer_msg_receive_timeout(ztimer_clock_t *clock, msg_t *msg,
                               uint32_t timeout)
{
    if (msg_try_receive(msg) == 1) {
        return 1;
    }

    ztimer_t t = { .base = { .next = NULL } };
    msg_t m = { .type = MSG_ZTIMER, .content.ptr = &m };

    ztimer_set_msg(clock, &t, timeout, &m, thread_getpid());

    msg_receive(msg);
    ztimer_remove(clock, &t);
    if (msg->type == MSG_ZTIMER && msg->content.ptr == &m) {
        /* we hit the timeout */
        return -ETIME;
    }
    else {
        return 1;
    }
}

#endif /* MODULE_CORE_MSG */

#ifdef MODULE_CORE_MBOX
struct ztimer_mbox_thread_status {
    ztimer_t timer;
    mbox_t *mbox;
    thread_t *thread;
    int status;
};

static void _ztimer_mbox_get_timeout(void *arg)
{
    struct ztimer_mbox_thread_status *data = arg;
    if (!list_remove(&data->mbox->readers, &data->thread->rq_entry)) {
        /* timeout triggered just after the message was received but before the
         * timer was canceled. --> ignore the timeout */
        DEBUG_PUTS("ztimer_mbox_get_timeout: timeout triggered, but message already received");
        return;
    }

    sched_set_status(data->thread, STATUS_PENDING);
    data->status = -ETIMEDOUT;
    thread_yield_higher();
    DEBUG_PUTS("ztimer_mbox_get_timeout: timeout triggered");
}

int ztimer_mbox_get_timeout(ztimer_clock_t *clock, mbox_t *mbox, msg_t *msg, uint32_t timeout)
{
    struct ztimer_mbox_thread_status data = {
        .timer = {
            .arg = &data,
            .callback = _ztimer_mbox_get_timeout,
        },
        .mbox = mbox,
        .thread = thread_get_active(),
        .status = 0,
    };

    ztimer_set(clock, &data.timer, timeout);
    mbox_get(mbox, msg);
    ztimer_remove(clock, &data.timer);
    return data.status;
}
#endif

#ifdef MODULE_CORE_THREAD_FLAGS
static void _set_timeout_flag_callback(void *arg)
{
    thread_flags_set(arg, THREAD_FLAG_TIMEOUT);
}

void ztimer_set_timeout_flag(ztimer_clock_t *clock, ztimer_t *t,
                             uint32_t timeout)
{
    t->callback = _set_timeout_flag_callback;
    t->arg = thread_get_active();
    thread_flags_clear(THREAD_FLAG_TIMEOUT);
    ztimer_set(clock, t, timeout);
}
#endif

void ztimer_mutex_unlock(ztimer_clock_t *clock, ztimer_t *timer, uint32_t offset,
                         mutex_t *mutex)
{
    unsigned state = irq_disable();

    timer->callback = _callback_unlock_mutex;
    timer->arg = (void *)mutex;

    irq_restore(state);
    ztimer_set(clock, timer, offset);
}

static void _callback_wakeup(void *arg)
{
    thread_wakeup((kernel_pid_t)((intptr_t)arg));
}

void ztimer_set_wakeup(ztimer_clock_t *clock, ztimer_t *timer, uint32_t offset,
                       kernel_pid_t pid)
{
    ztimer_remove(clock, timer);

    timer->callback = _callback_wakeup;
    timer->arg = (void *)((intptr_t)pid);

    ztimer_set(clock, timer, offset);
}

static void timeout_cb(void *arg)
{
    mutex_cancel(arg);
}

int ztimer_mutex_lock_timeout(ztimer_clock_t *clock, mutex_t *mutex,
                              uint32_t timeout)
{
    if (mutex_trylock(mutex)) {
        return 0;
    }

    mutex_cancel_t mc = mutex_cancel_init(mutex);
    ztimer_t t = { .callback = timeout_cb, .arg = &mc };

    ztimer_set(clock, &t, timeout);
    if (mutex_lock_cancelable(&mc)) {
        return -ECANCELED;
    }

    ztimer_remove(clock, &t);
    return 0;
}

int ztimer_rmutex_lock_timeout(ztimer_clock_t *clock, rmutex_t *rmutex,
                               uint32_t timeout)
{
    if (rmutex_trylock(rmutex)) {
        return 0;
    }
    if (ztimer_mutex_lock_timeout(clock, &rmutex->mutex, timeout) == 0) {
        atomic_store_kernel_pid(&rmutex->owner, thread_getpid());
        rmutex->refcount++;
        return 0;
    }
    return -ECANCELED;
}

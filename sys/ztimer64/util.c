/*
 * Copyright (C) 2021 Kaspar Schleiser <kaspar@schleiser.de>
 *               2021 Inria
 *               2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_ztimer64_util ztimer64 utility functions
 * @ingroup     sys_ztimer64
 * @{
 *
 * @file
 * @brief       ztimer64 high-level utility function implementations
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include <assert.h>
#include <errno.h>

#include "irq.h"
#include "mutex.h"
#include "rmutex.h"
#include "thread.h"
#include "ztimer64.h"

static void _callback_unlock_mutex(void *arg)
{
    mutex_t *mutex = (mutex_t *)arg;

    mutex_unlock(mutex);
}

void ztimer64_sleep_until(ztimer64_clock_t *clock, uint64_t target)
{
    assert(!irq_is_in());
    mutex_t mutex = MUTEX_INIT_LOCKED;

    ztimer64_t timer = {
        .callback = _callback_unlock_mutex,
        .arg = (void *)&mutex,
    };

    ztimer64_set_at(clock, &timer, target);
    mutex_lock(&mutex);
}

void ztimer64_periodic_wakeup(ztimer64_clock_t *clock, uint64_t *last_wakeup,
                              uint64_t period)
{
    unsigned state = irq_disable();
    uint64_t now = ztimer64_now(clock);
    uint64_t target = *last_wakeup + period;

    irq_restore(state);

    if (target > now) {
        ztimer64_sleep_until(clock, target);
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

static inline void _setup_msg(ztimer64_t *timer, msg_t *msg,
                              kernel_pid_t target_pid)
{
    timer->callback = _callback_msg;
    timer->arg = (void *)msg;

    /* use sender_pid field to get target_pid into callback function */
    msg->sender_pid = target_pid;
}

void ztimer64_set_msg_at(ztimer64_clock_t *clock, ztimer64_t *timer,
                         uint64_t target,
                         msg_t *msg, kernel_pid_t target_pid)
{
    _setup_msg(timer, msg, target_pid);
    ztimer64_set_at(clock, timer, target);
}

int ztimer64_msg_receive_until(ztimer64_clock_t *clock, msg_t *msg,
                               uint64_t target)
{
    if (msg_try_receive(msg) == 1) {
        return 1;
    }

    ztimer64_t t;
    msg_t m = { .type = MSG_ZTIMER, .content.ptr = &m };

    ztimer64_set_msg_at(clock, &t, target, &m, thread_getpid());

    msg_receive(msg);
    ztimer64_remove(clock, &t);
    if (msg->type == MSG_ZTIMER64 && msg->content.ptr == &m) {
        /* we hit the timeout */
        return -ETIME;
    }
    else {
        return 1;
    }
}

#endif /* MODULE_CORE_MSG */

#ifdef MODULE_CORE_THREAD_FLAGS
static void _set_timeout_flag_callback(void *arg)
{
    thread_flags_set(arg, THREAD_FLAG_TIMEOUT);
}

void ztimer64_set_timeout_flag_at(ztimer64_clock_t *clock, ztimer64_t *t,
                                  uint64_t target)
{
    t->callback = _set_timeout_flag_callback;
    t->arg = thread_get_active();
    thread_flags_clear(THREAD_FLAG_TIMEOUT);
    ztimer64_set_at(clock, t, target);
}
#endif

static void _callback_wakeup(void *arg)
{
    thread_wakeup((kernel_pid_t)((intptr_t)arg));
}

void ztimer64_set_wakeup_at(ztimer64_clock_t *clock, ztimer64_t *timer,
                            uint64_t target,
                            kernel_pid_t pid)
{
    ztimer64_remove(clock, timer);

    timer->callback = _callback_wakeup;
    timer->arg = (void *)((intptr_t)pid);

    ztimer64_set_at(clock, timer, target);
}

static void timeout_cb(void *arg)
{
    mutex_cancel(arg);
}

int ztimer64_mutex_lock_until(ztimer64_clock_t *clock, mutex_t *mutex,
                              uint64_t target)
{
    if (mutex_trylock(mutex)) {
        return 0;
    }

    mutex_cancel_t mc = mutex_cancel_init(mutex);
    ztimer64_t t = { .callback = timeout_cb, .arg = &mc };

    ztimer64_set_at(clock, &t, target);
    if (mutex_lock_cancelable(&mc)) {
        return -ECANCELED;
    }

    ztimer64_remove(clock, &t);
    return 0;
}

int ztimer64_rmutex_lock_until(ztimer64_clock_t *clock, rmutex_t *rmutex,
                               uint64_t target)
{
    if (rmutex_trylock(rmutex)) {
        return 0;
    }
    if (ztimer64_mutex_lock_until(clock, &rmutex->mutex, target) == 0) {
        atomic_store_explicit(&rmutex->owner,
                              thread_getpid(), memory_order_relaxed);
        rmutex->refcount++;
        return 0;
    }
    return -ECANCELED;
}

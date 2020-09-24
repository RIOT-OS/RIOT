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
#include "rmutex.h"
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
    /*
     * @brief:  one means the thread was removed from the mutex thread waiting list
     *          by _mutex_timeout()
     */
    volatile uint8_t dequeued;
    /*
     * @brief:  mutex_lock() should block because _mutex_timeout() did not shoot
     */
    volatile uint8_t blocking;
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
    _xtimer_set64(timer, offset, 0);
}

void _xtimer_set_msg64(xtimer_t *timer, uint64_t offset, msg_t *msg, kernel_pid_t target_pid)
{
    _setup_msg(timer, msg, target_pid);
    _xtimer_set64(timer, offset, offset >> 32);
}

/* Prepares the message to trigger the timeout.
 * Additionally, the xtimer_t struct gets initialized.
 */
static void _setup_timer_msg(msg_t *m, xtimer_t *t)
{
    m->type = MSG_XTIMER;
    m->content.ptr = m;

    t->offset = t->long_offset = 0;
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
    _xtimer_set_msg64(&t, timeout_ticks, &tmsg, thread_getpid());
    return _msg_wait(m, &tmsg, &t);
}

int _xtimer_msg_receive_timeout(msg_t *msg, uint32_t timeout_ticks)
{
    msg_t tmsg;
    xtimer_t t;
    _setup_timer_msg(&tmsg, &t);
    _xtimer_set_msg(&t, timeout_ticks, &tmsg, thread_getpid());
    return _msg_wait(msg, &tmsg, &t);
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

/*
 * The value pointed to by unlocked will be set to 1 if the thread was removed from the waiting queue otherwise 0.
 */
static void _mutex_remove_thread_from_waiting_queue(mutex_t *mutex, thread_t *thread, volatile uint8_t *unlocked)
{
    unsigned irqstate = irq_disable();
    assert(mutex != NULL && thread != NULL);

    if (mutex->queue.next != MUTEX_LOCKED && mutex->queue.next != NULL) {
        list_node_t *node = list_remove(&mutex->queue, (list_node_t *)&thread->rq_entry);
        /* if thread was removed from the list */
        if (node != NULL) {
            if (mutex->queue.next == NULL) {
                mutex->queue.next = MUTEX_LOCKED;
            }
            *unlocked = 1;

            sched_set_status(thread, STATUS_PENDING);
            irq_restore(irqstate);
            sched_switch(thread->priority);
            return;
        }
    }
    *unlocked = 0;
    irq_restore(irqstate);
}

static void _mutex_timeout(void *arg)
{
    /* interrupts are disabled because xtimer can spin
     * if xtimer_set spins the callback is executed
     * in the thread context
     *
     * If the xtimer spin is fixed in the future
     * interups disable/restore can be removed
     */
    unsigned int irqstate = irq_disable();

    mutex_thread_t *mt = arg;
    mt->blocking = 0;
    _mutex_remove_thread_from_waiting_queue(mt->mutex, mt->thread, &mt->dequeued);
    irq_restore(irqstate);
}

static int _mutex_lock_internal(mutex_t *mutex, volatile uint8_t *blocking)
{
    unsigned irqstate = irq_disable();

    DEBUG("PID[%" PRIkernel_pid "]: Mutex in use.\n", thread_getpid());

    if (mutex->queue.next == NULL) {
        /* mutex is unlocked. */
        mutex->queue.next = MUTEX_LOCKED;
        DEBUG("PID[%" PRIkernel_pid "]: mutex_wait early out.\n",
              thread_getpid());
        irq_restore(irqstate);
        return 1;
    }
    else if (*blocking) {
        thread_t *me = thread_get_active();
        DEBUG("PID[%" PRIkernel_pid "]: Adding node to mutex queue: prio: %"
              PRIu32 "\n", thread_getpid(), (uint32_t)me->priority);
        sched_set_status(me, STATUS_MUTEX_BLOCKED);
        if (mutex->queue.next == MUTEX_LOCKED) {
            mutex->queue.next = (list_node_t *)&me->rq_entry;
            mutex->queue.next->next = NULL;
        }
        else {
            thread_add_to_list(&mutex->queue, me);
        }
        irq_restore(irqstate);
        thread_yield_higher();
        /* We were woken up by scheduler. Waker removed us from queue.
         * We have the mutex now. */
        return 1;
    }
    else {
        irq_restore(irqstate);
        return 0;
    }
}

int xtimer_mutex_lock_timeout(mutex_t *mutex, uint64_t timeout)
{
    xtimer_t t;
    mutex_thread_t mt = {
        mutex, thread_get_active(), .dequeued = 0, .blocking = 1
    };

    if (timeout != 0) {
        t.callback = _mutex_timeout;
        t.arg = &mt;
        xtimer_set64(&t, timeout);
    }
    int ret = _mutex_lock_internal(mutex, &mt.blocking);
    if (ret == 0) {
        return -1;
    }
    xtimer_remove(&t);
    return -mt.dequeued;
}

int xtimer_rmutex_lock_timeout(rmutex_t *rmutex, uint64_t timeout)
{
    if (rmutex_trylock(rmutex)) {
        return 0;
    }
    if (xtimer_mutex_lock_timeout(&rmutex->mutex, timeout) == 0) {
        atomic_store_explicit(&rmutex->owner,
                              thread_getpid(), memory_order_relaxed);
        rmutex->refcount++;
        return 0;
    }
    return -1;
}

#ifdef MODULE_CORE_THREAD_FLAGS
static void _set_timeout_flag_callback(void* arg)
{
    thread_flags_set(arg, THREAD_FLAG_TIMEOUT);
}

static void _set_timeout_flag_prepare(xtimer_t *t)
{
    t->callback = _set_timeout_flag_callback;
    t->arg = thread_get_active();
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

uint64_t xtimer_left_usec(const xtimer_t *timer)
{
    unsigned state = irq_disable();
    /* ensure we're working on valid data by making a local copy of timer */
    xtimer_t t = *timer;
    uint64_t now_us = xtimer_now_usec64();
    irq_restore(state);

    uint64_t start_us = _xtimer_usec_from_ticks64(
        ((uint64_t)t.long_start_time << 32) | t.start_time);
    uint64_t target_us = start_us + _xtimer_usec_from_ticks64(
        ((uint64_t)t.long_offset) << 32 | t.offset);

    /* Let's assume that 64bit won't overflow anytime soon. There'd be >580
     * years when counting nanoseconds. With microseconds, there are 580000
     * years of space in 2**64... */
    if (now_us > target_us) {
        return 0;
    }

    return target_us - now_us;
}

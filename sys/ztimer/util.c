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

static list_node_t rooms = {.next = NULL};
static mutex_t room_lock = MUTEX_INIT;

static void _callback_unlock_mutex(void *arg)
{
    mutex_t *mutex = (mutex_t *)arg;

    mutex_unlock(mutex);
}

static void _callback_sleep_range(void *arg)
{
    callback_sleep_range_arg_t *args = (callback_sleep_range_arg_t *)arg;
    
    mutex_lock(&room_lock);
    list_remove(&rooms, &(args->room->node));
    mutex_unlock(&room_lock);

    ztimer_release(ZTIMER_USEC);
    
    mutex_lock(args->lock);
    cond_broadcast(args->condition_var);
    mutex_unlock(args->lock);
}

// unsure if we really need this.
static void _add_room_sorted(ztimer_room_t* new_room)
{
    if (!new_room || !&(new_room->node)) {
        return;
    }

    list_node_t* curr = &rooms;
    while (curr->next != NULL) {
        ztimer_room_t* curr_room = (ztimer_room_t *)curr->next;
        if (curr_room->end > new_room->end) {
            break;
        }
        curr = curr->next;
    }
    new_room->node.next = curr->next;
    mutex_lock(&room_lock);
    curr->next = &(new_room->node);
    mutex_unlock(&room_lock);
}

void ztimer_sleep_range(uint32_t min_time, uint32_t max_time)
{
    assert(!irq_is_in());
    ztimer_now_t start = ztimer_now(ZTIMER_USEC);

    // add this interval to a room
    bool added_to_existing_room = false;
    list_node_t* curr = &rooms;
    
    // Set relative times based on whether or not there are any rooms currently.
    // No rooms means there is no guarantee about timer being acquired.
    ztimer_now_t base;
    uint32_t relative_min;
    uint32_t relative_max;
    if (curr->next != NULL) {
        base = ztimer_now(ZTIMER_USEC);
        relative_min = base + min_time;
        relative_max = base + max_time;

        while (curr->next != NULL) {
            //ztimer_room_t* curr_room = (ztimer_room_t *)curr->next;
            ztimer_room_t* curr_room = container_of((clist_node_t*)curr->next, 
                                                     ztimer_room_t, node);
            if (!(relative_min > curr_room->end ||
                  relative_max < curr_room->begin)) {
                added_to_existing_room = true;
                if (relative_min > curr_room->begin) {
                    curr_room->begin = relative_min;
                }
                if (relative_max < curr_room->end) {
                    curr_room->end = relative_max;
                    ztimer_remove(ZTIMER_USEC, curr_room->room_timer);
                    ztimer_now_t elapsed = ztimer_now(ZTIMER_USEC) - start;
                    /* correct board / MCU specific overhead */
                    if (relative_max > elapsed) {
                        relative_max -= elapsed;
                    }
                    else {
                        relative_max = 0;
                    }
                    ztimer_set(ZTIMER_USEC, curr_room->room_timer, relative_max - base);
                }
                mutex_lock(curr_room->cond_lock);
                cond_wait(curr_room->condition_var, curr_room->cond_lock);
                mutex_unlock(curr_room->cond_lock);
                break;
            }
            curr = curr->next;
        }
    }

    // Create new room
    if (!added_to_existing_room) {
        ztimer_acquire(ZTIMER_USEC); // keep this timer as long as the room is in use.
        base = ztimer_now(ZTIMER_USEC);
        relative_min = base + min_time;
        relative_max = base + max_time;

        list_node_t new_node = {.next = NULL};
        mutex_t new_cond_lock = MUTEX_INIT;
        cond_t new_condition_var = COND_INIT;
        callback_sleep_range_arg_t callback_args = {
            .condition_var = &new_condition_var,
            .lock = &new_cond_lock
        };
        ztimer_t new_timer = {
            .callback = _callback_sleep_range,
            .arg = (void *)&callback_args,
        };
        ztimer_room_t new_room = {
            .node = new_node,
            .begin = relative_min,
            .end = relative_max,
            .cond_lock = &new_cond_lock,
            .condition_var = &new_condition_var,
            .room_timer = &new_timer
        };
        callback_args.room = &new_room;
        _add_room_sorted(&new_room);
        mutex_lock(&room_lock);
        mutex_unlock(&room_lock);
        ztimer_now_t elapsed = ztimer_now(ZTIMER_USEC) - start;
        /* correct board / MCU specific overhead */
        if (max_time > elapsed) {
            max_time -= elapsed;
        }
        else {
            max_time = 0;
        }
        //ztimer_release(ZTIMER_USEC);
        ztimer_set(ZTIMER_USEC, new_room.room_timer, max_time);
        mutex_lock(new_room.cond_lock);
        cond_wait(new_room.condition_var, new_room.cond_lock);
        mutex_unlock(new_room.cond_lock);
    }
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

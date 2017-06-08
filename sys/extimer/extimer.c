/*
 * Copyright (C) Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>

#include "irq.h"
#include "utlist.h"

#include "extimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static void _time_next_event(extimer_t *timer);
static void _callback(void *arg);

uint64_t extimer_get_absolute(uint64_t offset)
{
    const uint64_t now = xtimer_now64();
    const uint64_t res = now + offset;

    DEBUG("%s(offset=%" PRIu64 ")\n", DEBUG_FUNC, offset);

    DEBUG("%s():%d: entering\n", DEBUG_FUNC, __LINE__);
    if (res < now) { /* handle integer overflow */
        DEBUG("%s():%d: integer overflow, returning UINT64_MAX\n", DEBUG_FUNC,
              __LINE__);
        return UINT64_MAX;
    }
    DEBUG("%s():%d: returning %" PRIu64 "\n", DEBUG_FUNC, __LINE__,
          res);
    return res;
}

void extimer_add(extimer_t *timer, extimer_event_t *event, kernel_pid_t pid)
{
    DEBUG("%s(timer=%p, event=%p, pid=%" PRIkernel_pid ")\n", DEBUG_FUNC,
          (void *)timer, (void *)event, pid);
    assert(timer);
    assert(event->next == NULL);
    assert(pid != KERNEL_PID_UNDEF);
    DEBUG("%s():%d: entering mutex[%p]\n", DEBUG_FUNC, __LINE__,
          (void *)&timer->mutex);
    mutex_lock(&timer->mutex);
    event->msg.sender_pid = pid;    /* use msg_t::sender_pid to store receiver
                                     * until actual sending */
    if (timer->events == NULL) {
        DEBUG("%s():%d: no events in current timer, schedule immediately at %"
              PRIu64 " usec\n", DEBUG_FUNC, __LINE__, event->time);
        timer->events = event;
        _time_next_event(timer);
    }
    else {
        extimer_event_t *ptr = NULL, *tmp = timer->events;
        while ((tmp != NULL) && (tmp->time <= event->time)) {
            if (event == tmp) {   /* check rest of the list for identities to event */
                DEBUG("%s():%d: event already scheduled. Doing nothing\n",
                      DEBUG_FUNC, __LINE__);
                mutex_unlock(&timer->mutex);
                DEBUG("%s():%d: exiting mutex[%p]\n", DEBUG_FUNC, __LINE__,
                      (void *)&timer->mutex);
                return;
            }
            ptr = tmp;
            tmp = tmp->next;
        }
        if (ptr == NULL) {  /* replace head */
            DEBUG("%s():%d: schedule immediately at %" PRIu64 " usec\n",
                  DEBUG_FUNC, __LINE__, event->time);
            event->next = timer->events;
            timer->events = event;
            _time_next_event(timer);
        }
        else {
            DEBUG("%s():%d: schedule later\n", DEBUG_FUNC, __LINE__);
            list_add((list_node_t *)ptr, (list_node_t *)event);
        }
    }
    mutex_unlock(&timer->mutex);
    DEBUG("%s():%d: exiting mutex[%p]\n", DEBUG_FUNC, __LINE__,
          (void *)&timer->mutex);
}

extimer_event_t *extimer_del(extimer_t *timer, extimer_event_t *event)
{
    extimer_event_t *res = NULL;
    unsigned state;

    DEBUG("%s(timer=%p, event=%p)\n", DEBUG_FUNC, (void *)timer, (void *)event);
    assert(timer != NULL);
    assert(event != NULL);

    DEBUG("%s():%d: entering mutex[%p]\n", DEBUG_FUNC, __LINE__,
          (void *)&timer->mutex);
    mutex_lock(&timer->mutex);      /* lock mutex to wait for threads that might
                                     * still use the timer */
    DEBUG("%s():%d: disabling IRQ\n", DEBUG_FUNC, __LINE__);
    state = irq_disable();          /* disable IRQ to not fire timer during
                                     * deletion */
    if (timer->events == event) {
        DEBUG("%s():%d: removing currently scheduled event\n", DEBUG_FUNC,
              __LINE__);
        xtimer_remove(&timer->timer);
        timer->events = timer->events->next;    /* remove head */
        res = event;
        _time_next_event(timer);
    }
    else {
        extimer_event_t *ptr = timer->events;
        while ((ptr->next != NULL) && (ptr->next != event)) {
            ptr = ptr->next;
        }
        if (ptr->next) {
            DEBUG("%s():%d: removing event further down the line\n",
                  DEBUG_FUNC, __LINE__);
            res = ptr->next;
            ptr->next = event->next;
        }
#if ENABLE_DEBUG
        else {
            DEBUG("%s():%d: event not found\n", DEBUG_FUNC, __LINE__);
        }
#endif
    }
    irq_restore(state);
    DEBUG("%s():%d: restoring IRQ 0x%08x\n", DEBUG_FUNC, __LINE__, state);
    mutex_unlock(&timer->mutex);    /* unlock mutex */
    DEBUG("%s():%d: exiting mutex[%p]\n", DEBUG_FUNC, __LINE__,
          (void *)&timer->mutex);
    return res;
}

static void _callback(void *arg)
{
    extimer_t *timer = (extimer_t *)arg;

    assert(timer);
    extimer_event_t *event = extimer_del(timer, timer->events);
    assert(event);
    msg_send_int(&event->msg, event->msg.sender_pid);
}

static void _time_next_event(extimer_t *timer)
{
    extimer_event_t *event = timer->events;
    uint64_t offset, now;

    if (event == NULL) {
        return;
    }
    now = xtimer_now(); /* check for past events */
    if (event->time <= now) {
        offset = 0;
    }
    else {
        offset = event->time - now;
    }
    timer->timer.callback = _callback;
    timer->timer.arg = (void *)timer;
    mutex_unlock(&timer->mutex);    /*  in case it fires immediately */
    DEBUG("%s():%d: exiting mutex[%p]\n", DEBUG_FUNC, __LINE__,
          (void *)&timer->mutex);
    DEBUG("%s():%d: Setting timer %p to fire in %" PRIu64 " usec\n", DEBUG_FUNC,
          __LINE__, (void *)timer, offset);
    _xtimer_set64(&timer->timer, offset, offset >> 32);
    DEBUG("%s():%d: entering mutex[%p]\n", DEBUG_FUNC, __LINE__,
          (void *)&timer->mutex);
    mutex_lock(&timer->mutex);
}


/** @} */

/**
 * virtual timer
 *
 * Copyright (C) 2013, 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup vtimer
 * @{
 * @file
 * @author Kaspar Schleiser <kaspar@schleiser.de> (author)
 * @author Oliver Hahm <oliver.hahm@inria.fr> (modifications)
 * @author Ludwig Ortmann <ludwig.ortmann@fu-berlin.de> (cleaning up the mess)
 * @}
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "irq.h"
#include "priority_queue.h"
#include "timex.h"
#include "hwtimer.h"
#include "msg.h"
#include "mutex.h"
#include "thread.h"
#include "kernel_macros.h"

#include "vtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define VTIMER_THRESHOLD 20UL
#define VTIMER_BACKOFF 10UL

#define SECONDS_PER_TICK (4096U)
#define MICROSECONDS_PER_TICK (4096UL * 1000000)

static void vtimer_callback(void *ptr);
static void vtimer_callback_tick(vtimer_t *timer);
static void vtimer_callback_msg(vtimer_t *timer);
static void vtimer_callback_wakeup(vtimer_t *timer);

static int vtimer_set(vtimer_t *timer);
static int set_longterm(vtimer_t *timer);
static int set_shortterm(vtimer_t *timer);

static priority_queue_t longterm_priority_queue_root = PRIORITY_QUEUE_INIT;
static priority_queue_t shortterm_priority_queue_root = PRIORITY_QUEUE_INIT;

static vtimer_t longterm_tick_timer;
static uint32_t longterm_tick_start;
static volatile int in_callback = false;

static int hwtimer_id = -1;
static uint32_t hwtimer_next_absolute;

static uint32_t seconds = 0;

static inline priority_queue_node_t *timer_get_node(vtimer_t *timer)
{
    if (!timer) {
        return NULL;
    }
    return &timer->priority_queue_entry;
}

static inline vtimer_t *node_get_timer(priority_queue_node_t *node)
{
    if (!node) {
        return NULL;
    }
    return container_of(node, vtimer_t, priority_queue_entry);
}

static int set_longterm(vtimer_t *timer)
{
    timer->priority_queue_entry.priority = timer->absolute.seconds;
    priority_queue_add(&longterm_priority_queue_root, timer_get_node(timer));
    return 0;
}

static int update_shortterm(void)
{
    if (shortterm_priority_queue_root.first == NULL) {
        /* there is no vtimer to schedule, queue is empty */
        DEBUG("update_shortterm: shortterm_priority_queue_root.next == NULL - dont know what to do here\n");
        return 0;
    }
    if (hwtimer_id != -1) {
        /* there is a running hwtimer for us */
        if (hwtimer_next_absolute != shortterm_priority_queue_root.first->priority) {
            /* the next timer in the vtimer queue is not the next hwtimer */
            /* we have to remove the running hwtimer (and schedule a new one) */
            hwtimer_remove(hwtimer_id);
        }
        else {
            /* the next vtimer is the next hwtimer, nothing to do */
            return 0;
        }
    }

    /* short term part of the next vtimer */
    hwtimer_next_absolute = shortterm_priority_queue_root.first->priority;

    uint32_t next = hwtimer_next_absolute;

    /* current short term time */
    uint32_t now = HWTIMER_TICKS_TO_US(hwtimer_now());

    /* make sure the longterm_tick_timer does not get truncated */
    if (node_get_timer(shortterm_priority_queue_root.first)->action != vtimer_callback_tick) {
        /* the next vtimer to schedule is the long term tick */
        /* it has a shortterm offset of longterm_tick_start */
        next += longterm_tick_start;
    }

    if((next -  HWTIMER_TICKS_TO_US(VTIMER_THRESHOLD) - now) > MICROSECONDS_PER_TICK ) {
        DEBUG("truncating next (next -  HWTIMER_TICKS_TO_US(VTIMER_THRESHOLD) - now): %lu\n", (next -  HWTIMER_TICKS_TO_US(VTIMER_THRESHOLD) - now));
        next = now +  HWTIMER_TICKS_TO_US(VTIMER_BACKOFF);
    }

    DEBUG("update_shortterm: Set hwtimer to %" PRIu32 " (now=%lu)\n", next, HWTIMER_TICKS_TO_US(hwtimer_now()));
    hwtimer_id = hwtimer_set_absolute(HWTIMER_TICKS(next), vtimer_callback, NULL);

    return 0;
}

void vtimer_callback_tick(vtimer_t *timer)
{
    (void) timer;

    DEBUG("vtimer_callback_tick().\n");
    seconds += SECONDS_PER_TICK;

    longterm_tick_start = longterm_tick_timer.absolute.microseconds;
    longterm_tick_timer.absolute.microseconds += MICROSECONDS_PER_TICK;
    set_shortterm(&longterm_tick_timer);

    while (longterm_priority_queue_root.first) {
        vtimer_t *timer = node_get_timer(longterm_priority_queue_root.first);

        if (timer->absolute.seconds == seconds) {
            priority_queue_remove_head(&longterm_priority_queue_root);
            set_shortterm(timer);
        }
        else {
            break;
        }
    }
}

static void vtimer_callback_msg(vtimer_t *timer)
{
    msg_t msg;
    msg.type = timer->type;
    msg.content.value = (unsigned int) timer->arg;
    msg_send_int(&msg, timer->pid);
}

static void vtimer_callback_wakeup(vtimer_t *timer)
{
    thread_wakeup(timer->pid);
}

static void vtimer_callback_unlock(vtimer_t *timer)
{
    mutex_t *mutex = (mutex_t *) timer->arg;
    mutex_unlock(mutex);
}

static int set_shortterm(vtimer_t *timer)
{
    DEBUG("set_shortterm(): Absolute: %" PRIu32 " %" PRIu32 "\n", timer->absolute.seconds, timer->absolute.microseconds);
    timer->priority_queue_entry.priority = timer->absolute.microseconds;
    priority_queue_add(&shortterm_priority_queue_root, timer_get_node(timer));
    return 1;
}

void vtimer_callback(void *ptr)
{
    DEBUG("vtimer_callback ptr=%p\n", ptr);
    (void) ptr;

    in_callback = true;
    hwtimer_id = -1;

    /* get the vtimer that fired */
    vtimer_t *timer = node_get_timer(priority_queue_remove_head(&shortterm_priority_queue_root));

    if (timer) {
#if ENABLE_DEBUG
        vtimer_print(timer);
#endif
        DEBUG("vtimer_callback(): Shooting %" PRIu32 ".\n", timer->absolute.microseconds);

        /* shoot timer */
        timer->action(timer);
    }
    else {
        DEBUG("vtimer_callback(): spurious call.\n");
    }

    in_callback = false;
    update_shortterm();
}

void normalize_to_tick(timex_t *time)
{
    DEBUG("Normalizing: %" PRIu32 " %" PRIu32 "\n", time->seconds, time->microseconds);
    uint32_t seconds_tmp = time->seconds % SECONDS_PER_TICK;
    time->seconds -= seconds_tmp;
    uint32_t usecs_tmp = time->microseconds + (seconds_tmp * 1000000);
    DEBUG("Normalizin2: %" PRIu32 " %" PRIu32 "\n", time->seconds, usecs_tmp);

    if (usecs_tmp < time->microseconds) {
        usecs_tmp -= MICROSECONDS_PER_TICK;
        time->seconds += SECONDS_PER_TICK;
    }

    if (usecs_tmp > MICROSECONDS_PER_TICK) {
        usecs_tmp -= MICROSECONDS_PER_TICK;
        time->seconds += SECONDS_PER_TICK;
    }

    time->microseconds = usecs_tmp;
    DEBUG("     Result: %" PRIu32 " %" PRIu32 "\n", time->seconds, time->microseconds);
}

static int vtimer_set(vtimer_t *timer)
{
    DEBUG("vtimer_set(): New timer. Offset: %" PRIu32 " %" PRIu32 "\n", timer->absolute.seconds, timer->absolute.microseconds);

    timex_t now;
    vtimer_now(&now);
    timer->absolute = timex_add(now, timer->absolute);
    normalize_to_tick(&(timer->absolute));

    DEBUG("vtimer_set(): Absolute: %" PRIu32 " %" PRIu32 "\n", timer->absolute.seconds, timer->absolute.microseconds);
    DEBUG("vtimer_set(): NOW: %" PRIu32 " %" PRIu32 "\n", now.seconds, now.microseconds);

    int result = 0;

    if (timer->absolute.seconds == 0) {
        if (timer->absolute.microseconds > 10) {
            timer->absolute.microseconds -= 10;
        }
    }

    unsigned state = disableIRQ();
    if (timer->absolute.seconds != seconds) {
        /* we're long-term */
        DEBUG("vtimer_set(): setting long_term\n");
        result = set_longterm(timer);
    }
    else {
        DEBUG("vtimer_set(): setting short_term\n");

        if (set_shortterm(timer)) {
            /* delay update of next shortterm timer if we
            * are called from within vtimer_callback. */

            if (!in_callback) {
                result = update_shortterm();
            }
        }
    }
    restoreIRQ(state);

    return result;
}

void vtimer_now(timex_t *out)
{
    uint32_t us = HWTIMER_TICKS_TO_US(hwtimer_now() - longterm_tick_start);
    uint32_t us_per_s = 1000ul * 1000ul;

    out->seconds = seconds + us / us_per_s;
    out->microseconds = us % us_per_s;
}

void vtimer_gettimeofday(struct timeval *tp) {
    timex_t now;
    vtimer_now(&now);

    tp->tv_sec = now.seconds;
    tp->tv_usec = now.microseconds;
}

void vtimer_get_localtime(struct tm *localt)
{
    timex_t now;
    vtimer_now(&now);

    localt->tm_sec = now.seconds % 60;
    localt->tm_min = (now.seconds / 60) % 60;
    localt->tm_hour = (now.seconds / 60 / 60) % 24;

    // TODO: fill the other fields
}

int vtimer_init(void)
{
    DEBUG("vtimer_init().\n");
    unsigned state = disableIRQ();
    seconds = 0;

    longterm_tick_start = 0;

    longterm_tick_timer.action = vtimer_callback_tick;
    longterm_tick_timer.arg = NULL;

    longterm_tick_timer.absolute.seconds = 0;
    longterm_tick_timer.absolute.microseconds = MICROSECONDS_PER_TICK;

    DEBUG("vtimer_init(): Setting longterm tick to %" PRIu32 "\n", longterm_tick_timer.absolute.microseconds);

    set_shortterm(&longterm_tick_timer);
    update_shortterm();

    restoreIRQ(state);
    return 0;
}

int vtimer_set_wakeup(vtimer_t *t, timex_t interval, kernel_pid_t pid)
{
    t->action = vtimer_callback_wakeup;
    t->arg = NULL;
    t->absolute = interval;
    t->pid = pid;
    return vtimer_set(t);
}

int vtimer_usleep(uint32_t usecs)
{
    timex_t offset = timex_set(0, usecs);
    return vtimer_sleep(offset);
}

int vtimer_sleep(timex_t time)
{
    /**
     * Use spin lock for short periods.
     * Assumes that hardware timer ticks are shorter than a second.
     */
    if (time.seconds == 0) {
        unsigned long ticks = HWTIMER_TICKS(time.microseconds);
        if (ticks <= HWTIMER_SPIN_BARRIER) {
            hwtimer_spin(ticks);
            return 0;
        }
    }

    int ret;
    vtimer_t t;
    mutex_t mutex = MUTEX_INIT;
    mutex_lock(&mutex);

    t.action = vtimer_callback_unlock;
    t.arg = &mutex;
    t.absolute = time;

    ret = vtimer_set(&t);
    mutex_lock(&mutex);
    return ret;
}

int vtimer_remove(vtimer_t *t)
{
    unsigned irq_state = disableIRQ();

    priority_queue_remove(&shortterm_priority_queue_root, timer_get_node(t));
    priority_queue_remove(&longterm_priority_queue_root, timer_get_node(t));
    update_shortterm();

    restoreIRQ(irq_state);

    return 0;
}

int vtimer_set_msg(vtimer_t *t, timex_t interval, kernel_pid_t pid, uint16_t type, void *ptr)
{
    t->action = vtimer_callback_msg;
    t->type = type;
    t->arg = ptr;
    t->absolute = interval;
    t->pid = pid;
    vtimer_set(t);
    return 0;
}

int vtimer_msg_receive_timeout(msg_t *m, timex_t timeout) {
    msg_t timeout_message;
    timeout_message.type = MSG_TIMER;
    timeout_message.content.ptr = (char *) &timeout_message;

    vtimer_t t;
    vtimer_set_msg(&t, timeout, sched_active_pid, MSG_TIMER, &timeout_message);
    msg_receive(m);
    if (m->type == MSG_TIMER && m->content.ptr == (char *) &timeout_message) {
        /* we hit the timeout */
        return -1;
    }
    else {
        vtimer_remove(&t);
        return 1;
    }
}

#if ENABLE_DEBUG

void vtimer_print_short_queue(){
    priority_queue_print(&shortterm_priority_queue_root);
}

void vtimer_print_long_queue(){
    priority_queue_print(&longterm_priority_queue_root);
}

void vtimer_print(vtimer_t *t)
{
    printf("Seconds: %" PRIu32 " - Microseconds: %" PRIu32 "\n \
            action: %p\n \
            arg: %p\n \
            pid: %" PRIkernel_pid "\n",
           t->absolute.seconds, t->absolute.microseconds,
           t->action,
           t->arg,
           t->pid);
}

#endif

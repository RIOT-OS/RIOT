/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 Christian Mehlis <christian.mehlis@fu-berlin.de>
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 *
 * \ingroup system
 * @{
 */

#include <stdio.h>
#include <stdint.h>

#include "msg.h"
#include "thread.h"
#include "hwtimer.h"
#include "sched.h"
#include "cpu.h"
#include "irq.h"

#include "swtimer.h"

#define SWTIMER_OVERHEAD 80
#define SWTIMER_SPIN_THRESHOLD 100

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef ENABLE_DEBUG
#undef SWTIMER_OVERHEAD
#define SWTIMER_OVERHEAD 7500
#endif

/* workaround for buggy mspgcc signal.h */
#undef wakeup

static void swtimer_update_alarm(void);
static void swtimer_action(swtimer_t *swtimer);
static void swtimer_trigger(void* ptr);
static int swtimer_activate(swtimer_t *t);
static void swtimer_priolist_insert(swtimer_t *t);
static void swtimer_update_values(void);

static swtimer_t *swtimer_list = NULL;
static volatile swtime_t system_time = 0;
volatile swtime_t swtimer_next_alarm_absolute = 0;
static volatile unsigned long hwtimer_ticks_left = 0;
static volatile int hwtimer_id = -1;

extern unsigned long hwtimer_now(void);

int swtimer_init(void)
{
    return 0;
}

int swtimer_set(swtimer_t *t, swtime_t interval)
{
    t->interval = interval;
    t->next = NULL;
    swtimer_activate(t);
    return 0;
}

static int swtimer_activate(swtimer_t *t)
{
    DEBUG("swtimer_activate. now=%lu t->interval = %lu hwtimer_ticks=%lu\n", swtimer_now(), t->interval, HWTIMER_TICKS(t->interval));

    if (!inISR())
        dINT();

    if (t->interval <= SWTIMER_OVERHEAD) {
        DEBUG("swtimer_activate: interval too short, triggering right away.\n");
        swtimer_action(t);
        if (!inISR())
            eINT();
        return 0;
    }

    t->start = swtimer_now();

    swtimer_priolist_insert(t);

    if (swtimer_list == t) {
        swtimer_update_values();
        swtimer_update_alarm();
    }

    if (!inISR())
        eINT();

    return 0;
}

static void swtimer_update_values(void)
{
    swtimer_next_alarm_absolute = swtimer_list->start + swtimer_list->interval;
    swtime_t now = swtimer_now();
    swtime_t offset = swtimer_next_alarm_absolute - now;
    hwtimer_ticks_left = HWTIMER_TICKS(offset);

    DEBUG("swtimer_update_values abs: %lu offset: %lu hwtimer_ticks_left: %lu, now=%lu, hwtimer_now=%lu\n", swtimer_next_alarm_absolute, offset, hwtimer_ticks_left, swtimer_now(), hwtimer_now());
}

int swtimer_remove(swtimer_t *t)
{
    if ((!swtimer_list) || (!t)) {
        return -1;
    }

    if (!inISR())
        dINT();
    if (t == swtimer_list) {
        swtimer_list = t->next;
        if (swtimer_list) {
            swtimer_update_values();
            swtimer_update_alarm();
        } else {
            swtimer_next_alarm_absolute = 0;
            hwtimer_ticks_left = 0;
            hwtimer_remove(hwtimer_id);
            hwtimer_id = -1;
        }
    } else {
        swtimer_t *cur = t;
        while (cur) {
            if (cur->next == t) {
                cur->next = cur->next->next;
                break;
            }
            cur = cur->next;
        }
    }
    if (!inISR())
        eINT();
    return 0;
}

swtime_t swtimer_now(void)
{
    swtime_t now = system_time;
    now += HWTIMER_TICKS_TO_US(hwtimer_now());
    return now;
}

int swtimer_set_msg(swtimer_t *t, swtime_t interval, int pid, void *ptr)
{
    t->action_type = SWTIMER_MSG;
    t->action.msg.value = (unsigned int) ptr;
    t->action.msg.target_pid = pid;
    swtimer_set(t, interval);
    return 0;
}

int swtimer_set_wakeup(swtimer_t *t, swtime_t interval, int pid)
{
    t->action_type = SWTIMER_WAKEUP;
    t->action.wakeup.pid = pid;
    swtimer_set(t, interval);
    return 0;
}

static void swtimer_spin(swtime_t us)
{
    swtime_t target = swtimer_now() + us;
    while (target > swtimer_now())
        ;
}

int swtimer_usleep(swtime_t us)
{
    if (inISR()) {
        swtimer_spin(us);
        return 0;
    }
    swtimer_t t;
    t.interval = us;
    t.action_type = SWTIMER_WAKEUP;
    t.action.wakeup.pid = thread_getpid();
    swtimer_activate(&t);
    thread_sleep();
    return 0;
}

static void swtimer_priolist_insert(swtimer_t *t)
{
    t->next = NULL;
    if (swtimer_list == NULL) {
        DEBUG("swtimer: inserting first timer %x\n", (unsigned int)t);
        swtimer_list = t;
    } else {
        DEBUG("swtimer: inserting timer %x\n", (unsigned int)t);
        swtime_t t_absolute = t->start + t->interval;
        swtimer_t *last = NULL;
        swtimer_t *cur = swtimer_list;
        while (cur != NULL) {
            if (t_absolute < (cur->start + cur->interval)) {
                DEBUG("swtimer: timer %x elapses before timer %x\n", (unsigned int) t, (unsigned int) cur);
                t->next = cur;
                if (last) {
                    DEBUG("swtimer: setting ->next of %x to %x\n", (unsigned int) last->next, (unsigned int) t);
                    last->next = t;
                } else {
                    DEBUG("swtimer: %x is first timer now.\n", (unsigned int)t);
                    swtimer_list = t;
                }
                return;
            } else {
                DEBUG("insertF\n");
                if (cur->next) {
                    DEBUG("insertF1\n");
                    last = cur;
                    cur = cur->next;
                } else {
                    DEBUG("insertF2\n");
                    cur->next = t;
                    return;
                }
            }
        }
    }
}

static void swtimer_set_hwtimer(unsigned int offset)
{
    DEBUG("swtimer_set_hwtimer: hwtimer_now: %lu offset:%u\n", hwtimer_now(), offset);
    if (hwtimer_id != -1) {
        hwtimer_remove(hwtimer_id);
    }

    hwtimer_id = hwtimer_set(offset, swtimer_trigger, NULL);
}

static void swtimer_action(swtimer_t *swtimer)
{
    switch (swtimer->action_type) {
    case SWTIMER_WAKEUP: {
        thread_wakeup(swtimer->action.wakeup.pid);
        break;
    }
    case SWTIMER_MSG: {
        msg_t m;
        m.content.value = swtimer->action.msg.value;
        m.type = MSG_TIMER;
        int result = msg_send_int(&m, swtimer->action.msg.target_pid);
        if (result < 0) {
            // error
        }
        break;
    }
    }
}

static void swtimer_trigger(void* ptr)
{
    swtimer_t *next = swtimer_list;
    swtimer_list = swtimer_list->next;
    swtimer_action(next);
    if (!ptr)
        swtimer_update_alarm();
}

static void swtimer_update_alarm(void)
{
    DEBUG("swtimer_check_elapsed: Checking for elapsed timer...\n");

    while (swtimer_list) {
        swtimer_update_values();
        DEBUG("swtimer_check_elapsed: there are timers left to consider. hwtimer_ticks_left=%lu\n", hwtimer_ticks_left);

        if (hwtimer_ticks_left > HWTIMER_MAXTICKS) {
            if ((long int) hwtimer_ticks_left < 0) {
                printf("swtimer_update_alarm: We're late!\n");
            }
            return;
        }

        if (hwtimer_ticks_left < SWTIMER_SPIN_THRESHOLD) {
            DEBUG("swtimer_check_elapsed: spinning..\n");
            if (hwtimer_ticks_left != 0)
                hwtimer_spin(hwtimer_ticks_left);
            DEBUG("swtimer_check_elapsed: spinning done. shooting timer.\n");
            swtimer_trigger((void*) 1); /* flag to prevent recursion */
        } else {
            DEBUG("swtimer_check_elapsed: Setting hwtimer.\n");
            swtimer_set_hwtimer(hwtimer_ticks_left);
            return;
        }
    }
}

/** @} */

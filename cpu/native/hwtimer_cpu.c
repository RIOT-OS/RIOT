/**
 * Native CPU hwtimer_arch.h implementation
 *
 * Uses POSIX real-time extension timers to mimic hardware timers.
 * XXX: see hwtimer_isr_timer()
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup hwtimer
 * @ingroup native_cpu
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @file
 * @}
 */

#include <time.h>
#include <signal.h>
#include <stdint.h>
#include <err.h>
#include "hwtimer.h"
#include "hwtimer_arch.h"

#include "hwtimer_cpu.h"
#include "cpu.h"
#include "cpu-conf.h"

#include "debug.h"

static unsigned long native_hwtimer_now;
static int native_hwtimer_irq[ARCH_MAXTIMERS];
static timer_t native_hwtimer_timer[ARCH_MAXTIMERS];
static void (*int_handler)(int);

/**
 * sets timespec to given ticks
 */
void ticks2ts(unsigned long ticks, struct timespec *tp)
{
   tp->tv_sec = ticks / HWTIMER_SPEED;
   tp->tv_nsec = (ticks % HWTIMER_SPEED)*1000 ;
}

/**
 * returns ticks for give timespec
 */
unsigned long ts2ticks(struct timespec *tp)
{
    /* TODO: check for overflow */
    return((tp->tv_sec * HWTIMER_SPEED) + (tp->tv_nsec/1000));
}

/**
 * native timer signal handler,
 * 
 * XXX: Calls callback for all timers whenever any timer finishes.
 */
void hwtimer_isr_timer()
{
    DEBUG("hwtimer_isr_timer()\n");

    for (int i = 0; i<ARCH_MAXTIMERS; i++) {
        if (native_hwtimer_irq[i] == 1) {
            DEBUG("hwtimer_isr_timer(): calling hwtimer.int_handler(%i)\n", i);
            int_handler(i);
        }
        if (timer_getoverrun(native_hwtimer_timer[i]) > 0 ) {
            errx(1, "XXX: unhandled hwtimer situation");
        }
    }
}

void hwtimer_arch_enable_interrupt(void)
{
    DEBUG("hwtimer_arch_enable_interrupt()\n");
    if (register_interrupt(SIGALRM, hwtimer_isr_timer) != 0) {
        DEBUG("darn!\n\n");
    }
    return;
}

void hwtimer_arch_disable_interrupt(void)
{
    DEBUG("hwtimer_arch_disable_interrupt()\n");
    if (unregister_interrupt(SIGALRM) != 0) {
        DEBUG("darn!\n\n");
    }
    return;
}

void hwtimer_arch_unset(short timer)
{
    struct itimerspec its;

    DEBUG("hwtimer_arch_unset(%d)\n", timer);

    native_hwtimer_irq[timer] = 0;

    its.it_interval.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_value.tv_nsec = 0;
    its.it_value.tv_sec = 0;

    if( (timer_settime(native_hwtimer_timer[timer], 0, &its, NULL)) == -1) {
        err(1, "hwtimer_arch_unset: timer_settime");
    }

    return;
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    struct itimerspec its;

    DEBUG("hwtimer_arch_set(%li, %i)\n", offset, timer);
    native_hwtimer_irq[timer] = 1;

    ticks2ts(offset, &its.it_value);
    DEBUG("hwtimer_arch_set(): that is %lis %lins from now\n", its.it_value.tv_sec, its.it_value.tv_nsec);

    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    if (timer_settime(native_hwtimer_timer[timer], 0, &its, NULL) == -1) {
        err(1, "hwtimer_arch_unset: timer_settime");
    }

    return;
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    DEBUG("hwtimer_arch_set_absolute(%li, %i)\n", value, timer);
    value -= native_hwtimer_now;
    return(hwtimer_arch_set(value, timer));
}

unsigned long hwtimer_arch_now(void)
{
    struct timespec t;

    DEBUG("hwtimer_arch_now()\n");

    if (clock_gettime(CLOCK_MONOTONIC, &t) == -1) {
        err(1, "hwtimer_arch_now: clock_gettime");
    }
    native_hwtimer_now = ts2ticks(&t);

    DEBUG("hwtimer_arch_now(): it is now %lis %lins\n", t.tv_sec, t.tv_nsec);
    DEBUG("hwtimer_arch_now(): returning %li\n", native_hwtimer_now);
    return native_hwtimer_now;
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    struct sigevent sev;

    DEBUG("hwtimer_arch_init()\n");

    hwtimer_arch_disable_interrupt();

    int_handler = handler;

    sev.sigev_notify = SIGEV_SIGNAL;
    for (int i = 0; i<ARCH_MAXTIMERS; i++) {
        native_hwtimer_irq[i] = 0;
        //sev.sigev_signo =  _SIG_TIMER + i;;
        sev.sigev_signo =  SIGALRM;
        //sev.sigev_value.sival_ptr = &native_hwtimer_timer[i];
        if (timer_create(CLOCK_REALTIME, &sev, &native_hwtimer_timer[i]) == -1) {
            err(1, "timer_create");
        }
    }
    hwtimer_arch_enable_interrupt();
    return;
}

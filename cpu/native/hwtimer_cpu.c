/**
 * Native CPU hwtimer_arch.h implementation
 *
 * Uses POSIX realtime clock and POSIX itimer to mimic hardware.
 * Since there is only 1 itmer per process and RIOT needs several
 * hardware timers, hwtimers are being multiplexed onto the itimer.
 *
 * XXX: does not scale well with number of timers (overhead: O(N)).
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup hwtimer
 * @ingroup native_cpu
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @file
 * @}
 */

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <stdint.h>
#include <err.h>
#include "hwtimer.h"
#include "hwtimer_arch.h"

#include "hwtimer_cpu.h"
#include "cpu.h"
#include "cpu-conf.h"

#include "debug.h"

#define HWTIMERMINOFFSET 1000

static unsigned long native_hwtimer_now;

static int native_hwtimer_irq[ARCH_MAXTIMERS];
static struct itimerval native_hwtimer[ARCH_MAXTIMERS];
static int native_hwtimer_isset[ARCH_MAXTIMERS];

static int next_timer;
static void (*int_handler)(int);

/**
 * sets timeval to given ticks
 */
void ticks2tv(unsigned long ticks, struct timeval *tp)
{
    tp->tv_sec = ticks / HWTIMER_SPEED;
    tp->tv_usec = (ticks % HWTIMER_SPEED) ;
}

/**
 * returns ticks for give timeval
 */
unsigned long tv2ticks(struct timeval *tp)
{
    /* TODO: check for overflow */
    return((tp->tv_sec * HWTIMER_SPEED) + (tp->tv_usec));
}

/**
 * returns ticks for give timespec
 */
unsigned long ts2ticks(struct timespec *tp)
{
    /* TODO: check for overflow */
    return((tp->tv_sec * HWTIMER_SPEED) + (tp->tv_nsec / 1000));
}

/**
 * set next_timer to the next lowest enabled timer index
 */
void schedule_timer(void)
{
    int l = next_timer;

    for(
        int i = ((next_timer + 1) % ARCH_MAXTIMERS);
        i != next_timer;
        i = ((i + 1) % ARCH_MAXTIMERS)
    ) {

        if(native_hwtimer_isset[l] != 1) {
            /* make sure we dont compare to garbage in the following
             * if condition */
            l = i;
        }

        if(
            (native_hwtimer_isset[i] == 1) &&
            (tv2ticks(&(native_hwtimer[i].it_value)) < tv2ticks(&(native_hwtimer[l].it_value)))
        ) {
            /* set l to the lowest active time */
            l = i;
        }

    }

    next_timer = l;

    /* l could still point to some unused (garbage) timer if no timers
     * are set at all */
    if(native_hwtimer_isset[next_timer] == 1) {
        if(setitimer(ITIMER_REAL, &native_hwtimer[next_timer], NULL) == -1) {
            err(1, "schedule_timer");
        }
        else {
            DEBUG("schedule_timer(): set next timer.\n");
        }
    }
    else {
        DEBUG("schedule_timer(): no next timer!? This looks suspicous.\n");
        // TODO: unset  timer.
    }
}

/**
 * native timer signal handler
 *
 * set new system timer, call timer interrupt handler
 */
void hwtimer_isr_timer()
{
    int i;

    DEBUG("hwtimer_isr_timer()\n");

    i = next_timer;
    native_hwtimer_isset[next_timer] = 0;
    schedule_timer();

    if(native_hwtimer_irq[i] == 1) {
        DEBUG("hwtimer_isr_timer(): calling hwtimer.int_handler(%i)\n", i);
        int_handler(i);
    }
    else {
        DEBUG("hwtimer_isr_timer(): this should not have happened");
    }
}

void hwtimer_arch_enable_interrupt(void)
{
    DEBUG("hwtimer_arch_enable_interrupt()\n");

    if(register_interrupt(SIGALRM, hwtimer_isr_timer) != 0) {
        DEBUG("darn!\n\n");
    }

    return;
}

void hwtimer_arch_disable_interrupt(void)
{
    DEBUG("hwtimer_arch_disable_interrupt()\n");

    if(unregister_interrupt(SIGALRM) != 0) {
        DEBUG("darn!\n\n");
    }

    return;
}

void hwtimer_arch_unset(short timer)
{
    DEBUG("hwtimer_arch_unset(%d)\n", timer);

    native_hwtimer_irq[timer] = 0;
    native_hwtimer_isset[timer] = 0;
    schedule_timer();

    return;
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    DEBUG("hwtimer_arch_set(%li, %i)\n", offset, timer);

    if(offset < HWTIMERMINOFFSET) {
        offset = HWTIMERMINOFFSET;
        DEBUG("hwtimer_arch_set: offset < MIN, set to: %i\n", offset);
    }

    native_hwtimer_irq[timer] = 1;
    native_hwtimer_isset[timer] = 1;

    ticks2tv(offset, &(native_hwtimer[timer].it_value));
    DEBUG("hwtimer_arch_set(): that is %lis %lius from now\n",
          native_hwtimer[timer].it_value.tv_sec,
          native_hwtimer[timer].it_value.tv_usec);

    schedule_timer();

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

#ifdef __MACH__
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    t.tv_sec = mts.tv_sec;
    t.tv_nsec = mts.tv_nsec;
#else

    if(clock_gettime(CLOCK_MONOTONIC, &t) == -1) {
        err(1, "hwtimer_arch_now: clock_gettime");
    }

#endif

    native_hwtimer_now = ts2ticks(&t);

    DEBUG("hwtimer_arch_now(): it is now %lis %lins\n", t.tv_sec, t.tv_nsec);
    DEBUG("hwtimer_arch_now(): returning %li\n", native_hwtimer_now);
    return native_hwtimer_now;
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    DEBUG("hwtimer_arch_init()\n");

    hwtimer_arch_disable_interrupt();
    int_handler = handler;

    for(int i = 0; i < ARCH_MAXTIMERS; i++) {
        native_hwtimer_irq[i] = 0;
        native_hwtimer_isset[i] = 0;
        native_hwtimer[i].it_interval.tv_sec = 0;
        native_hwtimer[i].it_interval.tv_usec = 0;
    }

    hwtimer_arch_enable_interrupt();
    return;
}

/**
 * Native CPU hwtimer_arch.h implementation
 *
 * Uses POSIX realtime clock and POSIX itimer to mimic hardware.
 * Since there is only 1 itmer per process and RIOT needs several
 * hardware timers, hwtimers are being multiplexed onto the itimer.
 *
 * XXX: does not scale well with number of timers (overhead: O(N)).
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "hwtimer.h"
#include "arch/hwtimer_arch.h"

#include "hwtimer_cpu.h"
#include "cpu.h"
#include "cpu-conf.h"
#include "native_internal.h"

#define ENABLE_DEBUG (0)

#include "debug.h"

#define HWTIMERMINOFFSET (1000UL) // 1ms

static unsigned long native_hwtimer_now;
static unsigned long time_null;

static struct itimerval native_hwtimer[HWTIMER_MAXTIMERS];
static int native_hwtimer_isset[HWTIMER_MAXTIMERS];

static int next_timer = -1;
static void (*int_handler)(int);


/**
 * Subtract the `struct timeval' values x and y, storing the result in
 * result.
 * Return 1 if the difference is negative, otherwise 0.
 *
 * Source:
 * http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
 */
int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /**
     * Compute the time remaining to wait.
     * tv_usec is certainly positive.
     */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}


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
    /* try to find *an active* timer */
    next_timer = -1;
    for (int i = 0; i < HWTIMER_MAXTIMERS; i++) {
        if (native_hwtimer_isset[i] == 1) {
            next_timer = i;
            break;
        }
    }
    if (next_timer == -1) {
        DEBUG("schedule_timer(): no valid timer found - nothing to schedule\n");
        struct itimerval null_timer;
        null_timer.it_interval.tv_sec = 0;
        null_timer.it_interval.tv_usec = 0;
        null_timer.it_value.tv_sec = 0;
        null_timer.it_value.tv_usec = 0;
        if (real_setitimer(ITIMER_REAL, &null_timer, NULL) == -1) {
            err(EXIT_FAILURE, "schedule_timer: setitimer");
        }
        return;
    }

    /* find the next pending timer (next_timer now points to *a* valid pending timer) */
    for (int i = 0; i < HWTIMER_MAXTIMERS; i++) {
        if (
            (native_hwtimer_isset[i] == 1) &&
            (tv2ticks(&(native_hwtimer[i].it_value)) < tv2ticks(&(native_hwtimer[next_timer].it_value)))
        ) {
            /* timer in slot i is active and the timeout is more recent than next_timer */
            next_timer = i;
        }
    }

    /* next pending timer is in slot next_timer */
    struct timeval now;
    hwtimer_arch_now(); // update timer
    ticks2tv(native_hwtimer_now, &now);

    struct itimerval result;
    memset(&result, 0, sizeof(result));

    int retval = timeval_subtract(&result.it_value, &native_hwtimer[next_timer].it_value, &now);
    if (retval || (tv2ticks(&result.it_value) < HWTIMERMINOFFSET)) {
        DEBUG("\033[31mschedule_timer(): timer is already due (%i), mitigating.\033[0m\n", next_timer);
        result.it_value.tv_sec = 0;
        result.it_value.tv_usec = 1;
    }

    _native_syscall_enter();
    if (real_setitimer(ITIMER_REAL, &result, NULL) == -1) {
        err(EXIT_FAILURE, "schedule_timer: setitimer");
    }
    else {
        DEBUG("schedule_timer(): set next timer (%i).\n", next_timer);
    }
    _native_syscall_leave();
}

/**
 * native timer signal handler
 *
 * set new system timer, call timer interrupt handler
 */
void hwtimer_isr_timer(void)
{
    DEBUG("hwtimer_isr_timer()\n");

    if (next_timer == -1) {
        DEBUG("hwtimer_isr_timer(): next_timer is invalid\n");
        return;
    }

    if (native_hwtimer_isset[next_timer] == 1) {
        native_hwtimer_isset[next_timer] = 0;
        DEBUG("hwtimer_isr_timer(): calling hwtimer.int_handler(%i)\n", next_timer);
        int_handler(next_timer);
    }
    else {
        DEBUG("hwtimer_isr_timer(): this should not have happened\n");
    }

    schedule_timer();
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
    DEBUG("hwtimer_arch_unset(\033[31m%i\033[0m)\n", timer);

    native_hwtimer_isset[timer] = 0;
    schedule_timer();

    return;
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    DEBUG("hwtimer_arch_set(%lu, \033[31m%i\033[0m)\n", offset, timer);

    hwtimer_arch_now(); /* update native_hwtimer_now */
    offset += native_hwtimer_now;
    hwtimer_arch_set_absolute(offset, timer);

    return;
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    DEBUG("hwtimer_arch_set_absolute(%lu, %i)\n", value, timer);

    ticks2tv(value, &(native_hwtimer[timer].it_value));

    DEBUG("hwtimer_arch_set_absolute(): that is at %lu s %lu us\n",
          (unsigned long)native_hwtimer[timer].it_value.tv_sec,
          (unsigned long)native_hwtimer[timer].it_value.tv_usec);

    native_hwtimer_isset[timer] = 1;
    schedule_timer();

    return;
}

unsigned long hwtimer_arch_now(void)
{
    struct timespec t;

    DEBUG("hwtimer_arch_now()\n");

    _native_syscall_enter();
#ifdef __MACH__
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    t.tv_sec = mts.tv_sec;
    t.tv_nsec = mts.tv_nsec;
#else

    if (real_clock_gettime(CLOCK_MONOTONIC, &t) == -1) {
        err(EXIT_FAILURE, "hwtimer_arch_now: clock_gettime");
    }

#endif
    _native_syscall_leave();

    native_hwtimer_now = ts2ticks(&t) - time_null;

    struct timeval tv;
    ticks2tv(native_hwtimer_now, &tv);
    DEBUG("hwtimer_arch_now(): it is now %lu s %lu us\n",
            (unsigned long)tv.tv_sec, (unsigned long)tv.tv_usec);
    DEBUG("hwtimer_arch_now(): returning %lu\n", native_hwtimer_now);
    return native_hwtimer_now;
}

/**
 * Called once on process creation in order to mimic the behaviour a
 * regular hardware timer.
 */
void native_hwtimer_pre_init(void)
{
    /* initialize time delta */
    time_null = 0;
    time_null = hwtimer_arch_now();
    /* need to call hwtimer_arch_now as hwtimer_arch_now uses
     * time_null to delta native_hwtimer_now: */
    hwtimer_arch_now();
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    DEBUG("hwtimer_arch_init()\n");

    (void) fcpu;
    hwtimer_arch_disable_interrupt();
    int_handler = handler;

    for (int i = 0; i < HWTIMER_MAXTIMERS; i++) {
        native_hwtimer_isset[i] = 0;
        native_hwtimer[i].it_interval.tv_sec = 0;
        native_hwtimer[i].it_interval.tv_usec = 0;
    }

    hwtimer_arch_enable_interrupt();
    return;
}

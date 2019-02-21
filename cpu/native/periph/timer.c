/**
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_native
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Native CPU periph/timer.h implementation
 *
 * Uses POSIX realtime clock and POSIX itimer to mimic hardware.
 *
 * This is based on native's hwtimer implementation by Ludwig Knüpfer.
 * I removed the multiplexing, as xtimer does the same. (kaspar)
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach_init.h>
#include <mach/mach_port.h>
#include <mach/mach_host.h>
/* Both OS X and RIOT typedef thread_t. timer.c does not use either thread_t. */
#define thread_t riot_thread_t
#endif

#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "cpu.h"
#include "cpu_conf.h"
#include "native_internal.h"
#include "periph/timer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define NATIVE_TIMER_SPEED 1000000

static unsigned long time_null;

static timer_cb_t _callback;
static void *_cb_arg;

static struct itimerval itv;

/**
 * returns ticks for give timespec
 */
static unsigned long ts2ticks(struct timespec *tp)
{
    /* TODO: check for overflow */
    return(((unsigned long)tp->tv_sec * NATIVE_TIMER_SPEED) + (tp->tv_nsec / 1000));
}

/**
 * native timer signal handler
 *
 * set new system timer, call timer interrupt handler
 */
void native_isr_timer(void)
{
    DEBUG("%s\n", __func__);

    _callback(_cb_arg, 0);
}

int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
    (void)freq;
    DEBUG("%s\n", __func__);
    if (dev >= TIMER_NUMOF) {
        return -1;
    }
    if (freq != NATIVE_TIMER_SPEED) {
        return -1;
    }

    /* initialize time delta */
    time_null = 0;
    time_null = timer_read(0);

    _callback = cb;
    _cb_arg = arg;
    if (register_interrupt(SIGALRM, native_isr_timer) != 0) {
        DEBUG("darn!\n\n");
    }

    return 0;
}

static void do_timer_set(unsigned int offset)
{
    DEBUG("%s\n", __func__);

    if (offset && offset < NATIVE_TIMER_MIN_RES) {
        offset = NATIVE_TIMER_MIN_RES;
    }

    memset(&itv, 0, sizeof(itv));
    itv.it_value.tv_sec = (offset / 1000000);
    itv.it_value.tv_usec = offset % 1000000;

    DEBUG("timer_set(): setting %u.%06u\n", (unsigned)itv.it_value.tv_sec, (unsigned)itv.it_value.tv_usec);

    _native_syscall_enter();
    if (real_setitimer(ITIMER_REAL, &itv, NULL) == -1) {
        err(EXIT_FAILURE, "timer_arm: setitimer");
    }
    _native_syscall_leave();
}

int timer_set(tim_t dev, int channel, unsigned int offset)
{
    (void)dev;
    DEBUG("%s\n", __func__);

    if (channel != 0) {
        return -1;
    }

    if (!offset) {
        offset = NATIVE_TIMER_MIN_RES;
    }

    do_timer_set(offset);

    return 1;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    uint32_t now = timer_read(dev);
    return timer_set(dev, channel, value - now);
}

int timer_clear(tim_t dev, int channel)
{
    (void)dev;
    (void)channel;

    do_timer_set(0);

    return 1;
}

void timer_start(tim_t dev)
{
    (void)dev;
    DEBUG("%s\n", __func__);
}

void timer_stop(tim_t dev)
{
    (void)dev;
    DEBUG("%s\n", __func__);
}

unsigned int timer_read(tim_t dev)
{
    if (dev >= TIMER_NUMOF) {
        return 0;
    }

    struct timespec t;

    DEBUG("timer_read()\n");

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
        err(EXIT_FAILURE, "timer_read: clock_gettime");
    }

#endif
    _native_syscall_leave();

    return ts2ticks(&t) - time_null;
}

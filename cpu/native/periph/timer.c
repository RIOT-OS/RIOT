/**
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup timer
 * @ingroup native_cpu
 * @{
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @file
 * @brief Native CPU periph/timer.h implementation
 *
 * Uses POSIX realtime clock and POSIX per-process timer to mimic hardware.
 *
 * This is based on native's hwtimer implementation by Ludwig Knüpfer.
 * I removed the multiplexing, as xtimer does the same. (kaspar)
 *
 * @}
 */

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach_init.h>
#include <mach/mach_port.h>
#include <mach/mach_host.h>
#endif

#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "assert.h"
#include "cpu.h"
#include "cpu_conf.h"
#include "native_internal.h"
#include "periph/timer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define NATIVE_TIMER_SPEED 1000000

static unsigned long time_null[TIMER_NUMOF];

static void (*_callback[TIMER_NUMOF])(int);

static struct sigevent evt[TIMER_NUMOF];
static timer_t tid[TIMER_NUMOF];
static struct itimerspec itv[TIMER_NUMOF];

/**
 * returns ticks for give timespec
 */
static unsigned long ts2ticks(struct timespec *tp)
{
    /* TODO: check for overflow */
    return((tp->tv_sec * NATIVE_TIMER_SPEED) + (tp->tv_nsec / 1000));
}

/**
 * native timer signal handler for TIMER0
 */
void native_isr_timer0(void)
{
    DEBUG("%s\n", __func__);

    _callback[0](0);
}

/**
 * native timer signal handler for TIMER1
 */
void native_isr_timer1(void)
{
    DEBUG("%s\n", __func__);

    _callback[1](1);
}

int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    (void) ticks_per_us;
    assert(ticks_per_us != 1);

    DEBUG("%s\n", __func__);
    if (dev >= TIMER_NUMOF) {
        return -1;
    }

    /* initialize time delta */
    time_null[dev] = 0;
    time_null[dev] = timer_read(dev);

    evt[dev].sigev_notify = SIGEV_SIGNAL;
    evt[dev].sigev_signo = SIGRTMIN + dev;
    evt[dev].sigev_value.sival_ptr = &tid[dev];

    timer_irq_disable(dev);
    _callback[dev] = callback;
    timer_irq_enable(dev);

    if (timer_create(CLOCK_REALTIME, &evt[dev], &tid[dev]) == -1) {
        return -1;
    }

    return 0;
}

static void do_timer_set(tim_t dev, unsigned int offset)
{
    DEBUG("%s: %u\n", __func__, offset);

    if (offset && offset < NATIVE_TIMER_MIN_RES) {
        offset = NATIVE_TIMER_MIN_RES;
    }

    memset(&itv[dev], 0, sizeof(itv[dev]));
    itv[dev].it_value.tv_sec = (offset / 1000000);
    itv[dev].it_value.tv_nsec = (offset % 1000000) * 1000;

    DEBUG("timer_set(): setting %u.%06u\n", (unsigned)itv[dev].it_value.tv_sec, (unsigned)itv[dev].it_value.tv_nsec);

    _native_syscall_enter();
    if (real_timer_settime(tid[dev], 0, &itv[dev], NULL) == -1) {
        err(EXIT_FAILURE, "timer_arm: timer_settime");
    }
    _native_syscall_leave();
}

int timer_set(tim_t dev, int channel, unsigned int offset)
{
    DEBUG("%s\n", __func__);

    if (channel != 0) {
        return -1;
    }

    if (!offset) {
        offset = NATIVE_TIMER_MIN_RES;
    }

    do_timer_set(dev, offset);

    return 1;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    uint32_t now = timer_read(dev);
    return timer_set(dev, channel, value - now);
}

int timer_clear(tim_t dev, int channel)
{
    (void)channel;

    do_timer_set(dev, 0);

    return 1;
}

void timer_irq_enable(tim_t dev)
{
    DEBUG("%s\n", __func__);

    switch (dev) {
        case 0:
            if (register_interrupt(SIGRTMIN + dev, native_isr_timer0) != 0) {
                DEBUG("darn!\n\n");
            }
            break;
        case 1:
            if (register_interrupt(SIGRTMIN + dev, native_isr_timer1) != 0) {
                DEBUG("darn!\n\n");
            }
            break;
        default:
            break;
    }

    return;
}

void timer_irq_disable(tim_t dev)
{
    DEBUG("%s\n", __func__);

    if (unregister_interrupt(SIGRTMIN + dev) != 0) {
        DEBUG("darn!\n\n");
    }

    return;
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
    DEBUG("timer_read: %lu\n", (ts2ticks(&t) - time_null[dev]));

#endif
    _native_syscall_leave();

    return ts2ticks(&t) - time_null[dev];
}

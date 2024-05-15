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
 * I removed the multiplexing, as ztimer does the same. (kaspar)
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "cpu.h"
#include "cpu_conf.h"
#include "native_internal.h"
#include "panic.h"
#include "periph/timer.h"
#include "time_units.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define NATIVE_TIMER_SPEED 1000000

static unsigned long time_null;

static timer_cb_t _callback;
static void *_cb_arg;

static struct itimerspec its;

static timer_t itimer_monotonic;

/**
 * returns ticks for give timespec
 */
static unsigned long ts2ticks(struct timespec *tp)
{
    /* TODO: check for overflow */
    return (((unsigned long)tp->tv_sec * NATIVE_TIMER_SPEED) + (tp->tv_nsec / 1000));
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

uword_t timer_query_freqs_numof(tim_t dev)
{
    (void)dev;

    assert(TIMER_DEV(dev) < TIMER_NUMOF);

    return 1;
}

uint32_t timer_query_freqs(tim_t dev, uword_t index)
{
    (void)dev;

    assert(TIMER_DEV(dev) < TIMER_NUMOF);

    if (index > 0) {
        return 0;
    }

    return NATIVE_TIMER_SPEED;
}

int timer_init(tim_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
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

    //@@if (timer_create(CLOCK_MONOTONIC, NULL, &itimer_monotonic) != 0) {
    if (0) {//@@
        DEBUG_PUTS("Failed to create a monotonic itimer");
        return -1;
    }

    if (register_interrupt(SIGALRM, native_isr_timer) != 0) {
        DEBUG_PUTS("Failed to register SIGALRM handler");
        //@@timer_delete(itimer_monotonic);
        (void)itimer_monotonic;//@@
        return -1;
    }

    return 0;
}

static void do_timer_set(unsigned int offset, bool periodic)
{
    DEBUG("%s\n", __func__);

    if (offset && offset < NATIVE_TIMER_MIN_RES) {
        offset = NATIVE_TIMER_MIN_RES;
    }

    memset(&its, 0, sizeof(its));
    its.it_value.tv_sec = offset / NATIVE_TIMER_SPEED;
    its.it_value.tv_nsec = (offset % NATIVE_TIMER_SPEED) * (NS_PER_SEC / NATIVE_TIMER_SPEED);
    if (periodic) {
        its.it_interval = its.it_value;
    }

    DEBUG("timer_set(): setting %lu.%09lu\n", (unsigned long)its.it_value.tv_sec,
          (unsigned long)its.it_value.tv_nsec);
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

    do_timer_set(offset, false);
    timer_start(dev);

    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    unsigned int now = timer_read(dev);
    return timer_set(dev, channel, value - now);
}

int timer_set_periodic(tim_t dev, int channel, unsigned int value, uint8_t flags)
{
    if (channel != 0) {
        return -1;
    }

    do_timer_set(value, true);

    if (!(flags & TIM_FLAG_SET_STOPPED)) {
        timer_start(dev);
    }

    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    (void)channel;

    do_timer_set(0, false);
    timer_start(dev);

    return 0;
}

void timer_start(tim_t dev)
{
    (void)dev;
    DEBUG("%s\n", __func__);

    _native_syscall_enter();
    //@@if (timer_settime(itimer_monotonic, 0, &its, NULL) == -1) {
    if (0) {//@@
        core_panic(PANIC_GENERAL_ERROR, "Failed to set monotonic timer");
    }
    _native_syscall_leave();
}

void timer_stop(tim_t dev)
{
    (void)dev;
    DEBUG("%s\n", __func__);

    _native_syscall_enter();
    struct itimerspec zero = {0};
    //@@if (timer_settime(itimer_monotonic, 0, &zero, &its) == -1) {
    if (0) {(void)zero;//@@
        core_panic(PANIC_GENERAL_ERROR, "Failed to set monotonic timer");
    }
    _native_syscall_leave();

    DEBUG("time left: %lu.%09lu\n", (unsigned long)its.it_value.tv_sec, its.it_value.tv_nsec);
}

unsigned int timer_read(tim_t dev)
{
    if (dev >= TIMER_NUMOF) {
        return 0;
    }

    struct timespec t;

    DEBUG("timer_read()\n");

    _native_syscall_enter();

    if (clock_gettime(CLOCK_MONOTONIC, &t) == -1) {
        core_panic(PANIC_GENERAL_ERROR, "Failed to read monotonic clock");
    }

    _native_syscall_leave();

    return ts2ticks(&t) - time_null;
}

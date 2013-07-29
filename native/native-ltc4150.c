/**
 * Native Board ltc4150_arch.h implementation
 *
 * Only measures time at the moment. Uses POSIX real-time extension
 * timer to generate periodic signal/interrupt.
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup native_board
 * @ingroup ltc4150
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <err.h>

#include "ltc4150_arch.h"
#include "debug.h"

#include "cpu.h"
#include "cpu-conf.h"

#define native_ltc4150_startup_delay 10

static timer_t native_ltc4150_timer;
static struct itimerspec native_ltc4150_timer_time;

/**
 * unregister signal handler
 */
void ltc4150_disable_int(void)
{
    DEBUG("ltc4150_disable_int()\n");
    unregister_interrupt(_SIG_LTC4150);
}

/**
 * register signal handler
 */
void ltc4150_enable_int(void)
{
    DEBUG("ltc4150_enable_int()\n");
    register_interrupt(_SIG_LTC4150, ltc4150_interrupt);
}

/**
 * elaborate nop
 */
void ltc4150_sync_blocking(void)
{
    DEBUG("ltc4150_sync_blocking()\n");

    for (int i = native_ltc4150_startup_delay; i > 0; i--);
}

/**
 * set up posix real-time timer to simulate coloumb counter ticks
 */
void ltc4150_arch_init(void)
{
    struct sigevent sev;

    ltc4150_disable_int();

    /* create timer */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = _SIG_LTC4150;
    sev.sigev_value.sival_ptr = &native_ltc4150_timer;

    if (timer_create(CLOCK_MONOTONIC, &sev, &native_ltc4150_timer) == -1) {
        err(1, "ltc4150_arch_init(): timer_create");
    }

    /* set timer */
    native_ltc4150_timer_time.it_value.tv_sec = 0;
    native_ltc4150_timer_time.it_value.tv_nsec = 100000000;
    native_ltc4150_timer_time.it_interval.tv_sec = 0;
    native_ltc4150_timer_time.it_interval.tv_nsec = 100000000;

    if (timer_settime(native_ltc4150_timer, 0, &native_ltc4150_timer_time, NULL) == -1) {
        err(1, "ltc4150_arch_init: timer_settime");
    }

    puts("Native LTC4150 initialized.");
}
/** @} */

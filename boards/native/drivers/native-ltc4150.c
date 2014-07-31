/**
 * Native Board ltc4150_arch.h implementation
 *
 * Only measures time at the moment. Uses POSIX real-time extension
 * timer to generate periodic signal/interrupt.
 *
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#include "cpu.h"
#include "cpu-conf.h"
#include "hwtimer.h"
#include "irq.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define LTC_TIMER_INTERVAL (10 * 1000UL) // 10 ms

static int _native_ltc_hwtimer_id = -1;

/**
 * native ltc4150 hwtimer - interrupt handler proxy
 */
static void _native_ltc_int_handler(void *param)
{
    (void) param;
    DEBUG("_native_ltc_int_handler()\n");
    if (_native_ltc_hwtimer_id != -1) {
        ltc4150_interrupt();
        _native_ltc_hwtimer_id = hwtimer_set(LTC_TIMER_INTERVAL, _native_ltc_int_handler, NULL);
        if (_native_ltc_hwtimer_id == -1) {
            errx(1, "_int_handler: hwtimer_set");
        }
        else {
            DEBUG("_native_ltc_int_handler: _native_ltc_hwtimer_id is  %d\n", _native_ltc_hwtimer_id);
        }
    }
    else {
        DEBUG("_native_ltc_int_handler was called although no hwtimer is set\n");
    }
}

/**
 * unregister signal handler
 */
void ltc4150_disable_int(void)
{
    unsigned state = disableIRQ();
    DEBUG("ltc4150_disable_int()\n");
    if (_native_ltc_hwtimer_id != -1) {
        hwtimer_remove(_native_ltc_hwtimer_id);
        _native_ltc_hwtimer_id = -1;
    }
    restoreIRQ(state);
}

/**
 * register signal handler
 */
void ltc4150_enable_int(void)
{
    DEBUG("ltc4150_enable_int()\n");
    _native_ltc_hwtimer_id = hwtimer_set(LTC_TIMER_INTERVAL, _native_ltc_int_handler, NULL);
    if (_native_ltc_hwtimer_id == -1) {
        errx(1, "ltc4150_enable_int: hwtimer_set");
    }
    else {
        DEBUG("ltc4150_enable_int: _native_ltc_hwtimer_id is %d\n", _native_ltc_hwtimer_id);
    }
}

/**
 * elaborate nop
 */
void ltc4150_sync_blocking(void)
{
    DEBUG("ltc4150_sync_blocking()\n");
}

/**
 * set up posix real-time timer to simulate coloumb counter ticks
 */
void ltc4150_arch_init(void)
{
    ltc4150_disable_int();

    puts("Native LTC4150 initialized.");
}
/** @} */

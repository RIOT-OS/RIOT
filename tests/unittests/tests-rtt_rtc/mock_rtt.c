/*
 * SPDX-FileCopyrightText: 2020 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Mock implementation of a Real-Time Timer
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#include <stddef.h>
#include "periph/rtt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static uint32_t counter, alarm;
static rtt_cb_t alarm_cb, overflow_cb;
static void *alarm_arg, *overflow_arg;

uint32_t rtt_get_counter(void)
{
    DEBUG("%s() = %"PRIu32"\n", __func__, counter);
    return counter;
}

void rtt_set_counter(uint32_t _counter)
{
    DEBUG("%s(%"PRIu32")\n", __func__, _counter);
    counter = _counter & RTT_MAX_VALUE;
}

void rtt_set_alarm(uint32_t _alarm, rtt_cb_t cb, void *arg)
{
    DEBUG("%s(%"PRIu32")\n", __func__, _alarm);

    alarm     = _alarm & RTT_MAX_VALUE;
    alarm_cb  = cb;
    alarm_arg = arg;
}

uint32_t rtt_get_alarm(void)
{
    return alarm;
}

void rtt_clear_alarm(void)
{
    alarm_cb = NULL;
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    DEBUG("%s()\n", __func__);
    overflow_cb  = cb;
    overflow_arg = arg;
}

void rtt_clear_overflow_cb(void)
{
    overflow_cb = NULL;
}

void rtt_init(void)
{
    counter = 0;
}

static void _tick(void)
{
    counter = (counter + 1) & RTT_MAX_VALUE;

    if (overflow_cb && (counter == 0)) {
        DEBUG("RTT: overflow\n");
        overflow_cb(overflow_arg);
    }

    if (alarm_cb && (counter == alarm)) {
        DEBUG("RTT: alarm\n");
        alarm_cb(alarm_arg);
    }
}

void rtt_add_ticks(uint64_t ticks)
{
    while (ticks--) {
        _tick();
    }
}
/** @} */

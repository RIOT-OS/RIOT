/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_rtt64
 * @{
 *
 * @file
 * @brief       RTT extension for use in timekeeping
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "irq.h"
#include "rtt64.h"
#include "bitarithm.h"
#include "time_units.h"

#define ENABLE_DEBUG    0
#include "debug.h"

/**
 * @brief Number of bits available for the sub-second fraction
 */
#if RTT_FREQUENCY == 1
#define RTT_SUBSEC_BITS 0
#elif RTT_FREQUENCY == 2
#define RTT_SUBSEC_BITS 1
#elif RTT_FREQUENCY == 4
#define RTT_SUBSEC_BITS 2
#elif RTT_FREQUENCY == 8
#define RTT_SUBSEC_BITS 3
#elif RTT_FREQUENCY == 16
#define RTT_SUBSEC_BITS 4
#elif RTT_FREQUENCY == 32
#define RTT_SUBSEC_BITS 5
#elif RTT_FREQUENCY == 64
#define RTT_SUBSEC_BITS 6
#elif RTT_FREQUENCY == 128
#define RTT_SUBSEC_BITS 7
#elif RTT_FREQUENCY == 256
#define RTT_SUBSEC_BITS 8
#elif RTT_FREQUENCY == 512
#define RTT_SUBSEC_BITS 9
#elif RTT_FREQUENCY == 1024
#define RTT_SUBSEC_BITS 10
#elif RTT_FREQUENCY == 2048
#define RTT_SUBSEC_BITS 11
#elif RTT_FREQUENCY == 4096
#define RTT_SUBSEC_BITS 12
#elif RTT_FREQUENCY == 8192
#define RTT_SUBSEC_BITS 13
#elif RTT_FREQUENCY == 16384
#define RTT_SUBSEC_BITS 14
#elif RTT_FREQUENCY == 32768
#define RTT_SUBSEC_BITS 15
#elif RTT_FREQUENCY == 65536
#define RTT_SUBSEC_BITS 16
#else
#error Invalid RTT frequency (must be power of 2)
#endif

/**
 * @brief   Width of the RTT counter
 */
#define RTT_SHIFT   (1 + bitarithm_msb(RTT_MAX_VALUE))

/**
 * @brief fallback if not backup ram is available
 */
#ifndef BACKUP_RAM
#define BACKUP_RAM
#endif

static uint32_t overflows BACKUP_RAM;
static uint32_t alarm_overflows BACKUP_RAM;
static rtt_cb_t alarm_cb;
static void *alarm_cb_arg;

static void _overflow_cb(void *arg)
{
    (void)arg;
    ++overflows;

    if (alarm_overflows && --alarm_overflows == 0) {
        rtt_cb_t cb = alarm_cb;
        alarm_cb = NULL;
        cb(alarm_cb_arg);
    }
}

static uint64_t _time_to_counter(uint64_t secs, uint32_t us)
{
    uint64_t now = secs << 16;
    now |= ((uint64_t)us * 0xFFFF) / US_PER_SEC;

    return now;
}

static void _counter_to_time(uint64_t now, uint64_t *secs, uint32_t *us)
{
    *secs = now >> 16;
    *us = ((uint64_t)US_PER_SEC * (now & 0xFFFF)) >> 16;
}

void rtt64_init(void)
{
    rtt_set_overflow_cb(_overflow_cb, NULL);
}

void rtt64_set_counter(uint64_t now)
{
    unsigned state = irq_disable();

    rtt_set_counter(now >> (16 - RTT_SUBSEC_BITS));
    overflows = now >> RTT_SHIFT;

    irq_restore(state);
}

uint64_t rtt64_get_counter(void)
{
    uint64_t now;
    uint32_t overflows_prev;

    do {
        unsigned state = irq_disable();
        overflows_prev = overflows;

        now = (uint64_t)overflows << RTT_SHIFT;
        now |= (uint64_t)rtt_get_counter() << (16 - RTT_SUBSEC_BITS);

        irq_restore(state);
    } while (overflows != overflows_prev);

    return now;
}

void rtt64_set_alarm_counter(uint64_t alarm, rtt_cb_t cb, void *arg)
{
    unsigned state = irq_disable();

    alarm_cb = cb;
    alarm_cb_arg = arg;

    rtt_set_alarm(alarm & RTT_MAX_VALUE, cb, arg);
    alarm_overflows = alarm >> RTT_SHIFT;

    irq_restore(state);
}

uint64_t rtt64_get_alarm_counter(void)
{
    unsigned state = irq_disable();

    uint64_t alarm = (uint64_t)(overflows + alarm_overflows) << RTT_SHIFT;
    alarm |= (uint64_t)rtt_get_alarm() << (16 - RTT_SUBSEC_BITS);

    irq_restore(state);
    return alarm;
}

void rtt64_clear_alarm(void)
{
    rtt_clear_alarm();
    alarm_cb = NULL;
    alarm_overflows = 0;
}

void rtt64_get_time(uint64_t *secs, uint32_t *us)
{
    uint64_t now = rtt64_get_counter();

    _counter_to_time(now, secs, us);
}

void rtt64_set_time(uint64_t secs, uint32_t us)
{
    rtt64_set_counter(_time_to_counter(secs, us));
}

void rtt64_get_alarm_time(uint64_t *secs, uint32_t *us)
{
    uint64_t alarm = rtt64_get_alarm_counter();

    return _counter_to_time(alarm, secs, us);
}

void rtt64_set_alarm_time(uint64_t secs, uint32_t us, rtt_cb_t cb, void *arg)
{
    rtt64_set_alarm_counter(_time_to_counter(secs, us), cb, arg);
}

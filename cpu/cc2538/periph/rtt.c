/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @ingroup     drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       RTT implementation sleepmode timer
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "cpu.h"
#include "periph/rtt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define SMWDTHROSC_STLOAD_STLOAD_MASK  (0x00000001)

/* allocate memory for alarm and overflow callbacks + args */
static rtt_cb_t alarm_cb;
static void *alarm_arg;
static rtt_cb_t overflow_cb;
static void *overflow_arg;

static uint32_t rtt_alarm;
static uint32_t rtt_offset;

static enum {
    RTT_ALARM,
    RTT_OVERFLOW
} rtt_next_alarm;

static inline void _rtt_irq_enable(void)
{
    NVIC_SetPriority(SM_TIMER_ALT_IRQn, RTT_IRQ_PRIO);
    NVIC_EnableIRQ(SM_TIMER_ALT_IRQn);
}

static inline void _rtt_irq_disable(void)
{
    NVIC_DisableIRQ(SM_TIMER_ALT_IRQn);
}

void rtt_poweron(void)
{
    /* sleepmode timer is always running so only enable IRQ */
    _rtt_irq_enable();
}

void rtt_poweroff(void)
{
    /* sleepmode timer is always running so only disable IRQ */
    _rtt_irq_disable();
}

void rtt_init(void)
{
    rtt_clear_overflow_cb();
    rtt_clear_alarm();
    rtt_offset = 0;
    rtt_alarm = 0;
    rtt_poweron();
}

static inline uint32_t _rtt_get_counter(void)
{
    return ((SMWDTHROSC_ST0 & 0xFF)
         | ((SMWDTHROSC_ST1 & 0xFF) << 8)
         | ((SMWDTHROSC_ST2 & 0xFF) << 16)
         | ((SMWDTHROSC_ST3 & 0xFF) << 24));
}

uint32_t rtt_get_counter(void)
{
    return _rtt_get_counter() - rtt_offset;
}

void rtt_set_counter(uint32_t counter)
{
    rtt_alarm -= rtt_offset;
    rtt_offset = _rtt_get_counter() + counter;
    rtt_alarm += rtt_offset;

    /* re-set the overflow callback */
    if (overflow_cb) {
        rtt_set_overflow_cb(overflow_cb, overflow_arg);
    }
}

static void _set_alarm(uint32_t alarm)
{
    while (!(SMWDTHROSC_STLOAD & SMWDTHROSC_STLOAD_STLOAD_MASK)) {}
    SMWDTHROSC_ST3 = (alarm >> 24) & 0xFF;
    SMWDTHROSC_ST2 = (alarm >> 16) & 0xFF;
    SMWDTHROSC_ST1 = (alarm >>  8) & 0xFF;
    SMWDTHROSC_ST0 = alarm & 0xFF;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    assert(cb && !(alarm & ~RTT_MAX_VALUE));

    unsigned irq = irq_disable();
    rtt_alarm = alarm + rtt_offset;

    /* set callback*/
    alarm_cb = cb;
    alarm_arg = arg;

    DEBUG("rtt_set_alarm(%lu), alarm in %lu ticks, overflow in %lu ticks\n",
          alarm, rtt_alarm - _rtt_get_counter(),
          rtt_offset - _rtt_get_counter());

    /* only set overflow alarm if it happens before the scheduled alarm */
    if (overflow_cb == NULL || (rtt_offset >= rtt_alarm )) {
        rtt_next_alarm = RTT_ALARM;
        _set_alarm(rtt_alarm);
    }

    irq_restore(irq);
}

uint32_t rtt_get_alarm(void)
{
    return rtt_alarm - rtt_offset;
}

void rtt_clear_alarm(void)
{
    unsigned irq = irq_disable();

    alarm_cb = NULL;
    alarm_arg = NULL;
    irq_restore(irq);
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    unsigned irq = irq_disable();

    /* set callback*/
    overflow_cb = cb;
    overflow_arg = arg;

    /* only set overflow alarm if it happens before the scheduled alarm */
    if (alarm_cb == NULL || (rtt_alarm > rtt_offset)) {
        rtt_next_alarm = RTT_OVERFLOW;
        _set_alarm(rtt_offset);
    }

    irq_restore(irq);
}

void rtt_clear_overflow_cb(void)
{
    unsigned irq = irq_disable();

    overflow_cb = NULL;
    overflow_arg = NULL;
    irq_restore(irq);
}

void isr_sleepmode(void)
{
    rtt_cb_t tmp;
    bool both = (rtt_alarm == rtt_offset);

    switch (rtt_next_alarm) {
    case RTT_ALARM:
        if (alarm_cb) {
            /* 'consume' the callback (as it might be set again in the cb) */
            tmp = alarm_cb;
            alarm_cb = NULL;
            tmp(alarm_arg);

        }
        if (!both) {
            break;
        } /* fall-through */
    case RTT_OVERFLOW:
        /* 'consume' the callback (as it might be set again in the cb) */
        if (overflow_cb) {
            /* 'consume' the callback (as it might be set again in the cb) */
            tmp = overflow_cb;
            overflow_cb = NULL;
            tmp(overflow_arg);
        }
        break;
    }

    if (alarm_cb && (rtt_offset >= rtt_alarm)) {
        DEBUG("rtt: next alarm in %lu ticks (RTT)\n",
              rtt_alarm - _rtt_get_counter());

        rtt_next_alarm = RTT_ALARM;
        _set_alarm(rtt_alarm);
    } else if (overflow_cb && (rtt_alarm > rtt_offset)) {
        DEBUG("rtt: next alarm in %lu ticks (OVERFLOW)\n",
               rtt_offset - _rtt_get_counter());

        rtt_next_alarm = RTT_OVERFLOW;
        _set_alarm(rtt_offset);
    }

    cortexm_isr_end();
}

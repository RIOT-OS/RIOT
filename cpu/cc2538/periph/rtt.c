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

#include <stdint.h>

#include "cpu.h"
#include "periph/rtt.h"

#define SMWDTHROSC_STLOAD_STLOAD_MASK  (0x00000001)

/* allocate memory for alarm and overflow callbacks + args */
static rtt_cb_t alarm_cb = NULL;
static void *alarm_arg;
static rtt_cb_t overflow_cb = NULL;
static void *overflow_arg;

static uint32_t rtt_alarm;
static uint32_t rtt_offset;

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
    rtt_offset = _rtt_get_counter() + counter;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    assert(cb && !(alarm & ~RTT_MAX_VALUE));

    unsigned irq = irq_disable();

    alarm += rtt_offset;

    /* set alarm value */
    while (!(SMWDTHROSC_STLOAD & SMWDTHROSC_STLOAD_STLOAD_MASK)) {}
    SMWDTHROSC_ST3 = (alarm >> 24) & 0xFF;
    SMWDTHROSC_ST2 = (alarm >> 16) & 0xFF;
    SMWDTHROSC_ST1 = (alarm >>  8) & 0xFF;
    SMWDTHROSC_ST0 = alarm & 0xFF;

    rtt_alarm = alarm;

    /* set callback*/
    alarm_cb = cb;
    alarm_arg = arg;

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

    /* set threshold to RTT_MAX_VALUE */
    while (!(SMWDTHROSC_STLOAD & SMWDTHROSC_STLOAD_STLOAD_MASK)) {}
    SMWDTHROSC_ST3 = (rtt_offset >> 24) & 0xFF;
    SMWDTHROSC_ST2 = (rtt_offset >> 16) & 0xFF;
    SMWDTHROSC_ST1 = (rtt_offset >>  8) & 0xFF;
    SMWDTHROSC_ST0 =  rtt_offset & 0xFF;

    /* set callback*/
    overflow_cb = cb;
    overflow_arg = arg;

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
    if (alarm_cb) {
        /* 'consume' the callback (as it might be set again in the cb) */
        rtt_cb_t tmp = alarm_cb;
        alarm_cb = NULL;
        tmp(alarm_arg);
    }
    else if (overflow_cb) {
        /* 'consume' the callback (as it might be set again in the cb) */
        rtt_cb_t tmp = overflow_cb;
        overflow_cb = NULL;
        tmp(overflow_arg);
    }
    cortexm_isr_end();
}

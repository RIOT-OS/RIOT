/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        rtt.c
 * @brief       Low-level RTT driver implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include "periph/rtt.h"
#include "board.h"
#include "thread.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static rtt_cb_t _overflow_cb;
static void* _overflow_arg;

static rtt_cb_t _cmp0_cb;
static void* _cmp0_arg;

void rtt_init(void)
{
    DEBUG("%s:%d\n", __func__, __LINE__);
    rtt_poweron();

    /* reset */
    RTC->MODE0.CTRLA.bit.SWRST = 1;
    while(RTC->MODE0.CTRLA.bit.SWRST);

    /* set 32bit counting mode */
    RTC->MODE0.CTRLA.bit.MODE = 0;

    /* set clock source */
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_ULP32K;

    /* enable */
    RTC->MODE0.CTRLA.bit.ENABLE = 1;
    while(RTC->MODE0.SYNCBUSY.bit.ENABLE);

    /* initially clear flag */
    RTC->MODE0.INTFLAG.bit.CMP0 = 1;

    /* enable RTT IRQ */
    NVIC_EnableIRQ(RTC_IRQn);

    DEBUG("%s:%d %u\n", __func__, __LINE__, (unsigned)rtt_get_counter());
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    DEBUG("%s:%d\n", __func__, __LINE__);
    /* clear overflow cb to avoid race while assigning */
    rtt_clear_overflow_cb();

    /* set callback variables */
    _overflow_cb = cb;
    _overflow_arg = arg;

    /* enable overflow interrupt */
    RTC->MODE0.INTENSET.bit.OVF = 1;
}
void rtt_clear_overflow_cb(void)
{
    DEBUG("%s:%d\n", __func__, __LINE__);
    /* disable overflow interrupt */
    RTC->MODE0.INTENCLR.bit.OVF = 1;
}

uint32_t rtt_get_counter(void)
{
    DEBUG("%s:%d\n", __func__, __LINE__);
    while (RTC->MODE0.SYNCBUSY.bit.COUNT);
    return RTC->MODE0.COUNT.reg;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    DEBUG("%s:%d alarm=%u\n", __func__, __LINE__, (unsigned)alarm);

    /* disable interrupt to avoid race */
    rtt_clear_alarm();

    /* set COM register */
    while (RTC->MODE0.SYNCBUSY.bit.COMP0);
    RTC->MODE0.COMP[0].reg = alarm;

    /* setup callback */
    _cmp0_cb = cb;
    _cmp0_arg = arg;

    /* enable compare interrupt */
    RTC->MODE0.INTENSET.bit.CMP0 = 1;
}

void rtt_clear_alarm(void)
{
    DEBUG("%s:%d\n", __func__, __LINE__);
    /* clear compare interrupt */
    RTC->MODE0.INTENCLR.bit.CMP0 = 1;
}

void rtt_poweron(void)
{
    DEBUG("%s:%d\n", __func__, __LINE__);
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_RTC;
}

void rtt_poweroff(void)
{
    DEBUG("%s:%d\n", __func__, __LINE__);
    MCLK->APBAMASK.reg &= ~MCLK_APBAMASK_RTC;
}

void isr_rtc(void)
{
    if (RTC->MODE0.INTFLAG.bit.OVF) {
        RTC->MODE0.INTFLAG.bit.OVF = 1;
        if (_overflow_cb) {
            _overflow_cb(_overflow_arg);
        }
    }
    if (RTC->MODE0.INTFLAG.bit.CMP0) {
        /* clear flag */
        RTC->MODE0.INTFLAG.bit.CMP0 = 1;
        /* disable interrupt */
        RTC->MODE0.INTENCLR.bit.CMP0 = 1;
        if (_cmp0_cb) {
            _cmp0_cb(_cmp0_arg);
        }
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

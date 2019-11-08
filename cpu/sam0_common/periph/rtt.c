/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_rtt
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
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/*
 * Bit introduced by SAML21xxxB, setting it on SAML21xxxxA too has no ill
 * effects, but simplifies the code. (This bit is always set on SAML21xxxxA)
 */
#ifndef RTC_MODE0_CTRLA_COUNTSYNC
#define RTC_MODE0_CTRLA_COUNTSYNC   BIT15
#endif

static rtt_cb_t _overflow_cb;
static void* _overflow_arg;

static rtt_cb_t _cmp0_cb;
static void* _cmp0_arg;

static void _wait_syncbusy(void)
{
#ifdef REG_RTC_MODE0_SYNCBUSY
    while (RTC->MODE0.SYNCBUSY.reg) {}
#else
    while(RTC->MODE0.STATUS.bit.SYNCBUSY) {}
#endif
}

static inline void _rtt_reset(void)
{
#ifdef RTC_MODE0_CTRL_SWRST
    RTC->MODE0.CTRL.bit.SWRST = 1;
    while (RTC->MODE0.CTRL.bit.SWRST) {}
#else
    RTC->MODE0.CTRLA.bit.SWRST = 1;
    while (RTC->MODE0.CTRLA.bit.SWRST) {}
#endif
}

#ifdef CPU_SAMD21
static void _rtt_clock_setup(void)
{
    /* Setup clock GCLK2 with OSC32K */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(2) | GCLK_CLKCTRL_ID_RTC;
    while (GCLK->STATUS.bit.SYNCBUSY) {}
}
#else
static void _rtt_clock_setup(void)
{
    /* RTC source clock is external oscillator at 32kHz */
#if EXTERNAL_OSC32_SOURCE
    OSC32KCTRL->XOSC32K.bit.EN32K = 1;
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_XOSC32K;

    /* RTC uses internal 32,768KHz Oscillator */
#elif INTERNAL_OSC32_SOURCE
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_OSC32K;

    /* RTC uses Ultra Low Power internal 32,768KHz Oscillator */
#elif ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_ULP32K;

#else
#error "No clock source for RTT selected. "
#endif
}
#endif /* !CPU_SAMD21 - Clock Setup */

void rtt_init(void)
{
    _rtt_clock_setup();
    rtt_poweron();

    _rtt_reset();

    /* set 32bit counting mode & enable the RTC */
#ifdef REG_RTC_MODE0_CTRLA
    RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_MODE(0) | RTC_MODE0_CTRLA_ENABLE | RTC_MODE0_CTRLA_COUNTSYNC;
#else
    RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_MODE(0) | RTC_MODE0_CTRL_ENABLE;
#endif
    _wait_syncbusy();

    /* initially clear flag */
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0
                           |  RTC_MODE0_INTFLAG_OVF;

    NVIC_EnableIRQ(RTC_IRQn);

    DEBUG("%s:%d %u\n", __func__, __LINE__, (unsigned)rtt_get_counter());
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    /* clear overflow cb to avoid race while assigning */
    rtt_clear_overflow_cb();

    /* set callback variables */
    _overflow_cb = cb;
    _overflow_arg = arg;

    /* enable overflow interrupt */
    RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_OVF;
}
void rtt_clear_overflow_cb(void)
{
    /* disable overflow interrupt */
    RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_OVF;
}

uint32_t rtt_get_counter(void)
{
    _wait_syncbusy();
    return RTC->MODE0.COUNT.reg;
}

void rtt_set_counter(uint32_t count)
{
    RTC->MODE0.COUNT.reg = count;
    _wait_syncbusy();
}

uint32_t rtt_get_alarm(void)
{
    _wait_syncbusy();
    return RTC->MODE0.COMP[0].reg;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    DEBUG("%s:%d alarm=%u\n", __func__, __LINE__, (unsigned)alarm);

    /* disable interrupt to avoid race */
    rtt_clear_alarm();

    /* setup callback */
    _cmp0_cb = cb;
    _cmp0_arg = arg;

    /* set COM register */
    RTC->MODE0.COMP[0].reg = alarm;
    _wait_syncbusy();

    /* enable compare interrupt and clear flag */
    RTC->MODE0.INTFLAG.reg  = RTC_MODE0_INTFLAG_CMP0;
    RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP0;
}

void rtt_clear_alarm(void)
{
    /* clear compare interrupt */
    RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_CMP0;
}

void rtt_poweron(void)
{
#ifdef MCLK
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_RTC;
#else
    PM->APBAMASK.reg |= PM_APBAMASK_RTC;
#endif
}

void rtt_poweroff(void)
{
#ifdef MCLK
    MCLK->APBAMASK.reg &= ~MCLK_APBAMASK_RTC;
#else
    PM->APBAMASK.reg &= ~PM_APBAMASK_RTC;
#endif
}

void isr_rtc(void)
{
    if (RTC->MODE0.INTFLAG.bit.OVF) {
        RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_OVF;
        if (_overflow_cb) {
            _overflow_cb(_overflow_arg);
        }
    }
    if (RTC->MODE0.INTFLAG.bit.CMP0) {
        /* clear flag */
        RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
        /* disable interrupt */
        RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_CMP0;
        if (_cmp0_cb) {
            _cmp0_cb(_cmp0_arg);
        }
    }
    cortexm_isr_end();
}

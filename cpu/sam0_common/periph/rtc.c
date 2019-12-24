/*
 * Copyright (C) 2014 Baptiste CLENET
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_rtc
 * @{
 *
 * @file
 * @brief       Low-level RTC driver implementation
 *
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @author       ported to SAML21 by FWX <FWX@dialine.fr>
 * @}
 */

#include <time.h>
#include "cpu.h"
#include "periph/rtc.h"
#include "periph_conf.h"

/* SAML21 rev B needs an extra bit, which in rev A defaults to 1, but isn't
 * visible. Thus define it here. */
#ifndef RTC_MODE2_CTRLA_CLOCKSYNC
#define RTC_MODE2_CTRLA_CLOCKSYNC_Pos   15
#define RTC_MODE2_CTRLA_CLOCKSYNC       (0x1ul << RTC_MODE2_CTRLA_CLOCKSYNC_Pos)
#endif

typedef struct {
    rtc_alarm_cb_t cb;        /**< callback called from RTC interrupt */
    void *arg;                /**< argument passed to the callback */
} rtc_state_t;

static rtc_state_t rtc_callback;

/* At 1Hz, RTC goes till 63 years (2^5, see 17.8.22 in datasheet)
* reference_year is set to 100 (offset) to be in our current time (2000)
* Thanks to this, the user will be able to set time in 2000's*/
static uint16_t reference_year = 100;

static void _wait_syncbusy(void)
{
#ifdef REG_RTC_MODE2_SYNCBUSY
    while (RTC->MODE2.SYNCBUSY.reg) {}
#else
    while (RTC->MODE2.STATUS.bit.SYNCBUSY) {}
#endif
}

static inline void _rtc_set_enabled(bool on)
{
#ifdef REG_RTC_MODE2_CTRLA
    RTC->MODE2.CTRLA.bit.ENABLE = on;
#else
    RTC->MODE2.CTRL.bit.ENABLE = on;
#endif
    _wait_syncbusy();
}

#ifdef CPU_SAMD21
static void _rtc_clock_setup(void)
{
    /* Use 1024 Hz GCLK4 */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(4) | GCLK_CLKCTRL_ID_RTC;
    while (GCLK->STATUS.bit.SYNCBUSY) {}
}
#else
static void _rtc_clock_setup(void)
{
    /* RTC source clock is external oscillator at 1kHz */
#if EXTERNAL_OSC32_SOURCE
    OSC32KCTRL->XOSC32K.bit.EN1K = 1;
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K;

    /* RTC uses internal 32,768KHz Oscillator */
#elif INTERNAL_OSC32_SOURCE
    OSC32KCTRL->OSC32K.bit.EN1K = 1;
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_OSC1K;

    /* RTC uses Ultra Low Power internal 32,768KHz Oscillator */
#elif ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K;

#else
#error "No clock source for RTC selected. "
#endif
}
#endif /* !CPU_SAMD21 - Clock Setup */

void rtc_init(void)
{
    _rtc_clock_setup();
    rtc_poweron();
    _rtc_set_enabled(0);

    /* RTC config with RTC_MODE2_CTRL_CLKREP = 0 (24h) */
#ifdef REG_RTC_MODE2_CTRLA
    RTC->MODE2.CTRLA.reg = RTC_MODE2_CTRLA_PRESCALER_DIV1024   /* CLK_RTC_CNT = 1KHz / 1024 -> 1Hz */
                         | RTC_MODE2_CTRLA_CLOCKSYNC           /* Clock Read Synchronization Enable */
                         | RTC_MODE2_CTRLA_MODE_CLOCK;
#else
    RTC->MODE2.CTRL.reg = RTC_MODE2_CTRL_PRESCALER_DIV1024
                        | RTC_MODE2_CTRL_MODE_CLOCK;
#endif
    RTC->MODE2.INTENSET.reg = RTC_MODE2_INTENSET_OVF;

    /* Clear interrupt flags */
    RTC->MODE2.INTFLAG.reg |= RTC_MODE2_INTFLAG_OVF;
    RTC->MODE2.INTFLAG.reg |= RTC_MODE2_INTFLAG_ALARM0;

    _rtc_set_enabled(1);
}

int rtc_set_time(struct tm *time)
{
    /* normalize input */
    rtc_tm_normalize(time);

    if ((time->tm_year < reference_year) || (time->tm_year > reference_year + 63)) {
        return -1;
    }
    else {
        RTC->MODE2.CLOCK.reg = RTC_MODE2_CLOCK_YEAR(time->tm_year - reference_year)
                             | RTC_MODE2_CLOCK_MONTH(time->tm_mon + 1)
                             | RTC_MODE2_CLOCK_DAY(time->tm_mday)
                             | RTC_MODE2_CLOCK_HOUR(time->tm_hour)
                             | RTC_MODE2_CLOCK_MINUTE(time->tm_min)
                             | RTC_MODE2_CLOCK_SECOND(time->tm_sec);
    }

    _wait_syncbusy();
    return 0;
}

int rtc_get_time(struct tm *time)
{
 RTC_MODE2_CLOCK_Type clock;

    /* Read register in one time */
    clock.reg = RTC->MODE2.CLOCK.reg;

    time->tm_year = clock.bit.YEAR + reference_year;
    if ((time->tm_year < reference_year) || (time->tm_year > (reference_year + 63))) {
        return -1;
    }
    time->tm_mon = clock.bit.MONTH - 1;
    time->tm_mday = clock.bit.DAY;
    time->tm_hour = clock.bit.HOUR;
    time->tm_min = clock.bit.MINUTE;
    time->tm_sec = clock.bit.SECOND;
    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    /* normalize input */
    rtc_tm_normalize(time);

    rtc_clear_alarm();
    if ((time->tm_year < reference_year) || (time->tm_year > (reference_year + 63))) {
        return -2;
    }
    else {
        RTC->MODE2.Mode2Alarm[0].ALARM.reg = RTC_MODE2_ALARM_YEAR(time->tm_year - reference_year)
                                           | RTC_MODE2_ALARM_MONTH(time->tm_mon + 1)
                                           | RTC_MODE2_ALARM_DAY(time->tm_mday)
                                           | RTC_MODE2_ALARM_HOUR(time->tm_hour)
                                           | RTC_MODE2_ALARM_MINUTE(time->tm_min)
                                           | RTC_MODE2_ALARM_SECOND(time->tm_sec);
        RTC->MODE2.Mode2Alarm[0].MASK.reg = RTC_MODE2_MASK_SEL(6);
    }

    _wait_syncbusy();

    /* Setup interrupt */
    NVIC_EnableIRQ(RTC_IRQn);

    /* Enable IRQ */
    rtc_callback.cb = cb;
    rtc_callback.arg = arg;
    RTC->MODE2.INTFLAG.reg |= RTC_MODE2_INTFLAG_ALARM0;
    RTC->MODE2.INTENSET.bit.ALARM0 = 1;

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    RTC_MODE2_ALARM_Type alarm;

    /* Read alarm register in one time */
    alarm.reg = RTC->MODE2.Mode2Alarm[0].ALARM.reg;

    time->tm_year = alarm.bit.YEAR + reference_year;
    if ((time->tm_year < reference_year) || (time->tm_year > (reference_year + 63))) {
        return -1;
    }
    time->tm_mon = alarm.bit.MONTH - 1;
    time->tm_mday = alarm.bit.DAY;
    time->tm_hour = alarm.bit.HOUR;
    time->tm_min = alarm.bit.MINUTE;
    time->tm_sec = alarm.bit.SECOND;

    return 0;
}

void rtc_clear_alarm(void)
{
    /* disable interrupt */
    RTC->MODE2.INTENCLR.bit.ALARM0 = 1;
    rtc_callback.cb = NULL;
    rtc_callback.arg = NULL;
}

void rtc_poweron(void)
{
#ifdef MCLK
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_RTC;
#else
    PM->APBAMASK.reg |= PM_APBAMASK_RTC;
#endif
}

void rtc_poweroff(void)
{
#ifdef MCLK
    MCLK->APBAMASK.reg &= ~MCLK_APBAMASK_RTC;
#else
    PM->APBAMASK.reg &= ~PM_APBAMASK_RTC;
#endif
}

void isr_rtc(void)
{
    if (RTC->MODE2.INTFLAG.bit.ALARM0) {
        rtc_callback.cb(rtc_callback.arg);
        /* clear flag */
        RTC->MODE2.INTFLAG.reg |= RTC_MODE2_INTFLAG_ALARM0;
    }
    if (RTC->MODE2.INTFLAG.bit.OVF) {
        /* clear flag */
        RTC->MODE2.INTFLAG.reg |= RTC_MODE2_INTFLAG_OVF;
        /* At 1Hz, RTC goes till 63 years (2^5, see 17.8.22 in datasheet)
        * Start RTC again with reference_year 64 years more (Be careful with alarm set) */
        reference_year += 64;
    }
    cortexm_isr_end();
}

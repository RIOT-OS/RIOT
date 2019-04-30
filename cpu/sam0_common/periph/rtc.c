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
 * @autor       ported to SAML21 by FWX <FWX@dialine.fr>
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
    /* RTC uses External 32,768KHz Oscillator (OSC32K isn't accurate enough p1075/1138)*/
    SYSCTRL->XOSC32K.reg =  SYSCTRL_XOSC32K_ONDEMAND |
                            SYSCTRL_XOSC32K_EN32K |
                            SYSCTRL_XOSC32K_XTALEN |
                            SYSCTRL_XOSC32K_STARTUP(6) |
                            SYSCTRL_XOSC32K_ENABLE;

    /* Setup clock GCLK2 with OSC32K divided by 32 */
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(2)|GCLK_GENDIV_DIV(4);
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL );
    GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | (RTC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
    while (GCLK->STATUS.bit.SYNCBUSY) {}
}
#else
static void _rtc_clock_setup(void)
{
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_OSC32KCTRL;

#if EXTERNAL_OSC32_SOURCE

    /* RTC uses External 32,768KHz Oscillator */
    OSC32KCTRL->XOSC32K.reg = OSC32KCTRL_XOSC32K_XTALEN
                            | OSC32KCTRL_XOSC32K_EN1K
                            | OSC32KCTRL_XOSC32K_RUNSTDBY
                            | OSC32KCTRL_XOSC32K_ENABLE;

    /* Wait XOSC32K Ready */
    while (OSC32KCTRL->STATUS.bit.XOSC32KRDY==0) {}

    /* RTC source clock is external oscillator at 1kHz */
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K;

#endif /* EXTERNAL_OSC32_SOURCE */

#if INTERNAL_OSC32_SOURCE
    uint32_t * pCalibrationArea;
    uint32_t osc32kcal;

    /* Read OSC32KCAL, calibration data for OSC32 !!! */
    pCalibrationArea = (uint32_t*) NVMCTRL_OTP5;
    osc32kcal = ( (*pCalibrationArea) & 0x1FC0 ) >> 6;

    /* RTC use Low Power Internal Oscillator at 1kHz */
    OSC32KCTRL->OSC32K.reg = OSC32KCTRL_OSC32K_RUNSTDBY
                           | OSC32KCTRL_OSC32K_EN1K
                           | OSC32KCTRL_OSC32K_CALIB(osc32kcal)
                           | OSC32KCTRL_OSC32K_ENABLE;

    /* Wait OSC32K Ready */
    while (OSC32KCTRL->STATUS.bit.OSC32KRDY==0) {}

    /* RTC uses internal 32,768KHz Oscillator */
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_OSC1K;


#endif /* INTERNAL_OSC32_SOURCE */

#if ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE

    /* RTC uses Ultra Low Power internal 32,768KHz Oscillator */
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K;

#endif /* ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE */
}
#endif /* CPU_SAMD21 - Clock Setup */

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

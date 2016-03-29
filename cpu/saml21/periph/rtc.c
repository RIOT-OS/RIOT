/*
 * Copyright (C) 2014 Baptiste CLENET
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_saml21
 * @{
 * @file
 * @brief       Low-level RTC driver implementation
 * @author      Baptiste Clenet <baptiste.clenet@xsoen.com>
 * @autor       ported to SAML21 by FWX <FWX@dialine.fr>
 * @}
 */

#include <time.h>
#include "cpu.h"
#include "periph/rtc.h"
#include "periph_conf.h"
#include "sched.h"
#include "thread.h"

/* guard file in case no RTC device was specified */
#if RTC_NUMOF

typedef struct {
    rtc_alarm_cb_t cb;        /**< callback called from RTC interrupt */
    void *arg;                /**< argument passed to the callback */
} rtc_state_t;

static rtc_state_t rtc_callback;

/* At 1Hz, RTC goes till 63 years (2^5, see 17.8.22 in datasheet)
* reference_year is set to 100 (offset) to be in our current time (2000)
* Thanks to this, the user will be able to set time in 2000's*/
static uint16_t reference_year = 100;

void rtc_init(void)
{
    /* Turn on power manager for RTC */
    /* Already done in cpu_init() */
    /* MCLK->APBAMASK.reg |= MCLK_APBAMASK_RTC; */

    /* DISABLE RTC MASTER */
    rtc_poweroff();


#if EXTERNAL_OSC32_SOURCE

    /* RTC uses External 32,768KHz Oscillator */
    OSC32KCTRL->XOSC32K.reg = OSC32KCTRL_XOSC32K_XTALEN
                            | OSC32KCTRL_XOSC32K_EN1K
                            | OSC32KCTRL_XOSC32K_RUNSTDBY
                            | OSC32KCTRL_OSC32K_ENABLE;

    /* Wait XOSC32K Ready */
    while (OSC32KCTRL->STATUS.bit.XOSC32KRDY==0);

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
    while (OSC32KCTRL->STATUS.bit.OSC32KRDY==0);

    /* RTC uses internal 32,768KHz Oscillator */
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_OSC1K;


#endif /* INTERNAL_OSC32_SOURCE */

#if ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE

    /* RTC uses Ultra Low Power internal 32,768KHz Oscillator */
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K;

#endif /* ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE */

    /* Software Reset the RTC */
    RTC->MODE2.CTRLA.bit.SWRST = 1;
    /* Wait end of reset */
    while (RTC->MODE2.CTRLA.bit.SWRST);

    /* RTC config with RTC_MODE2_CTRL_CLKREP = 0 (24h) */
    RTC->MODE2.CTRLA.reg = RTC_MODE2_CTRLA_PRESCALER_DIV1024 |   /* CLK_RTC_CNT = 1KHz / 1024 -> 1Hz */
                           RTC_MODE2_CTRLA_MODE_CLOCK |          /* Mode 2: Clock/Calendar */
                           RTC_MODE2_CTRLA_SYNCDIS;              /* Clock Read Synchronization Enable */

    /* Clear interrupt flags */
    RTC->MODE2.INTFLAG.bit.OVF = 1;
    RTC->MODE2.INTFLAG.bit.ALARM0 = 1;

    rtc_poweron();
}

int rtc_set_time(struct tm *time)
{
    if ((time->tm_year < reference_year) || (time->tm_year > reference_year + 63)) {
        return -1;
    }
    else {
        while (RTC->MODE2.SYNCBUSY.bit.CLOCK);
        RTC->MODE2.CLOCK.reg = RTC_MODE2_CLOCK_YEAR(time->tm_year - reference_year)
                             | RTC_MODE2_CLOCK_MONTH(time->tm_mon + 1)
                             | RTC_MODE2_CLOCK_DAY(time->tm_mday)
                             | RTC_MODE2_CLOCK_HOUR(time->tm_hour)
                             | RTC_MODE2_CLOCK_MINUTE(time->tm_min)
                             | RTC_MODE2_CLOCK_SECOND(time->tm_sec);
        while (RTC->MODE2.SYNCBUSY.bit.CLOCK);
    }
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
        while (RTC->MODE2.SYNCBUSY.bit.ALARM0);
    }

    /* Setup interrupt */
    NVIC_EnableIRQ(RTC_IRQn);

    /* Enable IRQ */
    rtc_callback.cb = cb;
    rtc_callback.arg = arg;
    RTC->MODE2.INTFLAG.bit.ALARM0 = 1;
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
    RTC->MODE2.CTRLA.bit.ENABLE = 1;
    while (RTC->MODE2.SYNCBUSY.bit.ENABLE);
}

void rtc_poweroff(void)
{
    RTC->MODE2.CTRLA.bit.ENABLE = 0;
    while (RTC->MODE2.SYNCBUSY.bit.ENABLE);
}

void isr_rtc(void)
{
    if (RTC->MODE2.INTFLAG.bit.ALARM0) {
        rtc_callback.cb(rtc_callback.arg);
        /* clear flag */
        RTC->MODE2.INTFLAG.bit.ALARM0 = 1;
    }
    if (RTC->MODE2.INTFLAG.bit.OVF) {
        /* clear flag */
        RTC->MODE2.INTFLAG.bit.OVF = 1;
        /* At 1Hz, RTC goes till 63 years (2^5, see 17.8.22 in datasheet)
        * Start RTC again with reference_year 64 years more (Be careful with alarm set) */
        reference_year += 64;
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#endif /* RTC_NUMOF */

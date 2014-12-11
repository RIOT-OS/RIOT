/*
 * Copyright (C) 2014 Baptiste CLENET
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samd21
 * @{
 * @file
 * @brief       Low-level RTC driver implementation
 * @author      Baptiste Clenet <baptiste.clenet@xsoen.com>
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

static uint16_t reference_year = 100;

void rtc_init(void)
{
    RtcMode2 *rtcMode2 = &(RTC_DEV);

    /* Turn on power manager for RTC */
    PM->APBAMASK.reg |= PM_APBAMASK_RTC;

    SYSCTRL->OSC32K.bit.ENABLE = 0;
    SYSCTRL->OSC32K.bit.ONDEMAND = 1;
    SYSCTRL->OSC32K.bit.RUNSTDBY = 0;
    SYSCTRL->OSC32K.bit.EN1K = 1;
    SYSCTRL->OSC32K.bit.EN32K = 1;
    SYSCTRL->OSC32K.bit.ENABLE = 1;

    /* Setup clock GCLK2 */
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(2)|GCLK_GENDIV_DIV(4);
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL );
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
    GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | (RTC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
    while (GCLK->STATUS.bit.SYNCBUSY);

    /* DISABLE RTC MASTER */
    while(rtcMode2->STATUS.reg & RTC_STATUS_SYNCBUSY);
    rtc_poweroff();

    /* Reset RTC */
    while(rtcMode2->STATUS.bit.SYNCBUSY);
    rtcMode2->CTRL.reg= RTC_MODE2_CTRL_SWRST;
    while(rtcMode2->STATUS.bit.SYNCBUSY);

    /* RTC config with RTC_MODE2_CTRL_CLKREP = 0 (24h) */
    rtcMode2->CTRL.reg = RTC_MODE2_CTRL_PRESCALER_DIV1024|RTC_MODE2_CTRL_MODE_CLOCK;
    while(rtcMode2->STATUS.bit.SYNCBUSY);
    rtcMode2->INTENSET.reg = RTC_MODE2_INTENSET_OVF;
    while(rtcMode2->STATUS.bit.SYNCBUSY);
    rtc_poweron();
}

int rtc_set_time(struct tm *time)
{
    RtcMode2 *rtcMode2 = &(RTC_DEV);
    if((time->tm_year < reference_year) || (time->tm_year > reference_year + 63)){
        return -1;
    }
    else{
        rtcMode2->CLOCK.reg = RTC_MODE2_CLOCK_YEAR(time->tm_year - reference_year)
                | RTC_MODE2_CLOCK_MONTH(time->tm_mon + 1)
                | RTC_MODE2_CLOCK_DAY(time->tm_mday)
                | RTC_MODE2_CLOCK_HOUR(time->tm_hour)
                | RTC_MODE2_CLOCK_MINUTE(time->tm_min)
                | RTC_MODE2_CLOCK_SECOND(time->tm_sec);
    }
    while(rtcMode2->STATUS.bit.SYNCBUSY);
    return 0;
}

int rtc_get_time(struct tm *time)
{
    RtcMode2 *rtcMode2 = &(RTC_DEV);
    time->tm_year = rtcMode2->CLOCK.bit.YEAR + reference_year;
    if((time->tm_year < reference_year) || (time->tm_year > (reference_year + 63))){
            return -1;
        }
    time->tm_mon = rtcMode2->CLOCK.bit.MONTH - 1;
    time->tm_mday = rtcMode2->CLOCK.bit.DAY;
    time->tm_hour = rtcMode2->CLOCK.bit.HOUR;
    time->tm_min = rtcMode2->CLOCK.bit.MINUTE;
    time->tm_sec = rtcMode2->CLOCK.bit.SECOND;
    while(rtcMode2->STATUS.bit.SYNCBUSY);
    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    RtcMode2 *rtcMode2 = &(RTC_DEV);
    rtc_clear_alarm();
    if((time->tm_year < reference_year) || (time->tm_year > (reference_year + 63))){
        return -2;
    }
    else{
        rtcMode2->Mode2Alarm[0].ALARM.reg = RTC_MODE2_ALARM_YEAR(time->tm_year - reference_year)
                | RTC_MODE2_ALARM_MONTH(time->tm_mon + 1)
                | RTC_MODE2_ALARM_DAY(time->tm_mday)
                | RTC_MODE2_ALARM_HOUR(time->tm_hour)
                | RTC_MODE2_ALARM_MINUTE(time->tm_min)
                | RTC_MODE2_ALARM_SECOND(time->tm_sec);
        rtcMode2->Mode2Alarm[0].MASK.reg = RTC_MODE2_MASK_SEL(6);
    }
    while(rtcMode2->STATUS.bit.SYNCBUSY);

    /* Setup interrupt */
    NVIC_SetPriority(RTC_IRQn, 10);
    NVIC_EnableIRQ(RTC_IRQn);

    /* Enable IRQ */
    rtc_callback.cb = cb;
    rtc_callback.arg = arg;
    rtcMode2->INTFLAG.reg = RTC_MODE2_INTFLAG_ALARM0;
    while(rtcMode2->STATUS.bit.SYNCBUSY);
    rtcMode2->INTENSET.reg = RTC_MODE2_INTENSET_ALARM0;
    while(rtcMode2->STATUS.bit.SYNCBUSY);

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    RtcMode2 *rtcMode2 = &(RTC_DEV);
    time->tm_year = rtcMode2->Mode2Alarm[0].ALARM.bit.YEAR + reference_year;
    if((time->tm_year < reference_year) || (time->tm_year > (reference_year + 63))){
            return -1;
        }
    time->tm_mon = rtcMode2->CLOCK.bit.MONTH - 1;
    time->tm_mday = rtcMode2->CLOCK.bit.DAY;
    time->tm_hour = rtcMode2->CLOCK.bit.HOUR;
    time->tm_min = rtcMode2->CLOCK.bit.MINUTE;
    time->tm_sec = rtcMode2->CLOCK.bit.SECOND;
    while(rtcMode2->STATUS.bit.SYNCBUSY);
    return 0;
}

void rtc_clear_alarm(void)
{
    RtcMode2 *rtcMode2 = &(RTC_DEV);
    /* Disable IRQ */
    rtcMode2->INTENCLR.reg = RTC_MODE2_INTENCLR_ALARM0;
    rtc_callback.cb = NULL;
    rtc_callback.arg = NULL;
}

void rtc_poweron(void)
{
    RtcMode2 *rtcMode2 = &(RTC_DEV);
    rtcMode2->CTRL.bit.ENABLE = 1;
    while(rtcMode2->STATUS.bit.SYNCBUSY);
}

void rtc_poweroff(void)
{
    RtcMode2 *rtcMode2 = &(RTC_DEV);
    rtcMode2->CTRL.bit.ENABLE = 0;
    while(rtcMode2->STATUS.bit.SYNCBUSY);
}

void isr_rtc(void)
{
    RtcMode2 *rtcMode2 = &(RTC_DEV);
    uint16_t status = rtcMode2->INTFLAG.reg;
    if(status & RTC_MODE2_INTFLAG_ALARM0){
        rtc_callback.cb(rtc_callback.arg);
        rtcMode2->INTFLAG.reg = RTC_MODE2_INTFLAG_ALARM0;
    }
    if(status & RTC_MODE2_INTFLAG_OVF){
        /* At 1Hz, RTC goes till 63 years (2^5, see 17.8.22 in datasheet)
        * Start RTC again with reference_year 64 years more (Be careful with alarm set) */
        reference_year += 64;
        rtcMode2->INTFLAG.reg = RTC_MODE2_INTFLAG_OVF;
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#endif /* RTC_NUMOF */

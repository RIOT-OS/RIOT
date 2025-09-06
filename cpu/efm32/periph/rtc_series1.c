/*
 * SPDX-FileCopyrightText: 2016-2017 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_rtc
 * @{
 *
 * @file
 * @brief       RTC peripheral driver implementation for EFM32 Series 1 MCUs
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <time.h>

#include "cpu.h"

#include "periph_conf.h"
#include "periph/rtc.h"

#include "em_cmu.h"
#include "em_rtcc.h"
#include "em_rtcc_utils.h"

#define RTC_YEAR_OFFSET (100)       /**< RTCC has only two-digit notation */

typedef struct {
    rtc_alarm_cb_t alarm_cb;        /**< callback called from RTC interrupt */
    void *alarm_arg;                /**< argument passed to the callback */
    uint32_t alarm_year;            /**< alarm year */
} rtc_state_t;

static rtc_state_t rtc_state;

void rtc_init(void)
{
    /* enable clocks */
    CMU_ClockEnable(cmuClock_CORELE, true);
    CMU_ClockEnable(cmuClock_RTCC, true);

    /* reset and initialize peripheral */
    RTCC_Init_TypeDef init = RTCC_INIT_DEFAULT;

    init.enable = false;
    init.presc = rtccCntPresc_32768;
    init.cntMode = rtccCntModeCalendar;

    RTCC_Reset();
    RTCC_Init(&init);

    /* initialize alarm channel */
    RTCC_CCChConf_TypeDef init_channel = RTCC_CH_INIT_COMPARE_DEFAULT;

    RTCC_ChannelInit(0, &init_channel);

    /* enable interrupt */
    NVIC_ClearPendingIRQ(RTCC_IRQn);
    NVIC_EnableIRQ(RTCC_IRQn);

    /* enable peripheral */
    RTCC_Enable(true);
}

int rtc_set_time(struct tm *time)
{
    rtc_tm_normalize(time);

    RTCC_DateSet(
        RTCC_Year2BCD(time->tm_year - RTC_YEAR_OFFSET) |
        RTCC_Month2BCD(time->tm_mon) |
        RTCC_DayOfMonth2BCD(time->tm_mday) |
        RTCC_DayOfWeek2BCD(time->tm_wday));
    RTCC_TimeSet(
        RTCC_Hour2BCD(time->tm_hour) |
        RTCC_Minute2BCD(time->tm_min) |
        RTCC_Second2BCD(time->tm_sec));

    return 0;
}

int rtc_get_time(struct tm *time)
{
    uint32_t datestamp = RTCC_DateGet();
    uint32_t timestamp = RTCC_TimeGet();

    time->tm_year = RTCC_BCD2Year(datestamp) + RTC_YEAR_OFFSET;
    time->tm_mon = RTCC_BCD2Month(datestamp);
    time->tm_mday = RTCC_BCD2DayOfMonth(datestamp);
    time->tm_wday = RTCC_BCD2DayOfWeek(datestamp);
    time->tm_hour = RTCC_BCD2Hour(timestamp);
    time->tm_min = RTCC_BCD2Minute(timestamp);
    time->tm_sec = RTCC_BCD2Second(timestamp);

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    rtc_tm_normalize(time);

    rtc_state.alarm_cb = cb;
    rtc_state.alarm_arg = arg;
    rtc_state.alarm_year = time->tm_year;

    /* disable interrupt so it doesn't accidentally trigger */
    RTCC_IntDisable(RTCC_IEN_CC0);

    /* set compare registers */
    RTCC_ChannelDateSet(0,
                        RTCC_Channel_Month2BCD(time->tm_mon) |
                        RTCC_Channel_Day2BCD(time->tm_mday));
    RTCC_ChannelTimeSet(0,
                        RTCC_Channel_Hour2BCD(time->tm_hour) |
                        RTCC_Channel_Minute2BCD(time->tm_min) |
                        RTCC_Channel_Second2BCD(time->tm_sec));

    /* enable the interrupt */
    RTCC_IntClear(RTCC_IFC_CC0);
    RTCC_IntEnable(RTCC_IEN_CC0);

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    uint32_t datestamp = RTCC_ChannelDateGet(0);
    uint32_t timestamp = RTCC_ChannelTimeGet(0);

    time->tm_year = rtc_state.alarm_year;
    time->tm_mon = RTCC_Channel_BCD2Month(datestamp);
    time->tm_mday = RTCC_Channel_BCD2Day(datestamp);
    time->tm_hour = RTCC_Channel_BCD2Hour(timestamp);
    time->tm_min = RTCC_Channel_BCD2Minute(timestamp);
    time->tm_sec = RTCC_Channel_BCD2Second(timestamp);

    return 0;
}

void rtc_clear_alarm(void)
{
    rtc_state.alarm_cb = NULL;
    rtc_state.alarm_arg = NULL;
    rtc_state.alarm_year = 0;

    RTCC_IntDisable(RTCC_IEN_CC0);
}

void rtc_poweron(void)
{
    CMU_ClockEnable(cmuClock_RTCC, true);
}

void rtc_poweroff(void)
{
    CMU_ClockEnable(cmuClock_RTCC, false);
}

void isr_rtcc(void)
{
    if (RTCC_IntGet() & RTCC_IF_CC0) {
        if (rtc_state.alarm_cb != NULL) {
            /* check if year matches, otherwise alarm would go off each year */
            if (RTCC_BCD2Year(RTCC_DateGet()) + RTC_YEAR_OFFSET == rtc_state.alarm_year) {
                rtc_state.alarm_cb(rtc_state.alarm_arg);
            }
        }

        RTCC_IntClear(RTCC_IFC_CC0);
    }
    cortexm_isr_end();
}

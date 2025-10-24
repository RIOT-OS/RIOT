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
 * @brief       RTC peripheral driver implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <time.h>

#include "cpu.h"

#include "periph_conf.h"
#include "periph/rtc.h"

#include "em_cmu.h"
#include "em_rtc.h"

#define RTC_MAX_VALUE       (0xFFFFFF)
#define RTC_SHIFT_VALUE     (24U)

typedef struct {
    rtc_alarm_cb_t alarm_cb;        /**< callback called from RTC interrupt */
    void *alarm_arg;                /**< argument passed to the callback */
    uint32_t alarm;                 /**< scheduled alarm (may be deferred) */
    uint8_t overflows;              /**< number of overflows */
} rtc_state_t;

static rtc_state_t rtc_state;

/**
 * @brief Actual implementation of rtc_set_alarm
 */
static void _set_alarm(void)
{
    uint32_t overflows = (rtc_state.alarm >> RTC_SHIFT_VALUE);

    /* check if alarm is in reach of RTC counter, which basically means that
       the first 8 bits created by software now match */
    if (overflows == rtc_state.overflows) {
        /* disable interrupt so it doesn't accidentally trigger */
        RTC_IntDisable(RTC_IEN_COMP0);

        /* set compare register */
        RTC_CompareSet(0, rtc_state.alarm & RTC_MAX_VALUE);

        /* (re-)enable the interrupt */
        RTC_IntClear(RTC_IEN_COMP0);
        RTC_IntEnable(RTC_IEN_COMP0);
    }
}

void rtc_init(void)
{
    /* prescaler of 32768 = 1 s of resolution and overflow each 194 days */
    CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_32768);

    /* enable clocks */
    CMU_ClockEnable(cmuClock_CORELE, true);
    CMU_ClockEnable(cmuClock_RTC, true);

    /* initialize the state */
    rtc_state.overflows = 0;

    /* reset and initialize the peripheral */
    RTC_Init_TypeDef init = RTC_INIT_DEFAULT;

    init.enable = false;
    init.comp0Top = false;

    RTC_Reset();
    RTC_Init(&init);

    /* enable interrupts */
    RTC_IntEnable(RTC_IEN_OF);

    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_EnableIRQ(RTC_IRQn);

    /* enable peripheral */
    RTC_Enable(true);
}

__attribute__((weak))
void rtc_pre_set_time(struct tm *old_time, const struct tm *new_time)
{
    (void)old_time;
    (void)new_time;
}

__attribute__((weak))
void rtc_post_set_time(struct tm *old_time, const struct tm *new_time)
{
    (void)old_time;
    (void)new_time;
}

int rtc_set_time(struct tm *time)
{
    struct tm old_time;
    rtc_get_time(&old_time);
    rtc_pre_set_time(&old_time, time);

    time_t timestamp = rtc_mktime(time);

    rtc_state.overflows = (timestamp >> RTC_SHIFT_VALUE);
    RTC->CNT = timestamp & RTC_MAX_VALUE;

    rtc_post_set_time(&old_time, time);
    return 0;
}

int rtc_get_time(struct tm *time)
{
    time_t timestamp = RTC_CounterGet();

    timestamp = timestamp + (rtc_state.overflows << RTC_SHIFT_VALUE);

    rtc_localtime(timestamp, time);

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    rtc_state.alarm_cb = cb;
    rtc_state.alarm_arg = arg;
    rtc_state.alarm = rtc_mktime(time);

    /* alarm may not be in reach of current time, so defer if needed */
    _set_alarm();

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    rtc_localtime(rtc_state.alarm, time);

    return 0;
}

void rtc_clear_alarm(void)
{
    rtc_state.alarm_cb = NULL;
    rtc_state.alarm_arg = NULL;
    rtc_state.alarm = 0;

    RTC_IntDisable(RTC_IEN_COMP0);
}

void rtc_poweron(void)
{
    CMU_ClockEnable(cmuClock_RTC, true);
}

void rtc_poweroff(void)
{
    CMU_ClockEnable(cmuClock_RTC, false);
}

void isr_rtc(void)
{
    if ((RTC_IntGet() & RTC_IF_COMP0)) {
        if (rtc_state.alarm_cb != NULL) {
            rtc_state.alarm_cb(rtc_state.alarm_arg);
        }

        /* clear interrupt */
        RTC_IntClear(RTC_IFC_COMP0);
    }
    if (RTC_IntGet() & RTC_IF_OF) {
        rtc_state.overflows++;

        /* check if alarm should be enabled now */
        if (rtc_state.alarm_cb) {
            _set_alarm();
        }

        /* clear interrupt */
        RTC_IntClear(RTC_IFC_OF);
    }
    cortexm_isr_end();
}

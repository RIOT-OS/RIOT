/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_xmc1000
 * @{
 *
 * @file
 * @brief       Low-level RTC driver implementation
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include <time.h>

#include "periph/gating.h"
#include "periph/rtc.h"
#include "periph_conf.h"
#include "sched.h"
#include "thread.h"

/* guard file in case no RTC device was specified */
#if RTC_NUMOF

/* Wait for mirror transfers for this register to finish */
#define RTC_WAIT(Reg)                                   \
    do {} while (SCU_GENERAL->MIRRSTS &                 \
                 SCU_GENERAL_MIRRSTS_RTC_ ## Reg ## _Msk)

/* Storage for callback function and argument */
static rtc_alarm_cb_t callback;
static void *callback_arg;

static inline void _rtc_disable(void)
{
    RTC_WAIT(CTR);
    RTC->CTR &= ~RTC_CTR_ENB_Msk;
}

static inline void _rtc_enable(void)
{
    RTC_WAIT(CTR);
    RTC->CTR = (RTC_DIVIDER << RTC_CTR_DIV_Pos) | (1 << RTC_CTR_ENB_Pos);
}

static inline int _rtc_set(struct tm *time, bool alarm)
{
    time->tm_mday--;

    uint32_t v0 = (((time->tm_sec << RTC_TIM0_SE_Pos) & RTC_TIM0_SE_Msk) |
                   ((time->tm_min << RTC_TIM0_MI_Pos) & RTC_TIM0_MI_Msk) |
                   ((time->tm_hour << RTC_TIM0_HO_Pos) & RTC_TIM0_HO_Msk) |
                   ((time->tm_mday << RTC_TIM0_DA_Pos) & RTC_TIM0_DA_Msk));

    uint32_t v1 = (((time->tm_mon << RTC_TIM1_MO_Pos) & RTC_TIM1_MO_Msk) |
                   ((time->tm_year << RTC_TIM1_YE_Pos) & RTC_TIM1_YE_Msk));

    _rtc_disable();

    if (alarm) {
        RTC->ATIM0 = v0;
        RTC->ATIM1 = v1;
    }
    else {
        RTC->TIM0 = v0;
        RTC->TIM1 = v1 | ((time->tm_wday << RTC_TIM1_DAWE_Pos) & RTC_TIM1_DAWE_Msk);
    }

    _rtc_enable();

    return 0;
}

static inline int _rtc_get(struct tm *time, bool alarm)
{
    uint32_t tim0;
    uint32_t tim1;

    if (alarm) {
        RTC_WAIT(ATIM0);
        tim0 = RTC->ATIM0;
        RTC_WAIT(ATIM1);
        tim1 = RTC->ATIM1;
    }
    else {
        RTC_WAIT(TIM0);
        tim0 = RTC->TIM0;
        RTC_WAIT(TIM1);
        tim1 = RTC->TIM1;
    }

    time->tm_sec  = (tim0 & RTC_TIM0_SE_Msk) >> RTC_TIM0_SE_Pos;
    time->tm_min  = (tim0 & RTC_TIM0_MI_Msk) >> RTC_TIM0_MI_Pos;
    time->tm_hour = (tim0 & RTC_TIM0_HO_Msk) >> RTC_TIM0_HO_Pos;
    time->tm_mday = (tim0 & RTC_TIM0_DA_Msk) >> RTC_TIM0_DA_Pos;

    time->tm_mon  = (tim1 & RTC_TIM1_MO_Msk) >> RTC_TIM1_MO_Pos;
    time->tm_year = (tim1 & RTC_TIM1_YE_Msk) >> RTC_TIM1_YE_Pos;
    time->tm_wday = (tim1 & RTC_TIM1_DAWE_Msk) >> RTC_TIM1_DAWE_Pos;

    time->tm_mday++;

    return 0;
}

void rtc_init(void)
{
    rtc_poweron();

    /* do this once and leave it */
    NVIC_SetPriority(SCU_1_IRQn, RTC_IRQ_PRIO);
    NVIC_EnableIRQ(SCU_1_IRQn);

    _rtc_enable();
}

int rtc_set_time(struct tm *time)
{
    if (!time || _rtc_set(time, false)) {
        return -1;
    }
    return 0;
}

int rtc_get_time(struct tm *time)
{
    if (!time || _rtc_get(time, false)) {
        return -1;
    }
    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    if (!cb || !time || _rtc_set(time, true)) {
        return -1;
    }

    callback = cb;
    callback_arg = arg;

    /* enable interrupt */
    RTC->MSKSR = RTC_MSKSR_MAI_Msk;

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    if (RTC->MSKSR & RTC_MSKSR_MAI_Msk) {
        return _rtc_get(time, true);
    }
    else {
        return -1;
    }
}

void rtc_clear_alarm(void)
{
    RTC->MSKSR &= ~(RTC_MSKSR_MAI_Msk);

    callback = NULL;
    callback_arg = NULL;
}

void rtc_poweron(void)
{
    GATING_CLEAR(RTC);
}

void rtc_poweroff(void)
{
    GATING_SET(RTC);
}

void isr_scu1(void)
{
    callback(callback_arg);

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /* RTC_NUMOF */

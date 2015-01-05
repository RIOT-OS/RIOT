/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis_common_rtc
 *
 * @{
 *
 * @file
 * @brief       Low-level RTC driver implementation for Freescale Kinetis MCUs.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <time.h>
#include "cpu.h"
#include "periph/rtc.h"
#include "periph_conf.h"
#include "sched.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if RTC_NUMOF


#ifndef RTC_LOAD_CAP_BITS
#define RTC_LOAD_CAP_BITS    0
#endif

typedef struct {
    rtc_alarm_cb_t cb;        /**< callback called from RTC interrupt */
    void *arg;                /**< argument passed to the callback */
} rtc_state_t;

static rtc_state_t rtc_callback;

void rtc_init(void)
{
    RTC_Type *rtc = RTC_DEV;

    RTC_UNLOCK();
    /* Reset RTC */
    rtc->CR = RTC_CR_SWR_MASK;
    rtc->CR = 0;

    if (rtc->SR & RTC_SR_TIF_MASK) {
        /* Clear TIF by writing TSR. */
        rtc->TSR = 0;
    }

    /* Enable RTC oscillator and non-supervisor mode accesses. */
    /* Enable load capacitance as configured by periph_conf.h */
    rtc->CR = RTC_CR_OSCE_MASK | RTC_CR_SUP_MASK | RTC_LOAD_CAP_BITS;

    /* Clear TAF by writing TAR. */
    rtc->TAR = 0xffffff42;

    /* Disable all RTC interrupts. */
    rtc->IER = 0;

    rtc_poweron();
}

int rtc_set_time(struct tm *time)
{
    RTC_Type *rtc = RTC_DEV;
    time_t t = mktime(time);

    /* Disable Time Counter */
    rtc->SR &= ~RTC_SR_TCE_MASK;
    rtc->TSR = t;
    rtc->SR |= RTC_SR_TCE_MASK;
    return 0;
}

int rtc_get_time(struct tm *time)
{
    RTC_Type *rtc = RTC_DEV;
    time_t t;

    for (int i = 0; i < 3; i++) {
        t = rtc->TSR;

        if (t == (time_t)rtc->TSR) {
            gmtime_r(&t, time);
            return 0;
        }
    }

    return -1;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    RTC_Type *rtc = RTC_DEV;
    time_t t = mktime(time);

    /* Disable Timer Alarm Interrupt */
    rtc->IER &= ~RTC_IER_TAIE_MASK;

    rtc->TAR = t;

    rtc_callback.cb = cb;
    rtc_callback.arg = arg;

    /* Enable Timer Alarm Interrupt */
    rtc->IER |= RTC_IER_TAIE_MASK;

    /* Enable RTC interrupts */
    NVIC_SetPriority(RTC_IRQn, 10);
    NVIC_EnableIRQ(RTC_IRQn);

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    RTC_Type *rtc = RTC_DEV;
    time_t t = rtc->TAR;

    gmtime_r(&t, time);

    return 0;
}

void rtc_clear_alarm(void)
{
    RTC_Type *rtc = RTC_DEV;

    NVIC_DisableIRQ(RTC_IRQn);
    /* Disable Timer Alarm Interrupt */
    rtc->IER &= ~RTC_IER_TAIE_MASK;
    rtc->TAR = 0;
    rtc_callback.cb = NULL;
    rtc_callback.arg = NULL;
}

/* RTC module has independent power suply. We can not really turn it on/off. */

void rtc_poweron(void)
{
    RTC_Type *rtc = RTC_DEV;
    /* Enable Time Counter */
    rtc->SR |= RTC_SR_TCE_MASK;
}

void rtc_poweroff(void)
{
    RTC_Type *rtc = RTC_DEV;
    /* Disable Time Counter */
    rtc->SR &= ~RTC_SR_TCE_MASK;
}

void isr_rtc(void)
{
    RTC_Type *rtc = RTC_DEV;

    if (rtc->SR & RTC_SR_TAF_MASK) {
        rtc_callback.cb(rtc_callback.arg);
        rtc->TAR = 0;
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

#endif /* RTC_NUMOF */

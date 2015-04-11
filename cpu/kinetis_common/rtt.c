/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis_common_rtt
 *
 * @{
 *
 * @file
 * @brief       Low-level RTT interface implementation for Freescale Kinetis
 *              MCUs. Freescale's RTC module is what RIOT calls a Real-Time
 *              Timer (RTT), a simple counter which counts seconds; RIOT Real-
 *              Time Clocks (RTC) counts seconds, minutes, hours etc. We provide
 *              an RTT->RTC wrapper layer in a separate file to allow using the
 *              RTT as a system real time clock.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include <time.h>
#include "cpu.h"
#include "periph/rtt.h"
#include "periph_conf.h"
#include "sched.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if RTT_NUMOF


#ifndef RTC_LOAD_CAP_BITS
#define RTC_LOAD_CAP_BITS    0
#endif

typedef struct {
    rtt_cb_t alarm_cb;              /**< callback called from RTC alarm */
    void *alarm_arg;                /**< argument passed to the callback */
    rtt_cb_t overflow_cb;           /**< callback called when RTC overflows */
    void *overflow_arg;             /**< argument passed to the callback */
} rtt_state_t;

static rtt_state_t rtt_callback;

void rtt_init(void)
{
    RTC_Type *rtt = RTT_DEV;

    RTT_UNLOCK();
    /* Reset RTC */
    rtt->CR = RTC_CR_SWR_MASK;
    rtt->CR = 0;

    if (rtt->SR & RTC_SR_TIF_MASK) {
        /* Clear TIF by writing TSR. */
        rtt->TSR = 0;
    }

    /* Enable RTC oscillator and non-supervisor mode accesses. */
    /* Enable load capacitance as configured by periph_conf.h */
    rtt->CR = RTC_CR_OSCE_MASK | RTC_CR_SUP_MASK | RTC_LOAD_CAP_BITS;

    /* Clear TAF by writing TAR. */
    rtt->TAR = 0xffffff42;

    /* Disable all RTC interrupts. */
    rtt->IER = 0;

    rtt_poweron();
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    RTC_Type *rtt = RTT_DEV;
    rtt_callback.overflow_cb = cb;
    rtt_callback.overflow_arg = arg;
    rtt->IER |= RTC_IER_TOIE_MASK;
}

void rtt_clear_overflow_cb(void)
{
    RTC_Type *rtt = RTT_DEV;
    rtt_callback.overflow_cb = NULL;
    rtt_callback.overflow_arg = NULL;
    rtt->IER &= ~(RTC_IER_TOIE_MASK);
}

uint32_t rtt_get_counter(void)
{
    RTC_Type *rtt = RTT_DEV;
    uint32_t t;
    for (int i = 0; i < 3; i++) {
        /* Read twice to make sure we get a stable reading */
        t = rtt->TSR;

        if (t == rtt->TSR) {
            return t;
        }
    }
    /* Fallback if we are not getting stable readings */
    return t;
}

void rtt_set_counter(uint32_t counter)
{
    RTC_Type *rtt = RTT_DEV;

    /* Disable time counter before writing to the timestamp register */
    rtt->SR &= ~RTC_SR_TCE_MASK;
    rtt->TSR = counter;
    /* Enable when done */
    rtt->SR |= RTC_SR_TCE_MASK;
}


void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    RTC_Type *rtt = RTT_DEV;

    /* Disable Timer Alarm Interrupt */
    rtt->IER &= ~(RTC_IER_TAIE_MASK);

    rtt->TAR = alarm;

    rtt_callback.alarm_cb = cb;
    rtt_callback.alarm_arg = arg;

    /* Enable Timer Alarm Interrupt */
    rtt->IER |= RTC_IER_TAIE_MASK;

    /* Enable RTC interrupts */
    NVIC_SetPriority(RTT_IRQ, RTT_IRQ_PRIO);
    NVIC_EnableIRQ(RTT_IRQ);
}

uint32_t rtt_get_alarm(void)
{
    RTC_Type *rtt = RTT_DEV;
    return rtt->TAR;
}

void rtt_clear_alarm(void)
{
    RTC_Type *rtt = RTT_DEV;

    /* Disable Timer Alarm Interrupt */
    rtt->IER &= ~RTC_IER_TAIE_MASK;
    rtt->TAR = 0;
    rtt_callback.alarm_cb = NULL;
    rtt_callback.alarm_arg = NULL;
}

/* RTC module has independent power suply. We can not really turn it on/off. */

void rtt_poweron(void)
{
    RTC_Type *rtt = RTT_DEV;
    /* Enable Time Counter */
    rtt->SR |= RTC_SR_TCE_MASK;
}

void rtt_poweroff(void)
{
    RTC_Type *rtt = RTT_DEV;
    /* Disable Time Counter */
    rtt->SR &= ~RTC_SR_TCE_MASK;
}

void RTT_ISR(void)
{
    RTC_Type *rtt = RTT_DEV;

    if (rtt->SR & RTC_SR_TAF_MASK) {
        if (rtt_callback.alarm_cb != NULL) {
            /* Disable Timer Alarm Interrupt */
            rtt->IER &= ~RTC_IER_TAIE_MASK;
            rtt_callback.alarm_cb(rtt_callback.alarm_arg);
        }
    }

    if (rtt->SR & RTC_SR_TOF_MASK) {
        if (rtt_callback.overflow_cb != NULL) {
            rtt_callback.overflow_cb(rtt_callback.overflow_arg);
        }
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

#endif /* RTC_NUMOF */

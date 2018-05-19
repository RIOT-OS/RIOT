/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 * Copyright (C) 2018 Ishraq Ibne Ashraf
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_rtt
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
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

#include <time.h>
#include "cpu.h"
#include "periph/rtt.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

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

#if !defined(_MKE02Z4_H_)
    /* Reset RTC */
    rtt->CR = RTC_CR_SWR_MASK;
    /* cppcheck-suppress redundantAssignment
     * reset routine */
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
#else /* !defined(_MKE02Z4_H_) */
    /* LPOCLK (1kHz) as clock source */
    rtt->SC |= RTC_SC_RTCLKS(0x01);
    /* Prescaler (one tick every 100ms) */
    rtt->SC |= RTC_SC_RTCPS(0x06);
    /* Disable all RTC interrupts */
    rtt->SC &= ~(RTC_SC_RTIE_MASK);
    /* Disable RTC output */
    rtt->SC &= ~(RTC_SC_RTCO_MASK);
    /* Set modulo value to 0 */
    rtt->MOD |= RTC_MOD_MOD(0);
#endif /* !defined(_MKE02Z4_H_) */
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    RTC_Type *rtt = RTT_DEV;
    rtt_callback.overflow_cb = cb;
    rtt_callback.overflow_arg = arg;

#if !defined(_MKE02Z4_H_)
    rtt->IER |= RTC_IER_TOIE_MASK;
#else /* !defined(_MKE02Z4_H_) */
    rtt_callback.alarm_cb = cb;
    rtt_callback.alarm_arg = arg;
    rtt->SC |= RTC_SC_RTIE_MASK;
#endif /* !defined(_MKE02Z4_H_) */
}

void rtt_clear_overflow_cb(void)
{
    RTC_Type *rtt = RTT_DEV;
    rtt_callback.overflow_cb = NULL;
    rtt_callback.overflow_arg = NULL;

#if !defined(_MKE02Z4_H_)
    rtt->IER &= ~(RTC_IER_TOIE_MASK);
#else /* !defined(_MKE02Z4_H_) */
    rtt_callback.alarm_cb = NULL;
    rtt_callback.alarm_arg = NULL;
    rtt->SC &= ~(RTC_SC_RTIE_MASK);
#endif /* !defined(_MKE02Z4_H_) */
}

uint32_t rtt_get_counter(void)
{
    RTC_Type *rtt = RTT_DEV;

#if !defined(_MKE02Z4_H_)
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
#else /* !defined(_MKE02Z4_H_) */
    return rtt->CNT;
#endif /* !defined(_MKE02Z4_H_) */
}

void rtt_set_counter(uint32_t counter)
{
    RTC_Type *rtt = RTT_DEV;

#if !defined(_MKE02Z4_H_)
    /* Disable time counter before writing to the timestamp register */
    rtt->SR &= ~RTC_SR_TCE_MASK;
    rtt->TSR = counter;
    /* Enable when done */
    rtt->SR |= RTC_SR_TCE_MASK;
#else /* !defined(_MKE02Z4_H_) */
    /* Disable interrupt */
    rtt->SC &= ~(RTC_SC_RTIE_MASK);
    rtt->MOD = counter;
    /* Enable interrupt */
    rtt->SC |= RTC_SC_RTIE_MASK;
#endif /* !defined(_MKE02Z4_H_) */
}


void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    /* The alarm is triggered when TSR matches TAR, and TSR increments. This
     * seem counterintuitive as most users expect the alarm to trigger
     * immediately when the counter becomes equal to the alarm time. */
    RTC_Type *rtt = RTT_DEV;

#if !defined(_MKE02Z4_H_)
    /* Disable Timer Alarm Interrupt */
    rtt->IER &= ~(RTC_IER_TAIE_MASK);
    rtt->TAR = alarm - 1;

    /* Enable Timer Alarm Interrupt */
    rtt->IER |= RTC_IER_TAIE_MASK;
#else /* !defined(_MKE02Z4_H_) */
    /* Disable interrupt */
    rtt->SC &= ~(RTC_SC_RTIE_MASK);
    rtt->MOD = alarm - 1;

    /* Reset the counter */
    rtt->SC |= RTC_SC_RTCPS(0x00);
    rtt->SC |= RTC_SC_RTCPS(0x06);

    /* Enable interrupt */
    rtt->SC |= RTC_SC_RTIE_MASK;

    rtt_callback.overflow_cb = cb;
    rtt_callback.overflow_arg = arg;
#endif /* !defined(_MKE02Z4_H_) */

    rtt_callback.alarm_cb = cb;
    rtt_callback.alarm_arg = arg;

    /* Enable RTC interrupts */
    NVIC_SetPriority(RTT_IRQ, RTT_IRQ_PRIO);
    NVIC_EnableIRQ(RTT_IRQ);
}

uint32_t rtt_get_alarm(void)
{
    RTC_Type *rtt = RTT_DEV;

#if !defined(_MKE02Z4_H_)
    return rtt->TAR + 1;
#else /* !defined(_MKE02Z4_H_) */
    return rtt->MOD + 1;
#endif /* !defined(_MKE02Z4_H_) */
}

void rtt_clear_alarm(void)
{
    RTC_Type *rtt = RTT_DEV;

#if !defined(_MKE02Z4_H_)
    /* Disable Timer Alarm Interrupt */
    rtt->IER &= ~RTC_IER_TAIE_MASK;
    rtt->TAR = 0;
#else /* !defined(_MKE02Z4_H_) */
    /* Disable interrupt */
    rtt->SC &= ~(RTC_SC_RTIE_MASK);
    rtt_callback.overflow_arg = NULL;
    rtt_callback.overflow_arg = NULL;
#endif /* !defined(_MKE02Z4_H_) */

    rtt_callback.alarm_cb = NULL;
    rtt_callback.alarm_arg = NULL;
}

/* RTC module has independent power suply. We can not really turn it on/off. */

void rtt_poweron(void)
{
#if !defined(_MKE02Z4_H_)
    RTC_Type *rtt = RTT_DEV;
    /* Enable Time Counter */
    rtt->SR |= RTC_SR_TCE_MASK;
#else /* !defined(_MKE02Z4_H_) */
    RTT_UNLOCK();
#endif /* !defined(_MKE02Z4_H_) */
}

void rtt_poweroff(void)
{
#if !defined(_MKE02Z4_H_)
    RTC_Type *rtt = RTT_DEV;
    /* Disable Time Counter */
    rtt->SR &= ~RTC_SR_TCE_MASK;
#else /* !defined(_MKE02Z4_H_) */
    RTT_LOCK();
#endif /* !defined(_MKE02Z4_H_) */
}

void RTT_ISR(void)
{
    RTC_Type *rtt = RTT_DEV;

#if !defined(_MKE02Z4_H_)
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
#else /* !defined(_MKE02Z4_H_) */
    if ((rtt->SC & RTC_SC_RTIF_MASK)) {
        /* Disable interrupt */
        rtt->SC &= ~(RTC_SC_RTIE_MASK);

        if (rtt_callback.alarm_cb != NULL) {
            rtt_callback.alarm_cb(rtt_callback.alarm_arg);
        }

        if (rtt_callback.overflow_cb != NULL) {
            rtt_callback.overflow_cb(rtt_callback.overflow_arg);
        }

        /* Clear the interrupt flag and request */
        rtt->SC |= RTC_SC_RTIF_MASK;
    }
#endif /* !defined(_MKE02Z4_H_) */

    cortexm_isr_end();
}

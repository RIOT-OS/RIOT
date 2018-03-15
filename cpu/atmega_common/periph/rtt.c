/*
 * Copyright (C) 2016 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/*
 * @ingroup cpu_atmega_common
 * @ingroup drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       Low-level ATmega RTT driver implementation
 *
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <avr/interrupt.h>

#include "board.h"
#include "irq.h"
#include "periph/rtt.h"
#include "thread.h"

#include "debug.h"
#define ENABLE_DEBUG    (0)

/* guard file in case no RTT device is defined */
#if RTT_NUMOF

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    rtt_cb_t alarm_cb;              /**< callback called from RTC alarm */
    void *alarm_arg;                /**< argument passed to the callback */
    rtt_cb_t overflow_cb;           /**< callback called when RTC overflows */
    void *overflow_arg;             /**< argument passed to the callback */
} rtt_state_t;

static rtt_state_t rtt_callback;

void rtt_init(void) {
    printf("RTT init");

    rtt_poweron();

    /*
     * From the datasheet section "Asynchronous Operation of Timer/Counter2"
     * p148 for ATmega1284P.
     * 1. Disable the Timer/Counter2 interrupts by clearing OCIE2x and TOIE2.
     * 2. Select clock source by setting AS2 as appropriate.
     * 3. Write new values to TCNT2, OCR2x, and TCCR2x.
     * 4. To switch to asynchronous: Wait for TCN2UB, OCR2xUB, TCR2xUB.
     * 5. Clear the Timer/Counter2 Interrupt Flags.
     * 6. Enable interrupts, if needed
    */

    /* Disable all timer 2 interrupts */
    TIMSK2 = 0;

    /* Select asynchronous clock source */
    ASSR = (1 << AS2);

    /* Set count register to 0 */
    TCNT2 = 0;

    /* Reset compare value */
    OCR2A = 0;

    /* Reset timer control */
    TCCR2A = 0;

    /* 32768Hz / 1024 -> 32 ticks per second! */
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 <<  CS20);

    /* Wait until not busy anymore */
    DEBUG("RTT yields until ASSR not busy.");
    while( ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << TCR2AUB)) ) {
        thread_yield();
    }

    /* Clear interrupt flags */
    TIFR2 &= ~( (1 << OCF2B)  | (1 << OCF2A) | (1 << TOV2) );

    irq_enable();

    DEBUG("RTT initialized.");
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg) {
    /* Disable overflow interrupt */
    TIMSK2 &= ~(1 << TOIE2);

    rtt_callback.overflow_cb = cb;
    rtt_callback.overflow_arg = arg;

    /* Enable overflow interrupt */
    TIMSK2 |= (1 << TOIE2);
}

void rtt_clear_overflow_cb(void) {
    /* Disable overflow interrupt */
    TIMSK2 &= ~(1 << TOIE2);

    rtt_callback.overflow_cb = NULL;
    rtt_callback.overflow_arg = NULL;
}

uint32_t rtt_get_counter(void) {
    /* Make sure it is safe to read TCNT2, in case we just woke up */
    DEBUG("RTT yields until safe to read TCNT2.");
    TCCR2A = TCCR2A;
    while( ASSR & (1 << TCR2AUB) ) {
        thread_yield();
    }

    return TCNT2;
}

void rtt_set_counter(uint32_t counter) {
    /* Wait until not busy anymore (should be immediate) */
    DEBUG("RTT yields until safe to write TCNT2.");
    while( ASSR & (1 << TCN2UB) ) {
        thread_yield();
    }

    TCNT2 = (uint8_t)counter;

    /* Wait until it is safe to re-enter power-save */
    DEBUG("RTT yields until safe power-save.");
    while( ASSR & (1 << TCN2UB) ) {
        thread_yield();
    }
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg) {
    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2A);

    /* Set callback */
    rtt_callback.alarm_cb = cb;
    rtt_callback.alarm_arg = arg;

    /* Wait until not busy anymore (should be immediate) */
    DEBUG("RTT yields until safe to write OCR2A.");
    while( ASSR & (1 << OCR2AUB) ) {
        thread_yield();
    }

    /* Set alarm value */
    OCR2A = (uint8_t)alarm;

    /* Wait until alarm value takes effect */
    DEBUG("RTT yields until safe power-save.");
    while( ASSR & (1 << OCR2AUB) ) {
        thread_yield();
    }

    /* Enable alarm interrupt */
    TIMSK2 |= (1 << OCIE2A);
}

uint32_t rtt_get_alarm(void) {
    return OCR2A;
}

void rtt_clear_alarm(void) {
    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2A);

    rtt_callback.alarm_cb = NULL;
    rtt_callback.alarm_arg = NULL;
}

void rtt_poweron(void) {
    power_timer2_enable();
}

void rtt_poweroff(void) {
    power_timer2_disable();
}

ISR(TIMER2_OVF_vect) {
    __enter_isr();
    rtt_callback.overflow_cb(void *rtt_callback.overflow_arg);

    /* Wait until it is safe to re-enter power-save */
    DEBUG("RTT OVF yields until safe power-save.");
    TCCR2A = TCCR2A;
    while( ASSR & (1 << TCR2AUB) ) {
        thread_yield();
    }
    __exit_isr();
}

ISR(TIMER2_COMPA_vect) {
    __enter_isr();
    rtt_callback.alarm_cb(void *rtt_callback.alarm_arg);

    /* Wait until it is safe to re-enter power-save */
    DEBUG("RTT alarm yields until safe power-save.");
    TCCR2A = TCCR2A;
    while( ASSR & (1 << TCR2AUB) ) {
        thread_yield();
    }
    __exit_isr();
}

#ifdef __cplusplus
}
#endif

#endif /* RTT_NUMOF */

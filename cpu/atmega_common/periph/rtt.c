/*
 * Copyright (C) 2018 Acutam Automation, LLC
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
 * @note        The RTT only works if the board is equipped with a 32kHz
 *              oscillator.
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * In order to safely sleep when using the RTT:
 * 1. Disable interrupts
 * 2. Write to one of the asynch registers (e.g. TCCR2A)
 * 3. Wait for ASSR register's busy flags to clear
 * 4. Re-enable interrupts
 * 5. Sleep before interrupt re-enable takes effect
 *
 * @}
 */

#include <avr/interrupt.h>

#include "cpu.h"
#include "irq.h"
#include "periph/rtt.h"
#include "periph_conf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

typedef struct {
    uint16_t ext_comp;          /* Extend compare to 24-bits */
    rtt_cb_t alarm_cb;          /* callback called from RTT alarm */
    void *alarm_arg;            /* argument passed to the callback */
    rtt_cb_t overflow_cb;       /* callback called when RTT overflows */
    void *overflow_arg;         /* argument passed to the callback */
} rtt_state_t;

static rtt_state_t rtt_state;
static uint16_t ext_cnt;

static inline void _asynch_wait(void)
{
    /* Wait until all busy flags clear. According to the datasheet,
     * this can take up to 2 positive edges of TOSC1 (32kHz). */
    while (ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB)
                | (1 << TCR2AUB) | (1 << TCR2BUB))) {}
}

/* interrupts are disabled here */
static uint32_t _safe_cnt_get(void)
{
    uint8_t cnt = TCNT2;

    /* If an overflow occurred since we disabled interrupts, manually
     * increment `ext_cnt`
     */
    if (TIFR2 & (1 << TOV2)) {
        ++ext_cnt;

        /* If an overflow occurred just after we read `TCNT2`
           it has overflown back to zero now */
        if (cnt == 255) {
            cnt = 0;
        }

        /* Clear interrupt flag */
        TIFR2 = (1 << TOV2);
    }

    return (ext_cnt << 8) | cnt;
}

static inline uint8_t _rtt_div(uint16_t freq)
{
    switch (freq) {
    case 32768: return 0x1;
    case  4096: return 0x2;
    case  1024: return 0x3;
    case   512: return 0x4;
    case   256: return 0x5;
    case   128: return 0x6;
    case    32: return 0x7;
    default   : assert(0);
    }
}

void rtt_init(void)
{
    DEBUG("Initializing RTT\n");

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

    /* Set counter to 0 */
    TCNT2 = 0;

    /* Reset compare values */
    OCR2A = 0;
    OCR2B = 0;

    /* Reset timer control */
    TCCR2A = 0;

    /* 32768Hz / n */
    TCCR2B = _rtt_div(RTT_FREQUENCY);

    /* Wait until not busy anymore */
    DEBUG("RTT waits until ASSR not busy\n");
    _asynch_wait();

    /* Clear interrupt flags */
    /* Oddly, this is done by writing ones; see datasheet */
    TIFR2 = (1 << OCF2B) | (1 << OCF2A) | (1 << TOV2);

    /* Enable 8-bit overflow interrupt */
    TIMSK2 |= (1 << TOIE2);

    DEBUG("RTT initialized\n");
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    /* Make non-atomic write to callback atomic */
    unsigned state = irq_disable();

    rtt_state.overflow_cb = cb;
    rtt_state.overflow_arg = arg;

    irq_restore(state);
}

void rtt_clear_overflow_cb(void)
{
    /* Make non-atomic write to callback atomic */
    unsigned state = irq_disable();

    rtt_state.overflow_cb = NULL;
    rtt_state.overflow_arg = NULL;

    irq_restore(state);
}

uint32_t rtt_get_counter(void)
{
    unsigned state;
    uint32_t now;

    /* Make sure it is safe to read TCNT2, in case we just woke up */
    DEBUG("RTT sleeps until safe to read TCNT2\n");
    TCCR2A = 0;
    _asynch_wait();

    state = irq_disable();
    now = _safe_cnt_get();
    irq_restore(state);

    return now;
}

void rtt_set_counter(uint32_t counter)
{
    /* Wait until not busy anymore (should be immediate) */
    DEBUG("RTT sleeps until safe to write TCNT2\n");
    _asynch_wait();

    /* Make non-atomic writes atomic (for concurrent access) */
    unsigned state = irq_disable();

    /* Prevent overflow flag from being set during update */
    TCNT2 = 0;

    /* Clear overflow flag */
    /* Oddly, this is done by writing a one; see datasheet */
    TIFR2 = 1 << TOV2;

    ext_cnt = (uint16_t)(counter >> 8);
    TCNT2 = (uint8_t)counter;

    irq_restore(state);

    DEBUG("RTT set counter TCNT2: %" PRIu8 ", ext_cnt: %" PRIu16 "\n",
          TCNT2, ext_cnt);
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    /* Disable alarm */
    rtt_clear_alarm();

    /* Make sure it is safe to read TCNT2, in case we just woke up, and */
    /* safe to write OCR2B (in case it was busy) */
    DEBUG("RTT sleeps until safe read TCNT2 and to write OCR2B\n");
    TCCR2A = 0;
    _asynch_wait();

    /* Make non-atomic writes atomic */
    unsigned state = irq_disable();
    uint32_t now = _safe_cnt_get();

    /* Set the alarm value. Atomic for concurrent access */
    rtt_state.ext_comp = (uint16_t)(alarm >> 8);
    OCR2A = (uint8_t)alarm;

    rtt_state.alarm_cb = cb;
    rtt_state.alarm_arg = arg;

    irq_restore(state);

    DEBUG("RTT set alarm TCNT2: %" PRIu8 ", OCR2A: %" PRIu8 "\n", TCNT2, OCR2A);

    /* Enable alarm interrupt only if it will trigger before overflow */
    if (rtt_state.ext_comp <= (uint16_t)(now >> 8)) {
        /* Clear interrupt flag */
        TIFR2 = (1 << OCF2A);

        /* Enable interrupt */
        TIMSK2 |= (1 << OCIE2A);

        DEBUG("RTT alarm interrupt active\n");
    }
    else {
        DEBUG("RTT alarm interrupt not active\n");
    }
}

uint32_t rtt_get_alarm(void)
{
    return (rtt_state.ext_comp << 8) | OCR2A;
}

void rtt_clear_alarm(void)
{
    /* Make non-atomic writes atomic */
    unsigned state = irq_disable();

    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2A);

    rtt_state.alarm_cb = NULL;
    rtt_state.alarm_arg = NULL;

    irq_restore(state);
}

void rtt_poweron(void)
{
    power_timer2_enable();
}

void rtt_poweroff(void)
{
    power_timer2_disable();
}

ISR(TIMER2_OVF_vect)
{
    atmega_enter_isr();

    ext_cnt++;

    /* Enable RTT alarm if overflowed enough times */
    if (rtt_state.ext_comp <= ext_cnt) {
        /* Clear interrupt flag */
        TIFR2 = (1 << OCF2A);

        /* Enable interrupt */
        TIMSK2 |= (1 << OCIE2A);
    }

    /* Virtual 24-bit timer overflowed */
    if (ext_cnt == 0) {
        /* Execute callback */
        if (rtt_state.overflow_cb != NULL) {
            rtt_state.overflow_cb(rtt_state.overflow_arg);
        }
    }

    atmega_exit_isr();
}

ISR(TIMER2_COMPA_vect)
{
    atmega_enter_isr();
    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2A);

    if (rtt_state.alarm_cb != NULL) {
        /* Clear callback */
        rtt_cb_t cb = rtt_state.alarm_cb;
        rtt_state.alarm_cb = NULL;

        /* Execute callback */
        cb(rtt_state.alarm_arg);
    }

    atmega_exit_isr();
}

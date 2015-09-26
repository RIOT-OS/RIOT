/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation for the ATmega2560 CPU
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include <avr/interrupt.h>

#include "board.h"
#include "cpu.h"
#include "thread.h"

#include "periph/timer.h"
#include "periph_conf.h"

#define IRQ_DISABLED 0x00

typedef struct {
    void (*cb)(int);
} timer_conf_t;

/**
 * @brief Timer state memory
 */
timer_conf_t config[TIMER_NUMOF];

/**
 * @brief Setup the given timer
 *
 */
int timer_init(tim_t dev, unsigned int us_per_ticks, void (*callback)(int))
{
    /* reject impossible us_per_ticks values */
    if ((us_per_ticks != 4)) {
        return -1;
    }


    /* select the timer and enable the timer specific peripheral clocks */
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            TIMER0_COUNTER   = 0;
            TIMER0_CONTROL_B |= TIMER0_FREQ_250KHZ;
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            TIMER1_COUNTER   = 0;
            TIMER1_CONTROL_B |= TIMER1_FREQ_250KHZ;
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            TIMER2_COUNTER   = 0;
            TIMER2_CONTROL_B |= TIMER2_FREQ_250KHZ;
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    /* save callback */
    config[dev].cb = callback;

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    return timer_set_absolute(dev, channel, timer_read(dev) + timeout);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    unsigned state = disableIRQ();

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            switch (channel) {
                case 0:
                    TIMER0_COMP_A = (uint16_t) value;
                    TIMER0_IRQ_FLAG_REG &= ~(1 << TIMER0_COMP_A_FLAG);
                    TIMER0_IRQ_MASK_REG |= (1 << TIMER0_COMP_A_EN);
                    break;

                case 1:
                    TIMER0_COMP_B = (uint16_t) value;
                    TIMER0_IRQ_FLAG_REG &= ~(1 << TIMER0_COMP_B_FLAG);
                    TIMER0_IRQ_MASK_REG |= (1 << TIMER0_COMP_B_EN);
                    break;

                case 2:
                    TIMER0_COMP_C = (uint16_t) value;
                    TIMER0_IRQ_FLAG_REG &= ~(1 << TIMER0_COMP_C_FLAG);
                    TIMER0_IRQ_MASK_REG |= (1 << TIMER0_COMP_C_EN);
                    break;

                default:
                    restoreIRQ(state);
                    return -1;
            }

            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            switch (channel) {
                case 0:
                    TIMER1_COMP_A = (uint16_t) value;
                    TIMER1_IRQ_FLAG_REG &= ~(1 << TIMER1_COMP_A_FLAG);
                    TIMER1_IRQ_MASK_REG |= (1 << TIMER1_COMP_A_EN);
                    break;

                case 1:
                    TIMER1_COMP_B = (uint16_t) value;
                    TIMER1_IRQ_FLAG_REG &= ~(1 << TIMER1_COMP_B_FLAG);
                    TIMER1_IRQ_MASK_REG |= (1 << TIMER1_COMP_B_EN);
                    break;

                case 2:
                    TIMER1_COMP_C = (uint16_t) value;
                    TIMER1_IRQ_FLAG_REG &= ~(1 << TIMER1_COMP_C_FLAG);
                    TIMER1_IRQ_MASK_REG |= (1 << TIMER1_COMP_C_EN);
                    break;

                default:
                    restoreIRQ(state);
                    return -1;
            }

            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            switch (channel) {
                case 0:
                    TIMER2_COMP_A = (uint16_t) value;
                    TIMER2_IRQ_FLAG_REG &= ~(1 << TIMER2_COMP_A_FLAG);
                    TIMER2_IRQ_MASK_REG |= (1 << TIMER2_COMP_A_EN);
                    break;

                case 1:
                    TIMER2_COMP_B = (uint16_t) value;
                    TIMER2_IRQ_FLAG_REG &= ~(1 << TIMER2_COMP_B_FLAG);
                    TIMER2_IRQ_MASK_REG |= (1 << TIMER2_COMP_B_EN);
                    break;

                case 2:
                    TIMER2_COMP_C = (uint16_t) value;
                    TIMER2_IRQ_FLAG_REG &= ~(1 << TIMER2_COMP_C_FLAG);
                    TIMER2_IRQ_MASK_REG |= (1 << TIMER2_COMP_C_EN);
                    break;

                default:
                    restoreIRQ(state);
                    return -1;
            }

            break;
#endif

        case TIMER_UNDEFINED:
        default:
            restoreIRQ(state);
            return -1;
    }

    /* enable interrupts for given timer */
    timer_irq_enable(dev);
    restoreIRQ(state);

    return 1;
}

int timer_clear(tim_t dev, int channel)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            switch (channel) {
                case 0:
                    TIMER0_IRQ_FLAG_REG &= ~(1 << TIMER0_COMP_A_FLAG);
                    break;

                case 1:
                    TIMER0_IRQ_FLAG_REG &= ~(1 << TIMER0_COMP_B_FLAG);
                    break;

                case 2:
                    TIMER0_IRQ_FLAG_REG &= ~(1 << TIMER0_COMP_C_FLAG);
                    break;

                default:
                    return -1;
            }

            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            switch (channel) {
                case 0:
                    TIMER1_IRQ_FLAG_REG &= ~(1 << TIMER1_COMP_A_FLAG);
                    break;

                case 1:
                    TIMER1_IRQ_FLAG_REG &= ~(1 << TIMER1_COMP_B_FLAG);
                    break;

                case 2:
                    TIMER1_IRQ_FLAG_REG &= ~(1 << TIMER1_COMP_C_FLAG);
                    break;

                default:
                    return -1;
                    break;
            }

            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            switch (channel) {
                case 0:
                    TIMER2_IRQ_FLAG_REG &= ~(1 << TIMER2_COMP_A_FLAG);
                    break;

                case 1:
                    TIMER2_IRQ_FLAG_REG &= ~(1 << TIMER2_COMP_B_FLAG);
                    break;

                case 2:
                    TIMER2_IRQ_FLAG_REG &= ~(1 << TIMER2_COMP_C_FLAG);
                    break;

                default:
                    return -1;
                    break;
            }

            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    timer_irq_disable(dev);
    return 1;
}

unsigned int timer_read(tim_t dev)
{
    uint16_t a;
    uint32_t b;
    /*
     * Disabling interrupts globally because read from 16 Bit register can
     * otherwise be messed up
     */
    unsigned state = disableIRQ();

    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            do {
                a = TIMER0_COUNTER;
                b = TIMER0_COUNTER;
            } while (a != b);
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            do {
                a = TIMER1_COUNTER;
                b = TIMER1_COUNTER;
            } while (a != b);
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            do {
                a = TIMER2_COUNTER;
                b = TIMER2_COUNTER;
            } while (a != b);
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            a = 0;
    }

    restoreIRQ(state);
    return a;
}

void timer_stop(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            TIMER0_CONTROL_B = TIMER0_FREQ_DISABLE;
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            TIMER1_CONTROL_B = TIMER1_FREQ_DISABLE;
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            TIMER2_CONTROL_B = TIMER2_FREQ_DISABLE;
            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }
}

void timer_start(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            TIMER0_CONTROL_B |= TIMER0_FREQ_250KHZ;
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            TIMER1_CONTROL_B |= TIMER1_FREQ_250KHZ;
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            TIMER1_CONTROL_B |= TIMER1_FREQ_250KHZ;
            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_enable(tim_t dev)
{
#ifdef DEVELHELP
    printf("timer_irq_enable not implemented\n");
#endif
}

void timer_irq_disable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER0_IRQ_MASK_REG &= ~(1 << TIMER0_COMP_A_EN);
            TIMER0_IRQ_MASK_REG &= ~(1 << TIMER0_COMP_B_EN);
            TIMER0_IRQ_MASK_REG &= ~(1 << TIMER0_COMP_C_EN);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER1_IRQ_MASK_REG &= ~(1 << TIMER1_COMP_A_EN);
            TIMER1_IRQ_MASK_REG &= ~(1 << TIMER1_COMP_B_EN);
            TIMER1_IRQ_MASK_REG &= ~(1 << TIMER1_COMP_C_EN);
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER2_IRQ_MASK_REG &= ~(1 << TIMER2_COMP_A_EN);
            TIMER2_IRQ_MASK_REG &= ~(1 << TIMER2_COMP_B_EN);
            TIMER2_IRQ_MASK_REG &= ~(1 << TIMER2_COMP_C_EN);
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_reset(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER0_COUNTER = 0;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER1_COUNTER = 0;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER2_COUNTER = 0;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

static inline void _isr(int timer, int chan)
{
    __enter_isr();
    timer_clear(timer, chan);

    config[timer].cb(chan);

    if (sched_context_switch_request) {
        thread_yield();
    }
    __exit_isr();
}

#if TIMER_0_EN
ISR(TIMER0_COMPA_ISR, ISR_BLOCK)
{
    _isr(0, 0);
}

ISR(TIMER0_COMPB_ISR, ISR_BLOCK)
{
    _isr(0, 1);
}

ISR(TIMER0_COMPC_ISR, ISR_BLOCK)
{
    _isr(0, 2);
}
#endif /* TIMER_0_EN */

#if TIMER_1_EN
ISR(TIMER1_COMPA_ISR, ISR_BLOCK)
{
    _isr(1, 0);
}

ISR(TIMER1_COMPB_ISR, ISR_BLOCK)
{
    _isr(1, 1);
}

ISR(TIMER1_COMPC_ISR, ISR_BLOCK)
{
    _isr(1, 2);
}
#endif /* TIMER_1_EN */

#if TIMER_2_EN
ISR(TIMER2_COMPA_ISR, ISR_BLOCK)
{
    _isr(2, 0);
}

ISR(TIMER2_COMPB_ISR, ISR_BLOCK)
{
    _isr(2, 1);
}

ISR(TIMER2_COMPC_ISR, ISR_BLOCK)
{
    _isr(2, 2);
}
#endif /* TIMER_2_EN */

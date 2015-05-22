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

#include "periph/timer.h"
#include "periph_conf.h"

static inline int __set_timer(tim_t dev,
                              int channel,
                              unsigned int timeout,
                              unsigned int interval
                             );

#define IRQ_DISABLED 0x00

typedef struct {
    void (*cb)(int);
    volatile uint8_t ctr_a;
    volatile uint8_t ctr_b;
    volatile uint8_t ctr_c;
    uint8_t limit;
    uint16_t timeout_a;
    uint16_t timeout_b;
    uint16_t timeout_c;
} timer_conf_t;

/**
 * @brief Timer state memory
 */
timer_conf_t config[TIMER_NUMOF];

/**
 * @brief Setup the given timer
 *
 */
int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    /* reject impossible ticks_per_us values */
    if ((ticks_per_us > 16) && (ticks_per_us == 0)) {
        return -1;
    }

    config[dev].limit = 16 / ticks_per_us;
    config[dev].ctr_a = 0x00;
    config[dev].ctr_b = 0x00;
    config[dev].ctr_c = 0x00;

    /* select the timer and enable the timer specific peripheral clocks */
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            TIMER0_COUNTER   = 0;
            TIMER0_CONTROL_B |= TIMER0_FREQ_16MHZ;
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            TIMER1_COUNTER   = 0;
            TIMER1_CONTROL_B |= TIMER1_FREQ_16MHZ;
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            TIMER2_COUNTER   = 0;
            TIMER2_CONTROL_B |= TIMER2_FREQ_16MHZ;
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
    return __set_timer(dev, channel, timer_read(dev) + timeout, timeout);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int timeout)
{
    return __set_timer(dev, channel, timeout, timeout);
}

int timer_clear(tim_t dev, int channel)
{

    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            switch (channel) {
                case 0:
                    TIMER0_IRQ_FLAG_REG &= ~(1 << TIMER0_COMP_A_FLAG);
                    config[dev].timeout_a = IRQ_DISABLED;
                    break;

                case 1:
                    TIMER0_IRQ_FLAG_REG &= ~(1 << TIMER0_COMP_B_FLAG);
                    config[dev].timeout_b = IRQ_DISABLED;
                    break;

                case 2:
                    TIMER0_IRQ_FLAG_REG &= ~(1 << TIMER0_COMP_C_FLAG);
                    config[dev].timeout_c = IRQ_DISABLED;
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
                    config[dev].timeout_a = IRQ_DISABLED;
                    break;

                case 1:
                    TIMER1_IRQ_FLAG_REG &= ~(1 << TIMER1_COMP_B_FLAG);
                    config[dev].timeout_b = IRQ_DISABLED;
                    break;

                case 2:
                    TIMER1_IRQ_FLAG_REG &= ~(1 << TIMER1_COMP_C_FLAG);
                    config[dev].timeout_c = IRQ_DISABLED;
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
                    config[dev].timeout_a = IRQ_DISABLED;
                    break;

                case 1:
                    TIMER2_IRQ_FLAG_REG &= ~(1 << TIMER2_COMP_B_FLAG);
                    config[dev].timeout_b = IRQ_DISABLED;
                    break;

                case 2:
                    TIMER2_IRQ_FLAG_REG &= ~(1 << TIMER2_COMP_C_FLAG);
                    config[dev].timeout_c = IRQ_DISABLED;
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
    uint16_t value;
    /*
     * Disabling interrupts globally because read from 16 Bit register can
     * otherwise be messed up
     */
    disableIRQ();

    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            value = TIMER0_COUNTER;
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            value = TIMER1_COUNTER;
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            value = TIMER2_COUNTER;
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            value = 0;
            enableIRQ();
    }

    enableIRQ();
    return value;
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
            TIMER0_CONTROL_B |= TIMER0_FREQ_16MHZ;
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            TIMER1_CONTROL_B |= TIMER1_FREQ_16MHZ;
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            TIMER1_CONTROL_B |= TIMER1_FREQ_16MHZ;
            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_enable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            if (config[dev].timeout_a != IRQ_DISABLED) {
                TIMER0_IRQ_MASK_REG |= (1 << TIMER0_COMP_A_EN);
            }

            if (config[dev].timeout_b != IRQ_DISABLED) {
                TIMER0_IRQ_MASK_REG |= (1 << TIMER0_COMP_B_EN);
            }

            if (config[dev].timeout_c != IRQ_DISABLED) {
                TIMER0_IRQ_MASK_REG |= (1 << TIMER0_COMP_C_EN);
            }

            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            if (config[dev].timeout_a != IRQ_DISABLED) {
                TIMER1_IRQ_MASK_REG |= (1 << TIMER1_COMP_A_EN);
            }

            if (config[dev].timeout_b != IRQ_DISABLED) {
                TIMER1_IRQ_MASK_REG |= (1 << TIMER1_COMP_B_EN);
            }

            if (config[dev].timeout_c != IRQ_DISABLED) {
                TIMER1_IRQ_MASK_REG |= (1 << TIMER1_COMP_C_EN);
            }

            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            if (config[dev].timeout_a != IRQ_DISABLED) {
                TIMER2_IRQ_MASK_REG |= (1 << TIMER2_COMP_A_EN);
            }

            if (config[dev].timeout_b != IRQ_DISABLED) {
                TIMER2_IRQ_MASK_REG |= (1 << TIMER2_COMP_B_EN);
            }

            if (config[dev].timeout_c != IRQ_DISABLED) {
                TIMER2_IRQ_MASK_REG |= (1 << TIMER2_COMP_C_EN);
            }

            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }

    enableIRQ();
}

void timer_irq_disable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            if (config[dev].timeout_a == IRQ_DISABLED) {
                TIMER0_IRQ_MASK_REG &= ~(1 << TIMER0_COMP_A_EN);
            }

            if (config[dev].timeout_b == IRQ_DISABLED) {
                TIMER0_IRQ_MASK_REG &= ~(1 << TIMER0_COMP_B_EN);
            }

            if (config[dev].timeout_c == IRQ_DISABLED) {
                TIMER0_IRQ_MASK_REG &= ~(1 << TIMER0_COMP_C_EN);
            }

            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            if (config[dev].timeout_a == IRQ_DISABLED) {
                TIMER1_IRQ_MASK_REG &= ~(1 << TIMER1_COMP_A_EN);
            }

            if (config[dev].timeout_b == IRQ_DISABLED) {
                TIMER1_IRQ_MASK_REG &= ~(1 << TIMER1_COMP_B_EN);
            }

            if (config[dev].timeout_c == IRQ_DISABLED) {
                TIMER1_IRQ_MASK_REG &= ~(1 << TIMER1_COMP_C_EN);
            }

            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            if (config[dev].timeout_a == IRQ_DISABLED) {
                TIMER2_IRQ_MASK_REG &= ~(1 << TIMER2_COMP_A_EN);
            }

            if (config[dev].timeout_b == IRQ_DISABLED) {
                TIMER2_IRQ_MASK_REG &= ~(1 << TIMER2_COMP_B_EN);
            }

            if (config[dev].timeout_c == IRQ_DISABLED) {
                TIMER2_IRQ_MASK_REG &= ~(1 << TIMER2_COMP_C_EN);
            }

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

inline int __set_timer(tim_t dev, int channel, unsigned int timeout, unsigned int interval)
{
    /*
     * Disabling interrupts globally because write to 16 Bit register can
     * otherwise be messed up
     */
    disableIRQ();

    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            switch (channel) {
                case 0:
                    TIMER0_COMP_A = (uint16_t) timeout * config[dev].limit;
                    config[dev].timeout_a = interval;
                    break;

                case 1:
                    TIMER0_COMP_B = (uint16_t) timeout * config[dev].limit;
                    config[dev].timeout_b = interval;
                    break;

                case 2:
                    TIMER0_COMP_C = (uint16_t) timeout * config[dev].limit;
                    config[dev].timeout_c = interval;
                    break;

                default:
                    enableIRQ();
                    return -1;
            }

            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            switch (channel) {
                case 0:
                    TIMER1_COMP_A = (uint16_t) timeout * config[dev].limit;
                    config[dev].timeout_a = interval;
                    break;

                case 1:
                    TIMER1_COMP_B = (uint16_t) timeout * config[dev].limit;
                    config[dev].timeout_b = interval;
                    break;

                case 2:
                    TIMER1_COMP_C = (uint16_t) timeout * config[dev].limit;
                    config[dev].timeout_c = interval;
                    break;

                default:
                    enableIRQ();
                    return -1;
            }

            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            switch (channel) {
                case 0:
                    TIMER2_COMP_A = (uint16_t) timeout * config[dev].limit;
                    config[dev].timeout_a = interval;
                    break;

                case 1:
                    TIMER2_COMP_B = (uint16_t) timeout * config[dev].limit;
                    config[dev].timeout_b = interval;
                    break;

                case 2:
                    TIMER2_COMP_C = (uint16_t) timeout * config[dev].limit;
                    config[dev].timeout_c = interval;
                    break;

                default:
                    enableIRQ();
                    return -1;
            }

            break;
#endif

        case TIMER_UNDEFINED:
        default:
            enableIRQ();
            return -1;
    }

    /* enable interrupts for given timer */
    timer_irq_enable(dev);
    enableIRQ();

    return 1;
}

#if TIMER_0_EN
ISR(TIMER0_COMPA_ISR, ISR_BLOCK)
{
    config[TIMER_0].ctr_a++;

    if (config[TIMER_0].ctr_a >= config[TIMER_0].limit) {
        config[TIMER_0].limit = 0;
        config[TIMER_0].cb(0);
        TIMER0_COMP_A = TIMER0_COMP_A + config[TIMER_0].timeout_a * config[TIMER_0].limit;
    }
}

ISR(TIMER0_COMPB_ISR, ISR_BLOCK)
{
    config[TIMER_0].ctr_b++;

    if (config[TIMER_0].ctr_b >= config[TIMER_0].limit) {
        config[TIMER_0].limit = 0;
        config[TIMER_0].cb(1);
        TIMER0_COMP_B = TIMER0_COMP_B + config[TIMER_0].timeout_b * config[TIMER_0].limit;
    }
}

ISR(TIMER0_COMPC_ISR, ISR_BLOCK)
{
    config[TIMER_0].ctr_c++;

    if (config[TIMER_0].ctr_c >= config[TIMER_0].limit) {
        config[TIMER_0].limit = 0;
        config[TIMER_0].cb(2);
        TIMER0_COMP_C = TIMER0_COMP_C + config[TIMER_0].timeout_c * config[TIMER_0].limit;
    }
}
#endif /* TIMER_0_EN */

#if TIMER_1_EN
ISR(TIMER1_COMPA_ISR, ISR_BLOCK)
{
    config[TIMER_1].ctr_a++;

    if (config[TIMER_1].ctr_a >= config[TIMER_1].limit) {
        config[TIMER_1].limit = 0;
        config[TIMER_1].cb(0);
        TIMER1_COMP_A = TIMER1_COMP_A + config[TIMER_1].timeout_a * config[TIMER_1].limit;
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

ISR(TIMER1_COMPB_ISR, ISR_BLOCK)
{
    config[TIMER_1].ctr_b++;

    if (config[TIMER_1].ctr_b >= config[TIMER_1].limit) {
        config[TIMER_1].limit = 0;
        config[TIMER_1].cb(1);
        TIMER1_COMP_B = TIMER1_COMP_B + config[TIMER_1].timeout_b * config[TIMER_1].limit;
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

ISR(TIMER1_COMPC_ISR, ISR_BLOCK)
{
    config[TIMER_1].ctr_c++;

    if (config[TIMER_1].ctr_c >= config[TIMER_1].limit) {
        config[TIMER_1].limit = 0;
        config[TIMER_1].cb(2);
        TIMER1_COMP_C = TIMER1_COMP_C + config[TIMER_1].timeout_c * config[TIMER_1].limit;
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /* TIMER_1_EN */

#if TIMER_2_EN
ISR(TIMER2_COMPA_ISR, ISR_BLOCK)
{
    config[TIMER_2].ctr_a++;

    if (config[TIMER_2].ctr_a >= config[TIMER_2].limit) {
        config[TIMER_2].limit = 0;
        config[TIMER_2].cb(0);
        TIMER2_COMP_A = TIMER2_COMP_A + config[TIMER_2].timeout_a * config[TIMER_2].limit;
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

ISR(TIMER2_COMPB_ISR, ISR_BLOCK)
{
    config[TIMER_2].ctr_b++;

    if (config[TIMER_2].ctr_b >= config[TIMER_2].limit) {
        config[TIMER_2].limit = 0;
        config[TIMER_2].cb(1);
        TIMER2_COMP_B = TIMER2_COMP_B + config[TIMER_2].timeout_b * config[TIMER_2].limit;
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

ISR(TIMER2_COMPC_ISR, ISR_BLOCK)
{
    config[TIMER_2].ctr_c++;

    if (config[TIMER_2].ctr_c >= config[TIMER_2].limit) {
        config[TIMER_2].limit = 0;
        config[TIMER_2].cb(2);
        TIMER2_COMP_C = TIMER2_COMP_C + config[TIMER_2].timeout_c * config[TIMER_2].limit;
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /* TIMER_2_EN */

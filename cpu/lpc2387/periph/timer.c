/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc2387
 * @{
 *
 * @file
 * @brief       Implementation of the low-level timer driver for the LPC2387
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "periph_conf.h"
#include "periph_cpu.h"
#include "periph/timer.h"

/**
 * @brief   Check the board config to make sure we do not exceed max number of
 *          timers
 */
#if TIMER_NUMOF > 3
#error "ERROR in timer configuration: too many timers defined"
#endif

/**
 * @brief   Interrupt context information for configured timers
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

/**
 * @brief   Forward declarations for interrupt functions
 * @{
 */
void tim_isr_0(void);
void tim_isr_1(void);
void tim_isr_2(void);
void tim_isr_3(void);
/** @} */

/**
 * @brief   Get the base pointer of a timer
 */
static inline lpc23xx_timer_t *get_dev(tim_t tim)
{
    switch (tim) {
        case 0:
            return TMR0;
#if TIMER_NUMOF > 1
        case 1:
            return TMR1;
#endif
#if TIMER_NUMOF > 2
        case 2:
            return TMR2;
#endif
#if TIMER_NUMOF > 3
        case 3:
            return TMR3;
#endif
        default:
            return NULL;
    }
}

static inline void pwr_clk_and_isr(tim_t tim)
{
    switch (tim) {
        case 0:
            PCONP |= (1 << 1);
            PCLKSEL0 &= ~(0x03 << 2);
            PCLKSEL0 |=  (0x01 << 2);
            install_irq(TIMER0_INT, &tim_isr_0, 1);
            break;
#if TIMER_NUMOF > 1
        case 1:
            PCONP |= (1 << 2);
            PCLKSEL0 &= ~(0x03 << 4);
            PCLKSEL0 |=  (0x01 << 4);
            install_irq(TIMER1_INT, &tim_isr_1, 1);
            break;
#endif
#if TIMER_NUMOF > 2
        case 2:
            PCONP |= (1 << 22);
            PCLKSEL1 &= ~(0x03 << 12);
            PCLKSEL1 |=  (0x01 << 12);
            install_irq(TIMER2_INT, &tim_isr_2, 1);
            break;
#endif
#if TIMER_NUMOF > 3
        case 3:
            PCONP |= (1 << 23);
            PCLKSEL1 &= ~(0x03 << 14);
            PCLKSEL1 |=  (0x01 << 14);
            install_irq(TIMER3_INT, &tim_isr_3, 1);
            break;
#endif
    }
}

int timer_init(tim_t tim, unsigned int us_per_tick, void (*callback)(int))
{
    /* get the timers base register */
    lpc23xx_timer_t *dev = get_dev(tim);

    /* make sure the timer device is valid */
    if (dev == NULL) {
        return -1;
    }

    /* save the callback */
    isr_ctx[tim].cb = callback;
    /* enable power, config periph clock and install ISR vector */
    pwr_clk_and_isr(tim);
    /* reset timer configuration (sets the timer to timer mode) */
    dev->TCR = 0;
    dev->CTCR = 0;
    /* configure the prescaler */
    dev->PR = (us_per_tick * ((CLOCK_PCLK / 1000000) - 1));
    /* enable timer */
    dev->TCR = 1;
    return 0;
}

int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    unsigned int now = timer_read(tim);
    return timer_set_absolute(tim, channel, (timeout + now));
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    if (tim >= TIMER_NUMOF || channel >= TIMER_CHAN_NUMOF) {
        return -1;
    }

    lpc23xx_timer_t *dev = get_dev(tim);
    dev->MR[channel] = value;
    dev->MCR |= (1 << (channel * 3));
    return 0;
}

int timer_clear(tim_t tim, int channel)
{
    if (tim >= TIMER_NUMOF || channel >= TIMER_CHAN_NUMOF) {
        return -1;
    }
    get_dev(tim)->MCR &= ~(1 << (channel * 3));
    return 0;
}

unsigned int timer_read(tim_t tim)
{
    return (unsigned int)(get_dev(tim)->TC);
}

void timer_start(tim_t tim)
{
    get_dev(tim)->TCR = 1;
}

void timer_stop(tim_t tim)
{
    get_dev(tim)->TCR = 0;
}

void timer_irq_enable(tim_t tim)
{
    /* TODO */
}

void timer_irq_disable(tim_t tim)
{
    /* TODO */
}

static inline void isr_handler(lpc23xx_timer_t *dev, int tim_num)
{
    for (int i = 0; i < TIMER_CHAN_NUMOF; i++) {
        if (dev->IR & (1 << i)) {
            dev->IR |= (1 << i);
            dev->MCR &= ~(1 << (i * 3));
            isr_ctx[tim_num].cb(i);
        }
    }
    /* we must not forget to acknowledge the handling of the interrupt */
    VICVectAddr = 0;
}

void __attribute__((interrupt("IRQ"))) tim_isr_0(void)
{
    isr_handler(TMR0, 0);
}

#if TIMER_NUMOF > 1
void __attribute__((interrupt("IRQ"))) tim_isr_1(void)
{
    isr_handler(TMR1, 1);
}
#endif

#if TIMER_NUMOF > 2
void __attribute__((interrupt("IRQ"))) tim_isr_2(void)
{
    isr_handler(TMR2, 2);
}
#endif

#if TIMER_NUMOF > 3
void __attribute__((interrupt("IRQ"))) tim_isr_3(void)
{
    isr_handler(TMR3, 3);
}
#endif

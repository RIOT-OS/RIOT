/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/timer.h"

/**
 * @brief   Timer specific additional bus clock presacler
 *
 * This prescale factor is dependent on the actual APBx bus clock divider, if
 * the APBx presacler is != 1, it is set to 2, if the APBx prescaler is == 1, it
 * is set to 1.
 *
 * See reference manuals section 'reset and clock control'.
 */
static const uint8_t apbmul[] = {
#if (CLOCK_APB1 < CLOCK_CORECLOCK)
    [APB1] = 2,
#else
    [APB1] = 1,
#endif
#if (CLOCK_APB2 < CLOCK_CORECLOCK)
    [APB2] = 2
#else
    [APB2] = 1
#endif
};

/**
 * @brief   Interrupt context for each configured timer
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

/**
 * @brief   Get the timer device
 */
static inline TIM_TypeDef *dev(tim_t tim)
{
    return timer_config[tim].dev;
}

int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
    /* check if device is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* remember the interrupt context */
    isr_ctx[tim].cb = cb;
    isr_ctx[tim].arg = arg;

    /* enable the peripheral clock */
    periph_clk_en(timer_config[tim].bus, timer_config[tim].rcc_mask);

    /* configure the timer as upcounter in continuous mode */
    dev(tim)->CR1  = 0;
    dev(tim)->CR2  = 0;
    dev(tim)->ARR  = timer_config[tim].max;

    /* set prescaler */
    dev(tim)->PSC = (((periph_apb_clk(timer_config[tim].bus) *
                       apbmul[timer_config[tim].bus]) / freq) - 1);
    /* generate an update event to apply our configuration */
    dev(tim)->EGR = TIM_EGR_UG;

    /* enable the timer's interrupt */
    NVIC_EnableIRQ(timer_config[tim].irqn);
    /* reset the counter and start the timer */
    timer_start(tim);

    return 0;
}

int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    int now = timer_read(tim);
    return timer_set_absolute(tim, channel, now + timeout);
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    if (channel >= TIMER_CHAN) {
        return -1;
    }

    dev(tim)->CCR[channel] = (value & timer_config[tim].max);
    dev(tim)->SR &= ~(TIM_SR_CC1IF << channel);
    dev(tim)->DIER |= (TIM_DIER_CC1IE << channel);

    return 0;
}

int timer_clear(tim_t tim, int channel)
{
    if (channel >= TIMER_CHAN) {
        return -1;
    }

    dev(tim)->DIER &= ~(TIM_DIER_CC1IE << channel);
    return 0;
}

unsigned int timer_read(tim_t tim)
{
    return (unsigned int)dev(tim)->CNT;
}

void timer_start(tim_t tim)
{
    dev(tim)->CR1 |= TIM_CR1_CEN;
}

void timer_stop(tim_t tim)
{
    dev(tim)->CR1 &= ~(TIM_CR1_CEN);
}

static inline void irq_handler(tim_t tim)
{
    uint32_t status = (dev(tim)->SR & dev(tim)->DIER);

    for (unsigned int i = 0; i < TIMER_CHAN; i++) {
        if (status & (TIM_SR_CC1IF << i)) {
            dev(tim)->DIER &= ~(TIM_DIER_CC1IE << i);
            isr_ctx[tim].cb(isr_ctx[tim].arg, i);
        }
    }
    cortexm_isr_end();
}

#ifdef TIMER_0_ISR
void TIMER_0_ISR(void)
{
    irq_handler(0);
}
#endif

#ifdef TIMER_1_ISR
void TIMER_1_ISR(void)
{
    irq_handler(1);
}
#endif

#ifdef TIMER_2_ISR
void TIMER_2_ISR(void)
{
    irq_handler(2);
}
#endif

#ifdef TIMER_3_ISR
void TIMER_3_ISR(void)
{
    irq_handler(3);
}
#endif

#ifdef TIMER_4_ISR
void TIMER_4_ISR(void)
{
    irq_handler(4);
}
#endif

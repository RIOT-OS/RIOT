/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l1
 * @{
 *
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>

#include "cpu.h"
#include "board.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"

/** Unified IRQ handler for all timers */
static inline void irq_handler(tim_t timer, TIM_TypeDef *dev);

/** Timer state memory */
static timer_isr_ctx_t config[TIMER_NUMOF];

/**
 * @brief   Get the timer device
 */
static inline TIM_TypeDef *get_dev(tim_t tim)
{
    return timer_config[tim].dev;
}

    

int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
    /* check if device is valid */
    if (dev >= TIMER_NUMOF) {
        return -1;
    }

    /* enable timer peripheral clock */
    periph_clk_en(timer_config[dev].bus, timer_config[dev].rcc_mask);

    /* set timer's IRQ priority */
    NVIC_SetPriority(timer_config[dev].irqn, timer_config[dev].priority);

    /* set prescaler */
    get_dev(dev)->PSC = (timer_config[dev].freq / freq) - 1;

    /* set callback function */
    config[dev].cb = cb;
    config[dev].arg = arg;

    /* set timer to run in counter mode */
    get_dev(dev)->CR1 = 0;
    get_dev(dev)->CR2 = 0;

    /* set auto-reload and prescaler values and load new values */
    get_dev(dev)->EGR |= TIM_EGR_UG;

    /* enable the timer's interrupt */
    timer_irq_enable(dev);

    /* start the timer */
    timer_start(dev);

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    int now = timer_read(dev);
    return timer_set_absolute(dev, channel, now + timeout - 1);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (channel >= timer_config[dev].channels || dev >= TIMER_NUMOF) {
        return -1;
    }

    switch (channel) {
        case 0:
            get_dev(dev)->CCR1 = value;
            get_dev(dev)->SR &= ~TIM_SR_CC1IF;
            get_dev(dev)->DIER |= TIM_DIER_CC1IE;
            break;
        case 1:
            get_dev(dev)->CCR2 = value;
            get_dev(dev)->SR &= ~TIM_SR_CC2IF;
            get_dev(dev)->DIER |= TIM_DIER_CC2IE;
            break;
        case 2:
            get_dev(dev)->CCR3 = value;
            get_dev(dev)->SR &= ~TIM_SR_CC3IF;
            get_dev(dev)->DIER |= TIM_DIER_CC3IE;
            break;
        case 3:
            get_dev(dev)->CCR4 = value;
            get_dev(dev)->SR &= ~TIM_SR_CC4IF;
            get_dev(dev)->DIER |= TIM_DIER_CC4IE;
            break;
        default:
            return -1;
    }

    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    if (channel >= timer_config[dev].channels || dev >= TIMER_NUMOF) {
        return -1;
    }

    get_dev(dev)->DIER &= ~(TIM_DIER_CC1IE << channel);

    return 0;
}

unsigned int timer_read(tim_t dev)
{
    return (unsigned int)get_dev(dev)->CNT;
}

void timer_start(tim_t dev)
{
    get_dev(dev)->CR1 |= TIM_CR1_CEN;
}

void timer_stop(tim_t dev)
{
    get_dev(dev)->CR1 &= ~TIM_CR1_CEN;
}

void timer_irq_enable(tim_t dev)
{
    NVIC_EnableIRQ(timer_config[dev].irqn);
}

void timer_irq_disable(tim_t dev)
{
    NVIC_DisableIRQ(timer_config[dev].irqn);
}


void TIMER_0_ISR(void)
{
    irq_handler(TIMER_0, get_dev(TIMER_0));
}

void TIMER_1_ISR(void)
{
    irq_handler(TIMER_1, get_dev(TIMER_1));
}

void TIMER_2_ISR(void)
{
    irq_handler(TIMER_2, get_dev(TIMER_2));
}

static inline void irq_handler(tim_t timer, TIM_TypeDef *dev)
{
    if (dev->SR & TIM_SR_CC1IF) {
        dev->DIER &= ~TIM_DIER_CC1IE;
        dev->SR &= ~TIM_SR_CC1IF;
        config[timer].cb(config[timer].arg, 0);
    }
    else if (dev->SR & TIM_SR_CC2IF) {
        dev->DIER &= ~TIM_DIER_CC2IE;
        dev->SR &= ~TIM_SR_CC2IF;
        config[timer].cb(config[timer].arg, 1);
    }
    else if (dev->SR & TIM_SR_CC3IF) {
        dev->DIER &= ~TIM_DIER_CC3IE;
        dev->SR &= ~TIM_SR_CC3IF;
        config[timer].cb(config[timer].arg, 2);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f0
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>

#include "cpu.h"
#include "board.h"
#include "periph_conf.h"
#include "periph/timer.h"


static inline void irq_handler(tim_t timer, TIM_TypeDef *dev);

typedef struct {
    void (*cb)(int);
} timer_conf_t;

/**
 * Timer state memory
 */
timer_conf_t config[TIMER_NUMOF];


int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    TIM_TypeDef *timer;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            /* enable timer peripheral clock */
            TIMER_0_CLKEN();
            /* set timer's IRQ priority */
            NVIC_SetPriority(TIMER_0_IRQ_CHAN, TIMER_0_IRQ_PRIO);
            /* select timer */
            timer = TIMER_0_DEV;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            /* enable timer peripheral clock */
            TIMER_1_CLKEN();
            /* set timer's IRQ priority */
            NVIC_SetPriority(TIMER_1_IRQ_CHAN, TIMER_1_IRQ_PRIO);
            /* select timer */
            timer = TIMER_1_DEV;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    /* set callback function */
    config[dev].cb = callback;

    /* set timer to run in counter mode */
    timer->CR1 |= TIM_CR1_URS;

    /* set auto-reload and prescaler values and load new values */
    timer->ARR = TIMER_0_MAX_VALUE;
    timer->PSC = TIMER_0_PRESCALER * ticks_per_us;
    timer->EGR |= TIM_EGR_UG;

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
    TIM_TypeDef *timer = NULL;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            timer = TIMER_0_DEV;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            timer = TIMER_1_DEV;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    switch (channel) {
        case 0:
            timer->CCR1 = value;
            timer->SR &= ~TIM_SR_CC1IF;
            timer->DIER |= TIM_DIER_CC1IE;
            break;
        case 1:
            timer->CCR2 = value;
            timer->SR &= ~TIM_SR_CC2IF;
            timer->DIER |= TIM_DIER_CC2IE;
            break;
        case 2:
            timer->CCR3 = value;
            timer->SR &= ~TIM_SR_CC3IF;
            timer->DIER |= TIM_DIER_CC3IE;
            break;
        case 3:
            timer->CCR4 = value;
            timer->SR &= ~TIM_SR_CC4IF;
            timer->DIER |= TIM_DIER_CC4IE;
            break;
        default:
            return -1;
    }
    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    TIM_TypeDef *timer;
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            timer = TIMER_0_DEV;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            timer = TIMER_1_DEV;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }
    switch (channel) {
        case 0:
            timer->DIER &= ~TIM_DIER_CC1IE;
            break;
        case 1:
            timer->DIER &= ~TIM_DIER_CC2IE;
            break;
        case 2:
            timer->DIER &= ~TIM_DIER_CC3IE;
            break;
        case 3:
            timer->DIER &= ~TIM_DIER_CC4IE;
            break;
        default:
            return -1;
    }
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            return TIMER_0_DEV->CNT;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            return TIMER_1_DEV->CNT;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return 0;
    }
}

void timer_start(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->CR1 |= TIM_CR1_CEN;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CR1 |= TIM_CR1_CEN;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_stop(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->CR1 &= ~TIM_CR1_CEN;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CR1 &= ~TIM_CR1_CEN;
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
            NVIC_EnableIRQ(TIMER_0_IRQ_CHAN);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_EnableIRQ(TIMER_1_IRQ_CHAN);
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_disable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            NVIC_DisableIRQ(TIMER_0_IRQ_CHAN);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_DisableIRQ(TIMER_1_IRQ_CHAN);
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
            TIMER_0_DEV->CNT = 0;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CNT = 0;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

#if TIMER_0_EN
void TIMER_0_ISR(void)
{
    irq_handler(TIMER_0, TIMER_0_DEV);
}
#endif

#if TIMER_1_EN
void TIMER_1_ISR(void)
{
    irq_handler(TIMER_1, TIMER_1_DEV);
}
#endif

static inline void irq_handler(tim_t timer, TIM_TypeDef *dev)
{
    if (dev->SR & TIM_SR_CC1IF) {
        dev->DIER &= ~TIM_DIER_CC1IE;
        dev->SR &= ~TIM_SR_CC1IF;
        config[timer].cb(0);
    }
    else if (dev->SR & TIM_SR_CC2IF) {
        dev->DIER &= ~TIM_DIER_CC2IE;
        dev->SR &= ~TIM_SR_CC2IF;
        config[timer].cb(1);
    }
    else if (dev->SR & TIM_SR_CC3IF) {
        dev->DIER &= ~TIM_DIER_CC3IE;
        dev->SR &= ~TIM_SR_CC3IF;
        config[timer].cb(2);
    }
    else if (dev->SR & TIM_SR_CC4IF) {
        dev->DIER &= ~TIM_DIER_CC4IE;
        dev->SR &= ~TIM_SR_CC4IF;
        config[timer].cb(3);
    }
}

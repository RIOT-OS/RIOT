/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  driver_periph
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"

/**
 * @brief   All timers on this CPU have 4 channels
 */
#define CHANNEL_NUMOF       (4U)

/**
 * @brief   Interrupt state
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

/**
 * @brief   Get the timers base register
 */
static inline TIM_TypeDef *_tim(tim_t dev)
{
    return timer_config[dev].dev;
}

int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    TIM_TypeDef *tim;

    /* check if given timer exists */
    if (dev >= TIMER_NUMOF) {
        return -1;
    }

    /* get base register */
    tim = _tim(dev);
    /* save callback */
    isr_ctx[dev].cb = callback;
    /* enable peripheral clock */
    RCC->APB1ENR |= (1 << timer_config[dev].rcc);
    /* reset timer and configure to up-counting mode */
    tim->CR1 = 0;
    tim->CR2 = 0;
    tim->SR = 0;
    /* configure reload and pre-scaler values */
    tim->ARR = 0xffffffff;
    tim->PSC = (((CLOCK_CORECLOCK / 1000000) * ticks_per_us) - 1);
    /* trigger update event to make pre-scaler value effective */
    tim->EGR = TIM_EGR_UG;
    /* enable interrupts and start the timer */
    timer_irq_enable(dev);
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
    TIM_TypeDef *tim;

    if (dev >= TIMER_NUMOF || channel >= CHANNEL_NUMOF || channel < 0) {
        return -1;
    }

    tim = _tim(dev);
    tim->CCR[channel] = value;
    tim->SR &= ~(1 << (channel + 1));
    tim->DIER |= (1 << (channel + 1));
    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    TIM_TypeDef *tim;

    if (dev >= TIMER_NUMOF || channel >= CHANNEL_NUMOF || channel < 0) {
        return -1;
    }

    tim = _tim(dev);
    tim->DIER &= ~(1 << (channel + 1));
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    return (unsigned int)_tim(dev)->CNT;
}

void timer_start(tim_t dev)
{
    _tim(dev)->CR1 |= TIM_CR1_CEN;
}

void timer_stop(tim_t dev)
{
    _tim(dev)->CR1 &= ~(TIM_CR1_CEN);
}

void timer_irq_enable(tim_t dev)
{
    NVIC_EnableIRQ(timer_config[dev].irqn);
}

void timer_irq_disable(tim_t dev)
{
    NVIC_DisableIRQ(timer_config[dev].irqn);
}

void timer_reset(tim_t dev)
{
    _tim(dev)->CNT = 0;
}

static inline void irq_handler(tim_t num, TIM_TypeDef *tim)
{
    for (int i = 0; i < CHANNEL_NUMOF; i++) {
        uint16_t bit = (1 << (i + 1));
        if ((tim->SR & bit) && (tim->DIER & bit)) {
            tim->SR &= ~(bit);
            tim->DIER &= ~(bit);
            isr_ctx[num].cb(i);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef TIMER_0_ISR
void TIMER_0_ISR(void)
{
    irq_handler(0, timer_config[0].dev);
}
#endif

#ifdef TIMER_1_ISR
void TIMER_1_ISR(void)
{
    irq_handler(0, timer_config[0].dev);
}
#endif

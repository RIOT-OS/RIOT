/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 * 
 * @file        timer.c
 * @brief       Low-level timer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * 
 * @}
 */

#include <stdlib.h>

#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_tim.h"

#include "periph/timer.h"
#include "periph_conf.h"

#include "board.h"
#include "cpu.h"


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
    TIM_TimeBaseInitTypeDef tim_init;
    NVIC_InitTypeDef nvic_init;

    // init generic timer options
    tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_init.TIM_CounterMode = TIM_CounterMode_Up;

    // setup the interrupt controller
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    nvic_init.NVIC_IRQChannelSubPriority = 0;

    // set callback routine
    config[dev].cb = callback;

    switch (dev) {
        case TIMER_0:
            // enable clocks
            TIMER_0_CLKEN();
            // timer init
            tim_init.TIM_Period = TIMER_0_MAX_VALUE;
            tim_init.TIM_Prescaler = TIMER_0_PRESCALER * ticks_per_us;
            TIM_TimeBaseInit(TIMER_0_DEV, &tim_init);
            // irq setup
            nvic_init.NVIC_IRQChannel = TIMER_0_IRQCHAN;
            nvic_init.NVIC_IRQChannelPreemptionPriority = TIMER_0_IRQ_PRIO;
            NVIC_Init(&nvic_init);
            // enable timer
            TIM_Cmd(TIMER_0_DEV, ENABLE);
            break;
        case TIMER_1:
            // enable clocks
            TIMER_1_CLKEN();
            // timer init
            tim_init.TIM_Period = TIMER_1_MAX_VALUE;
            tim_init.TIM_Prescaler = TIMER_1_PRESCALER * ticks_per_us;
            TIM_TimeBaseInit(TIMER_1_DEV, &tim_init);
            // irq setup
            nvic_init.NVIC_IRQChannel = TIMER_1_IRQCHAN;
            nvic_init.NVIC_IRQChannelPreemptionPriority = TIMER_1_IRQ_PRIO;
            NVIC_Init(&nvic_init);
            // enable timer
            TIM_Cmd(TIMER_1_DEV, ENABLE);
            break;
        case TIMER_UNDEFINED:
        default:
            return -1;
    }
    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    int now = timer_read(dev);
    TIM_TypeDef *timer = NULL;
    switch (dev) {
        case TIMER_0:
            timer = TIMER_0_DEV;
            break;
        case TIMER_1:
            timer = TIMER_1_DEV;
            break;
        case TIMER_UNDEFINED:
        default:
            return -1;
    }
    switch (channel) {
        case 0:
            TIM_SetCompare1(timer, now + timeout - 1);
            TIM_ITConfig(timer, TIM_IT_CC1, ENABLE);
            break;
        case 1:
            TIM_SetCompare2(timer, now + timeout - 1);
            TIM_ITConfig(timer, TIM_IT_CC2, ENABLE);
            break;
        case 2:
            TIM_SetCompare3(timer, now + timeout - 1);
            TIM_ITConfig(timer, TIM_IT_CC3, ENABLE);
            break;
        case 3:
            TIM_SetCompare4(timer, now + timeout - 1);
            TIM_ITConfig(timer, TIM_IT_CC4, ENABLE);
            break;
        default:
            return -1;
    }
    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    TIM_TypeDef *timer = NULL;
    switch (dev) {
        case TIMER_0:
            timer = TIMER_0_DEV;
            break;
        case TIMER_1:
            timer = TIMER_1_DEV;
            break;
        case TIMER_UNDEFINED:
        default:
            return -1;
    }
    switch (channel) {
        case 0:
            TIM_ITConfig(timer, TIM_IT_CC1, DISABLE);
            break;
        case 1:
            TIM_ITConfig(timer, TIM_IT_CC2, DISABLE);
            break;
        case 2:
            TIM_ITConfig(timer, TIM_IT_CC3, DISABLE);
            break;
        case 3:
            TIM_ITConfig(timer, TIM_IT_CC4, DISABLE);
            break;
        default:
            return -1;
    }
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    int value = -1;
    switch (dev) {
        case TIMER_0:
            value = TIM_GetCounter(TIMER_0_DEV);
            break;
        case TIMER_1:
            value = TIM_GetCounter(TIMER_1_DEV);
            break;
        case TIMER_UNDEFINED:
        default:
            return 0;
    }
    return value;
}

void timer_stop(tim_t dev)
{
    switch (dev) {
        case TIMER_0:
            TIM_Cmd(TIMER_0_DEV, DISABLE);
            break;
        case TIMER_1:
            TIM_Cmd(TIMER_1_DEV, DISABLE);
            break;
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_start(tim_t dev)
{
    switch (dev) {
        case TIMER_0:
            TIM_Cmd(TIMER_0_DEV, ENABLE);
            break;
        case TIMER_1:
            TIM_Cmd(TIMER_1_DEV, ENABLE);
            break;
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_enable(tim_t dev)
{
    switch (dev) {
        case TIMER_0:
            NVIC_EnableIRQ(TIMER_0_IRQCHAN);
            break;
        case TIMER_1:
            NVIC_EnableIRQ(TIMER_1_IRQCHAN);
            break;
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_disable(tim_t dev)
{
    switch (dev) {
        case TIMER_0:
            NVIC_DisableIRQ(TIMER_0_IRQCHAN);
            break;
        case TIMER_1:
            NVIC_DisableIRQ(TIMER_1_IRQCHAN);
            break;
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_reset(tim_t dev)
{
    switch (dev) {
        case TIMER_0:
            TIM_Cmd(TIMER_0_DEV, DISABLE);
            break;
        case TIMER_1:
            TIM_Cmd(TIMER_1_DEV, DISABLE);
            break;
        case TIMER_UNDEFINED:
            break;
    }
}


__attribute__ ((naked))
void TIMER_0_ISR(void)
{
    ISR_ENTER();
    irq_handler(TIMER_0, TIMER_0_DEV);
    ISR_EXIT();
}

__attribute__ ((naked))
void TIMER_1_ISR(void)
{
    ISR_ENTER();
    irq_handler(TIMER_1, TIMER_1_DEV);
    ISR_EXIT();
}

static inline void irq_handler(tim_t timer, TIM_TypeDef *dev)
{
    if (TIM_GetITStatus(dev, TIM_IT_CC1) == SET) {
        TIM_ClearITPendingBit(dev, TIM_IT_CC1);
        TIM_ITConfig(dev, TIM_IT_CC1, DISABLE);
        config[timer].cb(0);
    } else if (TIM_GetITStatus(dev, TIM_IT_CC2) == SET) {
        TIM_ClearITPendingBit(dev, TIM_IT_CC2);
        TIM_ITConfig(dev, TIM_IT_CC2, DISABLE);
        config[timer].cb(1);
    } else if (TIM_GetITStatus(dev, TIM_IT_CC3) == SET) {
        TIM_ClearITPendingBit(dev, TIM_IT_CC3);
        TIM_ITConfig(dev, TIM_IT_CC3, DISABLE);
        config[timer].cb(2);
    } else if (TIM_GetITStatus(dev, TIM_IT_CC4) == SET) {
        TIM_ClearITPendingBit(dev, TIM_IT_CC4);
        TIM_ITConfig(dev, TIM_IT_CC4, DISABLE);
        config[timer].cb(3);
    }
}

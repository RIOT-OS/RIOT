/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file        timer.c
 * @brief       Low-level timer driver implementation
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"
#include "board.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"

typedef struct {
    void (*cb)(int);
} timer_conf_t;

/**
 * timer state memory
 */
static timer_conf_t timer_config[TIMER_NUMOF];

int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    NRF_TIMER_Type *timer;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            timer = TIMER_0_DEV;
            timer->POWER = 1;
            timer->BITMODE = TIMER_0_BITMODE;
            NVIC_SetPriority(TIMER_0_IRQ, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(TIMER_0_IRQ);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            timer = TIMER_1_DEV;
            timer->POWER = 1;
            timer->BITMODE = TIEMR_1_BITMODE;
            NVIC_SetPriority(TIMER_1_IRQ, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(TIMER_1_IRQ);
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            timer = TIMER_2_DEV;
            timer->POWER = 1;
            timer->BITMODE = TIMER_2_BITMODE;
            NVIC_SetPriority(TIMER_2_IRQ, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(TIMER_2_IRQ);
            break;
#endif
    case TIMER_UNDEFINED:
        return -1;
    }

    /* save callback */
    timer_config[dev].cb = callback;

    timer->TASKS_STOP = 1;

    timer->MODE = TIMER_MODE_MODE_Timer;        /* set the timer in Timer Mode. */
    timer->TASKS_CLEAR    = 1;                  /* clear the task first to be usable for later. */

    switch (ticks_per_us) {
        case 1:
            timer->PRESCALER = 4;
            break;
        case 2:
            timer->PRESCALER = 5;
            break;
        case 4:
            timer->PRESCALER = 6;
            break;
        case 8:
            timer->PRESCALER = 7;
            break;
        case 16:
            timer->PRESCALER = 8;
            break;
        default:
            return -1;
    }

    /* clear all compare channels */
    timer->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
    timer->SHORTS = (TIMER_SHORTS_COMPARE1_CLEAR_Enabled << TIMER_SHORTS_COMPARE1_CLEAR_Pos);
    timer->SHORTS = (TIMER_SHORTS_COMPARE2_CLEAR_Enabled << TIMER_SHORTS_COMPARE2_CLEAR_Pos);
    timer->SHORTS = (TIMER_SHORTS_COMPARE3_CLEAR_Enabled << TIMER_SHORTS_COMPARE3_CLEAR_Pos);

    /* start the timer */
    timer->TASKS_START = 1;

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    uint32_t now = timer_read(dev);
    return timer_set_absolute(dev, channel, (now + timeout - 1));
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    volatile NRF_TIMER_Type * timer;

    /* get timer base register address */
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
#if TIMER_2_EN
        case TIMER_2:
            timer = TIMER_2_DEV;
            break;
#endif
    case TIMER_UNDEFINED:
        return -1;
    }

    switch (channel) {
        case 0:
            timer->CC[0] = value;
            timer->INTENSET |= TIMER_INTENSET_COMPARE0_Msk;
            break;
        case 1:
            timer->CC[1] = value;
            timer->INTENSET |= TIMER_INTENSET_COMPARE1_Msk;
            break;
        case 2:
            timer->CC[2] = value;
            timer->INTENSET |= TIMER_INTENSET_COMPARE2_Msk;
            break;
        default:
            return -2;
    }

    return 1;
}

int timer_clear(tim_t dev, int channel)
{
    NRF_TIMER_Type *timer;

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
#if TIMER_2_EN
        case TIMER_2:
            timer = TIMER_2_DEV;
            break;
#endif
        case TIMER_UNDEFINED:
            return -1;
    }

    /* set timeout value */
    switch (channel) {
        case 0:
            timer->INTENCLR = TIMER_INTENCLR_COMPARE0_Msk;
            break;
        case 1:
            timer->INTENCLR = TIMER_INTENCLR_COMPARE1_Msk;
            break;
        case 2:
            timer->INTENCLR = TIMER_INTENCLR_COMPARE2_Msk;
            break;
        default:
            return -2;
    }

    return 1;
}

unsigned int timer_read(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->TASKS_CAPTURE[3] = 1;
            return TIMER_0_DEV->CC[3];
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->TASKS_CAPTURE[3] = 1;
            return TIMER_1_DEV->CC[3];
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->TASKS_CAPTURE[3] = 1;
            return TIMER_2_DEV->CC[3];
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
            TIMER_0_DEV->TASKS_START = 1;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->TASKS_START = 1;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->TASKS_START = 1;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_stop(tim_t dev) {
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->TASKS_STOP = 1;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->TASKS_STOP = 1;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->TASKS_STOP = 1;
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
            NVIC_EnableIRQ(TIMER_0_IRQ);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_EnableIRQ(TIMER_1_IRQ);
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            NVIC_EnableIRQ(TIMER_2_IRQ);
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
            NVIC_DisableIRQ(TIMER_0_IRQ);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_DisableIRQ(TIMER_1_IRQ);
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            NVIC_DisableIRQ(TIMER_2_IRQ);
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
            TIMER_0_DEV->TASKS_CLEAR = 1;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->TASKS_CLEAR = 1;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->TASKS_CLEAR = 1;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

#if TIMER_0_EN
void TIMER_0_ISR(void)
{
    for(int i = 0; i < TIMER_0_CHANNELS; i++){
        if(TIMER_0_DEV->EVENTS_COMPARE[i] == 1){
            TIMER_0_DEV->EVENTS_COMPARE[i] = 0;
            TIMER_0_DEV->INTENCLR = (1 << (16 + i));
            timer_config[TIMER_0].cb(i);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#if TIMER_1_EN
void TIMER_1_ISR(void)
{
    for(int i = 0; i < TIMER_1_CHANNELS; i++){
        if(TIMER_1_DEV->EVENTS_COMPARE[i] == 1){
            TIMER_1_DEV->EVENTS_COMPARE[i] = 0;
            TIMER_1_DEV->INTENCLR = (1 << (16 + i));
            timer_config[TIMER_1].cb(i);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#if TIMER_2_EN
void TIMER_2_ISR(void)
{
    for(int i = 0; i < TIMER_2_CHANNELS; i++){
        if(TIMER_2_DEV->EVENTS_COMPARE[i] == 1){
            TIMER_2_DEV->EVENTS_COMPARE[i] = 0;
            TIMER_2_DEV->INTENCLR = (1 << (16 + i));
            timer_config[TIMER_2].cb(i);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

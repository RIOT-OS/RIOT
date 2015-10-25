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
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"

/**
 * @name Flags to mark active channels
 * @{
 */
#define TIMER_CH0           0x01
#define TIMER_CH1           0x02
#define TIMER_CH2           0x04
/** @} */

/**
 * @brief struct for keeping track of a timer's state
 */
typedef struct {
    void (*cb)(int);
    uint8_t flags;
} timer_conf_t;

/**
 * @brief timer state memory
 */
static timer_conf_t timer_config[TIMER_NUMOF];

/**
 * @brief static timer mapping
 */
static NRF_TIMER_Type *const timer[] = {
#if TIMER_0_EN
    TIMER_0_DEV,
#endif
#if TIMER_1_EN
    TIMER_1_DEV,
#endif
#if TIMER_2_EN
    TIMER_2_DEV
#endif
};

int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    if (dev >= TIMER_NUMOF) {
        return -1;
    }

    /* save callback */
    timer_config[dev].cb = callback;
    /* power on timer */
    timer[dev]->POWER = 1;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->BITMODE = TIMER_0_BITMODE;
            NVIC_SetPriority(TIMER_0_IRQ, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(TIMER_0_IRQ);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->BITMODE = TIMER_1_BITMODE;
            NVIC_SetPriority(TIMER_1_IRQ, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(TIMER_1_IRQ);
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->BITMODE = TIMER_2_BITMODE;
            NVIC_SetPriority(TIMER_2_IRQ, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(TIMER_2_IRQ);
            break;
#endif
    case TIMER_UNDEFINED:
        return -1;
    }

    timer[dev]->TASKS_STOP = 1;
    timer[dev]->MODE = TIMER_MODE_MODE_Timer;        /* set the timer in Timer Mode. */
    timer[dev]->TASKS_CLEAR    = 1;                  /* clear the task first to be usable for later. */

    switch (ticks_per_us) {
        case 1:
            timer[dev]->PRESCALER = 4;
            break;
        case 2:
            timer[dev]->PRESCALER = 5;
            break;
        case 4:
            timer[dev]->PRESCALER = 6;
            break;
        case 8:
            timer[dev]->PRESCALER = 7;
            break;
        case 16:
            timer[dev]->PRESCALER = 8;
            break;
        default:
            return -1;
    }

    /* reset compare state */
    timer[dev]->EVENTS_COMPARE[0] = 0;
    timer[dev]->EVENTS_COMPARE[1] = 0;
    timer[dev]->EVENTS_COMPARE[2] = 0;

    /* start the timer */
    timer[dev]->TASKS_START = 1;
    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    uint32_t now = timer_read(dev);
    return timer_set_absolute(dev, channel, (now + timeout - 1));
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (dev >= TIMER_NUMOF) {
        return -1;
    }

    switch (channel) {
        case 0:
            timer[dev]->CC[0] = value;
            timer_config[dev].flags |= TIMER_CH0;
            timer[dev]->INTENSET |= TIMER_INTENSET_COMPARE0_Msk;
            break;
        case 1:
            timer[dev]->CC[1] = value;
            timer_config[dev].flags |= TIMER_CH1;
            timer[dev]->INTENSET |= TIMER_INTENSET_COMPARE1_Msk;
            break;
        case 2:
            timer[dev]->CC[2] = value;
            timer_config[dev].flags |= TIMER_CH2;
            timer[dev]->INTENSET |= TIMER_INTENSET_COMPARE2_Msk;
            break;
        default:
            return -2;
    }

    return 1;
}

int timer_clear(tim_t dev, int channel)
{
    if (dev >= TIMER_NUMOF) {
        return -1;
    }

    /* set timeout value */
    switch (channel) {
        case 0:
            timer_config[dev].flags &= ~TIMER_CH0;
            timer[dev]->INTENCLR = TIMER_INTENCLR_COMPARE0_Msk;
            break;
        case 1:
            timer_config[dev].flags &= ~TIMER_CH1;
            timer[dev]->INTENCLR = TIMER_INTENCLR_COMPARE1_Msk;
            break;
        case 2:
            timer_config[dev].flags &= ~TIMER_CH2;
            timer[dev]->INTENCLR = TIMER_INTENCLR_COMPARE2_Msk;
            break;
        default:
            return -2;
    }

    return 1;
}

unsigned int timer_read(tim_t dev)
{
    if (dev >= TIMER_NUMOF) {
        return 0;
    }

    timer[dev]->TASKS_CAPTURE[3] = 1;
    return timer[dev]->CC[3];
}

void timer_start(tim_t dev)
{
    if (dev < TIMER_NUMOF) {
        timer[dev]->TASKS_START = 1;
    }
}

void timer_stop(tim_t dev)
{
    if (dev < TIMER_NUMOF) {
        timer[dev]->TASKS_STOP = 1;
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
    if (dev < TIMER_NUMOF) {
        timer[dev]->TASKS_CLEAR = 1;
    }
}

#if TIMER_0_EN
void TIMER_0_ISR(void)
{
    for(int i = 0; i < TIMER_0_CHANNELS; i++){
        if(TIMER_0_DEV->EVENTS_COMPARE[i] == 1){
            TIMER_0_DEV->EVENTS_COMPARE[i] = 0;
            if (timer_config[TIMER_0].flags & (1 << i)) {
                timer_config[TIMER_0].flags &= ~(1 << i);
                TIMER_0_DEV->INTENCLR = (1 << (16 + i));
                timer_config[TIMER_0].cb(i);
            }
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
            if (timer_config[TIMER_1].flags & (1 << i)) {
                timer_config[TIMER_1].flags &= ~(1 << i);
                TIMER_1_DEV->INTENCLR = (1 << (16 + i));
                timer_config[TIMER_1].cb(i);
            }
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
            if (timer_config[TIMER_2].flags & (1 << i)) {
                timer_config[TIMER_2].flags &= ~(1 << i);
                TIMER_2_DEV->INTENCLR = (1 << (16 + i));
                timer_config[TIMER_2].cb(i);
            }
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

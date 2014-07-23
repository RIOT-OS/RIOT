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
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"
#include "board.h"
#include "periph_conf.h"
#include "periph/timer.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"

typedef struct {
    void (*cb)(int);
} timer_conf_t;

/**
 * Timer state memory
 */

/*
 * page 90 in nrf51822.pdf
 * TimerFrequency = fTimer = HFCLK / (2 ^ Prescaler)
 * nrf51822 has 4 Timer Capture/Compare Registers
 * and 3 Hardware Timers
 */
timer_conf_t config[TIMER_NUMOF];

int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{

    NRF_TIMER_Type *timer;

    switch (dev) {

#if TIMER_0_EN
    case (TIMER_0):
        timer = TIMER_0_DEV;
        timer->BITMODE = TIMER_BITMODE_BITMODE_32Bit;    //32 Bit Mode
        NVIC_SetPriority(TIMER_0_IRQ, TIMER_IRQ_PRIO);
        NVIC_EnableIRQ(TIMER_0_IRQ);
        break;
#endif

#if TIMER_1_EN
    case (TIMER_1):
        timer = TIMER_1_DEV;
        timer->BITMODE = TIMER_BITMODE_BITMODE_16Bit;    //16 Bit Mode
        NVIC_SetPriority(TIMER_1_IRQ, TIMER_IRQ_PRIO);
        NVIC_EnableIRQ(TIMER_1_IRQ);
        break;
#endif

#if TIMER_2_EN
    case (TIMER_2):
        timer = NRF_TIMER2;
        timer->BITMODE = TIMER_BITMODE_BITMODE_16Bit;    //16 Bit Mode
        NVIC_SetPriority(TIMER2_IRQn, 1);
        NVIC_EnableIRQ(TIMER2_IRQn);
        break;
#endif
    case TIMER_UNDEFINED:
        break;


    }

    /* save callback */
    config[dev].cb = callback;

    timer->TASKS_STOP = 1;

    timer->MODE    = TIMER_MODE_MODE_Timer;       // Set the timer in Timer Mode.
    timer->TASKS_CLEAR    = 1;                    // clear the task first to be usable for later.

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
            timer->PRESCALER = 1;
            break;
    }
    //timer->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
    timer->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
    timer->SHORTS = (TIMER_SHORTS_COMPARE1_CLEAR_Enabled << TIMER_SHORTS_COMPARE1_CLEAR_Pos);
    timer->SHORTS = (TIMER_SHORTS_COMPARE2_CLEAR_Enabled << TIMER_SHORTS_COMPARE2_CLEAR_Pos);
    timer->SHORTS = (TIMER_SHORTS_COMPARE3_CLEAR_Enabled << TIMER_SHORTS_COMPARE3_CLEAR_Pos);
    timer->TASKS_START = 1;

    return 1;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    volatile NRF_TIMER_Type * timer;

    /* get timer base register address */
    switch (dev) {

#if TIMER_0_EN
    case (TIMER_0):
        timer = TIMER_0_DEV;
        break;
#endif

#if TIMER_1_EN
    case (TIMER_1):
        timer = TIMER_1_DEV;
        break;
#endif

#if TIMER_2_EN
    case (TIMER_2):
        timer = TIMER_2_DEV;
        break;
#endif
    case TIMER_UNDEFINED:
        break;
    }

    /* set timeout value */
    uint32_t offset = timer_read(dev);

    switch (channel) {
        case 0:
            timer->CC[0] = (uint32_t) offset+timeout;
            timer->INTENSET |= TIMER_INTENSET_COMPARE0_Msk;
            break;
        case 1:
            timer->CC[1] = (uint32_t) offset+timeout;
            timer->INTENSET |= TIMER_INTENSET_COMPARE1_Msk;
            break;
        case 2:
            timer->CC[2] = (uint32_t) offset+timeout;
            timer->INTENSET |= TIMER_INTENSET_COMPARE2_Msk;
            break;
        case 3:
        default:
            return 0;
    }
    return 1;
}

int timer_clear(tim_t dev, int channel)
{
    volatile NRF_TIMER_Type * timer;

    /* get timer base register address */
    switch (dev) {

#if TIMER_0_EN
    case (TIMER_0):
        timer = TIMER_0_DEV;
        break;
#endif

#if TIMER_1_EN
    case (TIMER_1):
        timer = TIMER_1_DEV;
        break;
#endif

#if TIMER_2_EN
    case (TIMER_2):
        timer = TIMER_2_DEV;
        break;
#endif
    case TIMER_UNDEFINED:
        break;

    }

    timer->TASKS_CLEAR = 1;

    /* set timeout value */
    switch (channel) {
        case 0:
            timer->CC[0] = 0;

            break;
        case 1:
            timer->CC[1] = 0;

            break;
        case 2:
            timer->CC[2] = 0;

            break;
        case 3:
            timer->CC[3] = 0;

            break;
        default:
            break;
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
            TIMER_0_DEV->TASKS_SHUTDOWN = 1;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->TASKS_STOP = 1;
            TIMER_1_DEV->TASKS_SHUTDOWN = 1;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->TASKS_STOP = 1;
            TIMER_2_DEV->TASKS_SHUTDOWN = 1;
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
            TIMER_0_DEV->INTENSET = 1;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->INTENSET = 1;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->INTENSET = 1;
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
            TIMER_0_DEV->INTENCLR = 1;
            NVIC_DisableIRQ(TIMER0_IRQn);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->INTENCLR = 1;
            NVIC_DisableIRQ(TIMER1_IRQn);
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->INTENCLR = 1;
            NVIC_DisableIRQ(TIMER2_IRQn);
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_reset(tim_t dev)
{
    /* TODO */
}

#if TIMER_0_EN
__attribute__((naked)) void TIMER_0_ISR(void)
{
    ISR_ENTER();
    for(int i = 0; i < TIMER_0_CHANNELS; i++){
        if(TIMER_0_DEV->EVENTS_COMPARE[i] == 1){

//            TIMER_0_DEV->CC[i] = 0;
            TIMER_0_DEV->EVENTS_COMPARE[i] = 0;
            TIMER_0_DEV->INTENCLR = (1 << (16 + i));

            config[TIMER_0].cb(i);
        }
    }
    ISR_EXIT();
}
#endif

#if TIMER_1_EN
__attribute__((naked)) void TIMER_1_ISR(void)
{
    ISR_ENTER();
    for(int i = 0; i < TIMER_1_CHANNELS; i++){
        if(TIMER_1_DEV->EVENTS_COMPARE[i] == 1){
//            NRF_TIMER1->CC[i] = 0;
            TIMER_1_DEV->EVENTS_COMPARE[i] = 0;
            TIMER_1_DEV->INTENCLR = (1 << (16 + i));
            config[TIMER_1].cb(i);
        }
    }
    ISR_EXIT();
}
#endif

#if TIMER_2_EN
__attribute__((naked)) void TIMER_2_ISR(void)
{
    ISR_ENTER();
    for(int i = 0; i < TIMER_2_CHANNELS; i++){
        if(TIMER_2_DEV->EVENTS_COMPARE[i] == 1){
            TIMER_2_DEV->CC[i] = 0;
            TIMER_2_DEV->EVENTS_COMPARE[i] = 0;
            config[TIMER_2].cb(i);
        }
    }
    ISR_EXIT();
}
#endif

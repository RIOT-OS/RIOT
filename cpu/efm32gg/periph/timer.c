/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_efm32
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

//#include "cpu.h"
//#include "board.h"
//#include "periph_conf.h"
#include "periph/timer.h"

#include "em_timer.h"

static inline void irq_handler(tim_t dev, TIMER_TypeDef *timer);

typedef struct {
    void (*cb)(int);
} timer_conf_t;

/**
 * Timer state memory
 */
timer_conf_t config[TIMER_COUNT];

int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    TIMER_TypeDef *timer;
    TIMER_Init_TypeDef timer_init = {
        .enable     = true,
        .debugRun   = false,
        .prescale   = timerPrescale1,
        .clkSel     = timerClkSelHFPerClk,
        .count2x    = false,
        .ati        = false,
        .fallAction = timerInputActionNone,
        .riseAction = timerInputActionNone,
        .mode       = timerModeUp,
        .dmaClrAct  = false,
        .quadModeX4 = 4,
        .oneShot    = false,
        .sync       = false
    };

    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        //Do timer0 specific init
        timer = TIMER0;
        timer_init.prescale = TIMER_0_PRESCALER;
        break;
#endif
#ifdef TIMER_1_EN
    case TIMER_1:
        //Do timer1 specific init
        timer = TIMER1;
        timer_init.prescale = TIMER_1_PRESCALER;
        break;
#endif
    default:
        return -1;
    }

    // Set timeout value
    TIMER_TopSet(timer, TIMER_0_MAX_VALUE);

    // Initialise timer
    TIMER_Init(timer, &timer_init);

    config[dev].cb = callback;

    /* enable the timer's interrupt */
    timer_irq_enable(dev);

    /* start the timer */
    timer_start(dev);

    return 0;
}

//Set an interrupt on the specified channel n ticks from now
int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    int now = timer_read(dev);
    return timer_set_absolute(dev, channel, now + timeout - 1);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    TIMER_TypeDef *timer;
    TIMER_InitCC_TypeDef timerN_ccM_init = {
        .eventCtrl  = timerEventRising,
        .edge       = timerEdgeRising,
        .prsSel     = timerPRSSELCh0,
        .cufoa      = timerOutputActionNone,
        .cofoa      = timerOutputActionNone,
        .cmoa       = timerOutputActionToggle,
        .mode       = timerCCModeCompare,
        .filter     = false,
        .prsInput   = false,
        .coist      = false,
        .outInvert  = false,
    };

    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        timer = TIMER0;
        break;
#endif
#ifdef TIMER_1_EN
    case TIMER_1:
        timer = TIMER1;
        break;
#endif
    default:
        return -1;
    }

    // Check channel no is valid
    if (TIMER_CH_VALID(channel)) {

        //Set up compare channel
        TIMER_CompareSet(timer, channel, value);

        //Set the compare interrupt
        switch (channel) {
        case 0:
            timer->IEN |= TIMER_IEN_CC0;
            break;
        case 1:
            timer->IEN |= TIMER_IEN_CC1;
            break;
        case 2:
            timer->IEN |= TIMER_IEN_CC2;
            break;
        }

        //Start the capture compare register
        TIMER_InitCC(timer, channel, &timerN_ccM_init);

        return 0;

    } else {
        return -2;
    }
}

// Clear the event from a timer channel
int timer_clear(tim_t dev, int channel)
{
    TIMER_TypeDef *timer;

    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        timer = TIMER0;
        break;
#endif
#ifdef TIMER_1_EN
    case TIMER_1:
        timer = TIMER1;
        break;
#endif
    default:
        return -1;
    }

    //TODO: Enable timer channel event clearing
    //TODO: Clear interrupt flags following deactivation

    switch (channel) {
    case 0:
        timer->IEN &= ~TIMER_IEN_CC0;
        timer->IFC |= TIMER_IFC_CC0;
        break;
    case 1:
        timer->IEN &= ~TIMER_IEN_CC1;
        timer->IFC |= TIMER_IFC_CC1;
        break;
    case 2:
        timer->IEN &= ~TIMER_IEN_CC2;
        timer->IFC |= TIMER_IFC_CC2;
        break;
    default:
        return -1;
    }
    return 0;
}

//Fetch timer count in ticks
unsigned int timer_read(tim_t dev)
{
    TIMER_TypeDef *timer;

    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        timer = TIMER0;
        break;
#endif
#ifdef TIMER_1_EN
    case TIMER_1:
        timer = TIMER1;
        break;
#endif
    default:
        return -1;
    }

    //Fetch timer count
    return timer->CNT;
}

//Start the timer
void timer_start(tim_t dev)
{
    TIMER_TypeDef *timer;

    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        timer = TIMER0;
        break;
#endif
#ifdef TIMER_1_EN
    case TIMER_1:
        timer = TIMER1;
        break;
#endif
    default:
        return;
    }

    // Start timer
    timer->CMD = TIMER_CMD_START;
}


//Stop the timer
void timer_stop(tim_t dev)
{
    TIMER_TypeDef *timer;
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        timer = TIMER0;
        break;
#endif
#ifdef TIMER_1_EN
    case TIMER_1:
        timer = TIMER1;
        break;
#endif
    default:
        return;
    }

    // Stop timer
    timer->CMD = TIMER_CMD_STOP;
}

//Enable timer interrupts
void timer_irq_enable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        NVIC_EnableIRQ(TIMER0_IRQn);
        break;
#endif
#ifdef TIMER_1_EN
    case TIMER_1:
        NVIC_EnableIRQ(TIMER1_IRQn);
        break;
#endif
    default:
        return;
    }
}

//Disable timer interrupts
void timer_irq_disable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        NVIC_DisableIRQ(TIMER0_IRQn);
        break;
#endif
#ifdef TIMER_1_EN
    case TIMER_1:
        NVIC_DisableIRQ(TIMER1_IRQn);
        break;
#endif
    default:
        return;
    }
}

//Reset the timer
void timer_reset(tim_t dev)
{
    TIMER_TypeDef *timer;
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        timer = TIMER0;
        break;
#endif
#ifdef TIMER_1_EN
    case TIMER_1:
        timer = TIMER1;
        break;
#endif
    default:
        return;
    }

    //Reset timer
    timer->CNT = 0;
}

#ifdef TIMER_0_EN
__attribute__ ((naked)) void TIMER_0_ISR(void)
{
    ISR_ENTER();
    irq_handler(TIMER_0, TIMER_0_DEV);
    ISR_EXIT();
}
#endif

#ifdef TIMER_1_EN
__attribute__ ((naked)) void TIMER_1_ISR(void)
{
    ISR_ENTER();
    irq_handler(TIMER_1, TIMER_1_DEV);
    ISR_EXIT();
}
#endif

static inline void irq_handler(tim_t dev, TIMER_TypeDef *timer)
{
    //Check each channel
    if (timer->IF & TIMER_IF_CC0) {
        //Disable interrupt
        timer->IEN &= ~TIMER_IEN_CC0;
        //Clear the flag
        timer->IFC |= TIMER_IFC_CC0;
        //Call bound callback function
        config[dev].cb(0);
    } else if (timer->IF & TIMER_IF_CC1) {
        //Disable interrupt
        timer->IEN &= ~TIMER_IEN_CC1;
        //Clear the flag
        timer->IFC |= TIMER_IFC_CC1;
        //Call bound callback function
        config[dev].cb(1);
    } else if (timer->IF & TIMER_IF_CC2) {
        //Disable interrupt
        timer->IEN &= ~TIMER_IEN_CC2;
        //Clear the flag
        timer->IFC |= TIMER_IFC_CC2;
        //Call bound callback function
        config[dev].cb(2);
    }
}

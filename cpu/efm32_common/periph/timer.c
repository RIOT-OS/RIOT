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
 * @author      Ryan Kurte
 *
 * @}
 */

#include <stdlib.h>

#include "cpu.h"
#include "board.h"
#include "periph_conf.h"
#include "thread.h"
#include "periph/timer.h"

#include "em_timer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static inline void timer_irq_handler(tim_t dev, TIMER_TypeDef *timer);

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
        .fallAction = timerInputActionNone,
        .riseAction = timerInputActionNone,
        .mode       = timerModeUp,
        .dmaClrAct  = false,
        .quadModeX4 = 4,
        .oneShot    = false,
        .sync       = false,
#if 0
        .count2x    = false,
        .ati        = false
#endif
    };

    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        //Do timer0 specific init
        timer = TIMER0;
        timer_init.prescale = TIMER_0_PRESCALER;
        TIMER_0_CLKEN();
        break;
#endif
#ifdef TIMER_1_EN
    case TIMER_1:
        //Do timer1 specific init
        timer = TIMER1;
        timer_init.prescale = TIMER_1_PRESCALER;
        TIMER_1_CLKEN();
        break;
#endif
    default:
        return -1;
    }

    DEBUG("timer_init(): timer %d\n", dev);

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
    DEBUG("timer_set(): timer %d channel %d timeout %u\n", dev, channel, timeout);
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
        DEBUG("timer_set_absolute(): timer %d channel %d time %u\n", dev, channel, value);

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
        DEBUG("timer_set_absolute(): invalid channel\n");
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

    DEBUG("timer_clear(): timer %d channel %d\n", dev, channel);

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
    unsigned int timer_val;

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
    timer_val = timer->CNT;
    DEBUG("timer_read(): value %d\n", timer_val);

    return timer_val;
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

    DEBUG("timer_start(): timer %d\n", dev);

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

    DEBUG("timer_stop(): timer %d\n", dev);

    // Stop timer
    timer->CMD = TIMER_CMD_STOP;
}

//Enable timer interrupts
void timer_irq_enable(tim_t dev)
{
    DEBUG("timer_irq_enable(): timer %d\n", dev);

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
    DEBUG("timer_irq_disable(): timer %d\n", dev);

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
    DEBUG("timer_reset(): timer %d\n", dev);

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
    timer_irq_handler(TIMER_0, TIMER_0_DEV);
    //if (sched_context_switch_request) {
    //    thread_yield();
    //}
    ISR_EXIT();
}
#endif

#ifdef TIMER_1_EN
__attribute__ ((naked)) void TIMER_1_ISR(void)
{
    ISR_ENTER();
    timer_irq_handler(TIMER_1, TIMER_1_DEV);
    //if (sched_context_switch_request) {
    //    thread_yield();
    //}
    ISR_EXIT();
}
#endif

static inline void timer_irq_handler(tim_t dev, TIMER_TypeDef *timer)
{
    DEBUG("timer irq_handler(): timer %d\n", dev);

    //Check each channel
    if (timer->IF & TIMER_IF_CC0) {
        //Disable interrupt
        timer->IEN &= ~TIMER_IEN_CC0;
        //Clear the flag
        timer->IFC |= TIMER_IFC_CC0;
        //Call bound callback function
        if (config[dev].cb != NULL) {
            config[dev].cb(0);
        } else {
            printf("timer irq not bound\n");
        }

    } else if (timer->IF & TIMER_IF_CC1) {
        //Disable interrupt
        timer->IEN &= ~TIMER_IEN_CC1;
        //Clear the flag
        timer->IFC |= TIMER_IFC_CC1;
        //Call bound callback function
        if (config[dev].cb != NULL) {
            config[dev].cb(1);
        } else {
            printf("timer irq not bound\n");
        }

    } else if (timer->IF & TIMER_IF_CC2) {
        //Disable interrupt
        timer->IEN &= ~TIMER_IEN_CC2;
        //Clear the flag
        timer->IFC |= TIMER_IFC_CC2;
        //Call bound callback function
        if (config[dev].cb != NULL) {
            config[dev].cb(2);
        } else {
            printf("timer irq not bound\n");
        }

    }
}

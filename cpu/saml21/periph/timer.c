/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *               2014 Freie Universität Berlin
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        timer.c
 * @brief       Low-level timer driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "cpu.h"

#include "periph/timer.h"
#include "periph_conf.h"

#include "sched.h"
#include "thread.h"
#define ENABLE_DEBUG    (0)
#include "debug.h"

typedef struct {
    void (*cb)(int);
} timer_conf_t;

/**
 * @brief Timer state memory
 */
timer_conf_t config[TIMER_NUMOF];

/**
 * @brief Setup the given timer
 */
int timer_init(tim_t dev, unsigned int us_per_ticks, void (*callback)(int))
{
    /* configure GCLK0 to feed TC0 & TC1*/;
    GCLK->PCHCTRL[TC0_GCLK_ID].reg |= GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;
    while (!(GCLK->PCHCTRL[TC0_GCLK_ID].reg & GCLK_PCHCTRL_CHEN));

    /* select the timer and enable the timer specific peripheral clocks */
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        if (TIMER_0_DEV.CTRLA.bit.ENABLE) {
            return 0;
        }
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC0;
        /* reset timer */
        TIMER_0_DEV.CTRLA.bit.SWRST = 1;
        while (TIMER_0_DEV.SYNCBUSY.bit.SWRST);
        /* choosing 32 bit mode */
        TIMER_0_DEV.CTRLA.bit.MODE = TC_CTRLA_MODE_COUNT32_Val;
        /* sourced by 4MHz with Presc 4 results in 1MHz*/
        TIMER_0_DEV.CTRLA.bit.PRESCALER = TC_CTRLA_PRESCALER_DIV16_Val;
        /* initial prescaler resync */
        TIMER_0_DEV.CTRLA.bit.PRESCSYNC = TC_CTRLA_PRESCSYNC_RESYNC_Val;
        break;
#endif
    case TIMER_UNDEFINED:
    default:
        return -1;
    }

    /* save callback */
    config[dev].cb = callback;

    /* enable interrupts for given timer */
    timer_irq_enable(dev);

    timer_start(dev);

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    return timer_set_absolute(dev, channel, timer_read(dev) + timeout);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    DEBUG("Setting timer %i channel %i to %i\n", dev, channel, value);

    /* get timer base register address */
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        /* set timeout value */
        switch (channel) {
        case 0:
            TIMER_0_DEV.INTFLAG.bit.MC0 = 1;
            TIMER_0_DEV.CC[0].reg = value;
            TIMER_0_DEV.INTENSET.bit.MC0 = 1;
            break;
        case 1:
            TIMER_0_DEV.INTFLAG.bit.MC1 = 1;
            TIMER_0_DEV.CC[1].reg = value;
            TIMER_0_DEV.INTENSET.bit.MC1 = 1;
            break;
        default:
            return -1;
        }
        break;
#endif
    case TIMER_UNDEFINED:
    default:
        return -1;
    }

    return 1;
}

int timer_clear(tim_t dev, int channel)
{
    /* get timer base register address */
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        switch (channel) {
        case 0:
            TIMER_0_DEV.INTFLAG.bit.MC0 = 1;
            TIMER_0_DEV.INTENCLR.bit.MC0 = 1;
            break;
        case 1:
            TIMER_0_DEV.INTFLAG.bit.MC1 = 1;
            TIMER_0_DEV.INTENCLR.bit.MC1 = 1;
            break;
        default:
            return -1;
        }
        break;
#endif
    case TIMER_UNDEFINED:
    default:
        return -1;
    }

    return 1;
}

unsigned int timer_read(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        /* request syncronisation */
        TIMER_0_DEV.CTRLBSET.bit.CMD = TC_CTRLBSET_CMD_READSYNC_Val;
        while (TIMER_0_DEV.SYNCBUSY.bit.STATUS);
        return TIMER_0_DEV.COUNT.reg;
        break;
#endif
    default:
        return 0;
    }


}

void timer_stop(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV.CTRLA.bit.ENABLE = 0;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_start(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV.CTRLA.bit.ENABLE = 1;
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
            NVIC_EnableIRQ(TC0_IRQn);
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
            NVIC_DisableIRQ(TC0_IRQn);
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
            TIMER_0_DEV.CTRLA.bit.SWRST = 1;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

#if TIMER_0_EN
void TIMER_0_ISR(void)
{
    if (TIMER_0_DEV.INTFLAG.bit.MC0 && TIMER_0_DEV.INTENSET.bit.MC0) {
        if(config[TIMER_0].cb) {
            TIMER_0_DEV.INTFLAG.bit.MC0 = 1;
            TIMER_0_DEV.INTENCLR.reg = TC_INTENCLR_MC0;
            config[TIMER_0].cb(0);
        }
    }
    else if (TIMER_0_DEV.INTFLAG.bit.MC1 && TIMER_0_DEV.INTENSET.bit.MC1) {
        if(config[TIMER_0].cb) {
            TIMER_0_DEV.INTFLAG.bit.MC1 = 1;
            TIMER_0_DEV.INTENCLR.reg = TC_INTENCLR_MC1;
            config[TIMER_0].cb(1);
        }
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /* TIMER_0_EN */

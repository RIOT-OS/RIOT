/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
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

/**
 * @brief Timer state memory
 */
static timer_isr_ctx_t config[TIMER_NUMOF];


/**
 * @brief Setup the given timer
 */
int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
    /* at the moment, the timer can only run at 1MHz */
    if (freq != 1000000ul) {
        return -1;
    }

/* select the clock generator depending on the main clock source:
 * GCLK0 (1MHz) if we use the internal 8MHz oscillator
 * GCLK1 (8MHz) if we use the PLL */
#if CLOCK_USE_PLL
    /* configure GCLK1 (configured to 1MHz) to feed TC3, TC4 and TC5 */;
    GCLK->CLKCTRL.reg = (uint16_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK1 | (TC3_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
    while (GCLK->STATUS.bit.SYNCBUSY) {}
    /* TC4 and TC5 share the same channel */
    GCLK->CLKCTRL.reg = (uint16_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK1 | (TC4_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
#else
    /* configure GCLK0 to feed TC3, TC4 and TC5 */;
    GCLK->CLKCTRL.reg = (uint16_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | (TC3_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
    /* TC4 and TC5 share the same channel */
    GCLK->CLKCTRL.reg = (uint16_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | (TC4_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
#endif
    while (GCLK->STATUS.bit.SYNCBUSY) {}

    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        if (TIMER_0_DEV.CTRLA.bit.ENABLE) {
            return 0;
        }
        PM->APBCMASK.reg |= PM_APBCMASK_TC3;
        /* reset timer */
        TIMER_0_DEV.CTRLA.bit.SWRST = 1;
        while (TIMER_0_DEV.CTRLA.bit.SWRST) {}
        /* choosing 16 bit mode */
        TIMER_0_DEV.CTRLA.bit.MODE = TC_CTRLA_MODE_COUNT16_Val;
#if CLOCK_USE_PLL
        /* sourced by 1MHz with prescaler 1 results in... you know it :-) */
        TIMER_0_DEV.CTRLA.bit.PRESCALER = TC_CTRLA_PRESCALER_DIV1_Val;
#else
        /* sourced by 8MHz with Presc 8 results in 1MHz clk */
        TIMER_0_DEV.CTRLA.bit.PRESCALER = TC_CTRLA_PRESCALER_DIV8_Val;
#endif
        /* choose normal frequency operation */
        TIMER_0_DEV.CTRLA.bit.WAVEGEN = TC_CTRLA_WAVEGEN_NFRQ_Val;
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        if (TIMER_1_DEV.CTRLA.bit.ENABLE) {
            return 0;
        }
        PM->APBCMASK.reg |= PM_APBCMASK_TC4;
        /* reset timer */
        TIMER_1_DEV.CTRLA.bit.SWRST = 1;

        while (TIMER_1_DEV.CTRLA.bit.SWRST) {}


        TIMER_1_DEV.CTRLA.bit.MODE = TC_CTRLA_MODE_COUNT32_Val;
#if CLOCK_USE_PLL
        /* sourced by 1MHz and prescaler 1 to reach 1MHz */
        TIMER_1_DEV.CTRLA.bit.PRESCALER = TC_CTRLA_PRESCALER_DIV1_Val;
#else
        /* sourced by 8MHz with Presc 8 results in 1Mhz clk */
        TIMER_1_DEV.CTRLA.bit.PRESCALER = TC_CTRLA_PRESCALER_DIV8_Val;
#endif
        /* choose normal frequency operation */
        TIMER_1_DEV.CTRLA.bit.WAVEGEN = TC_CTRLA_WAVEGEN_NFRQ_Val;
        break;
#endif
    case TIMER_UNDEFINED:
    default:
        return -1;
    }

    /* save callback */
    config[dev].cb = cb;
    config[dev].arg = arg;

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
#if TIMER_1_EN
    case TIMER_1:
        /* set timeout value */
        switch (channel) {
        case 0:
            TIMER_1_DEV.INTFLAG.bit.MC0 = 1;
            TIMER_1_DEV.CC[0].reg = value;
            TIMER_1_DEV.INTENSET.bit.MC0 = 1;
            break;
        case 1:
            TIMER_1_DEV.INTFLAG.bit.MC1 = 1;
            TIMER_1_DEV.CC[1].reg = value;
            TIMER_1_DEV.INTENSET.bit.MC1 = 1;
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
#if TIMER_1_EN
    case TIMER_1:
        switch (channel) {
        case 0:
            TIMER_1_DEV.INTFLAG.bit.MC0 = 1;
            TIMER_1_DEV.INTENCLR.bit.MC0 = 1;
            break;
        case 1:
            TIMER_1_DEV.INTFLAG.bit.MC1 = 1;
            TIMER_1_DEV.INTENCLR.bit.MC1 = 1;
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
        TIMER_0_DEV.READREQ.reg = TC_READREQ_RREQ | TC_READREQ_ADDR(0x10);
        while (TIMER_0_DEV.STATUS.bit.SYNCBUSY) {}
        return TIMER_0_DEV.COUNT.reg;
#endif
#if TIMER_1_EN
    case TIMER_1:
        /* request syncronisation */
        TIMER_1_DEV.READREQ.reg = TC_READREQ_RREQ | TC_READREQ_ADDR(0x10);
        while (TIMER_1_DEV.STATUS.bit.SYNCBUSY) {}
        return TIMER_1_DEV.COUNT.reg;
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
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV.CTRLA.bit.ENABLE = 0;
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
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV.CTRLA.bit.ENABLE = 1;
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
            NVIC_EnableIRQ(TC3_IRQn);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_EnableIRQ(TC4_IRQn);
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
            NVIC_DisableIRQ(TC3_IRQn);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_DisableIRQ(TC4_IRQn);
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
            config[TIMER_0].cb(config[TIMER_0].arg, 0);
        }
    }
    else if (TIMER_0_DEV.INTFLAG.bit.MC1 && TIMER_0_DEV.INTENSET.bit.MC1) {
        if(config[TIMER_0].cb) {
            TIMER_0_DEV.INTFLAG.bit.MC1 = 1;
            TIMER_0_DEV.INTENCLR.reg = TC_INTENCLR_MC1;
            config[TIMER_0].cb(config[TIMER_0].arg, 1);
        }
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /* TIMER_0_EN */


#if TIMER_1_EN
void TIMER_1_ISR(void)
{
    if (TIMER_1_DEV.INTFLAG.bit.MC0 && TIMER_1_DEV.INTENSET.bit.MC0) {
        if (config[TIMER_1].cb) {
            TIMER_1_DEV.INTFLAG.bit.MC0 = 1;
            TIMER_1_DEV.INTENCLR.reg = TC_INTENCLR_MC0;
            config[TIMER_1].cb(config[TIMER_1].arg, 0);
        }
    }
    else if (TIMER_1_DEV.INTFLAG.bit.MC1 && TIMER_1_DEV.INTENSET.bit.MC1) {
        if(config[TIMER_1].cb) {
            TIMER_1_DEV.INTFLAG.bit.MC1 = 1;
            TIMER_1_DEV.INTENCLR.reg = TC_INTENCLR_MC1;
            config[TIMER_1].cb(config[TIMER_1].arg, 1);
        }

    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /* TIMER_1_EN */

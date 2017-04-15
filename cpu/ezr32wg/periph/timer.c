/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_ezr32wg
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/timer.h"
#include "periph_conf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   This timer implementation has three available channels
 */
#define CC_CHANNELS      (3U)

/**
 * @brief   Timer state memory
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];


int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
    TIMER_TypeDef *pre, *tim;

    /* test if given timer device is valid */
    if (dev >= TIMER_NUMOF) {
        return -1;
    }

    /* save callback */
    isr_ctx[dev].cb = cb;
    isr_ctx[dev].arg = arg;

    /* get timers */
    pre = timer_config[dev].prescaler;
    tim = timer_config[dev].timer;

    /* power on timers (if not already powered on) */
    CMU->HFPERCLKEN0 |= (0x3 << timer_config[dev].pre_cmu);
    /* stop both (in case they are running) */
    pre->CMD = TIMER_CMD_STOP;
    tim->CMD = TIMER_CMD_STOP;
    /* configure the pre-scale timer to drive the actual timer. For this we
     * configure it up-counting, driven by the HFPER clock and we set the TOP
     * register depending on the specified timer speed value */
    pre->CTRL = 0;
    pre->TOP = ((CLOCK_HFPERCLK / freq) - 1);
    pre->CNT = 0;
    pre->IEN = 0;

    /* configure the actual timer to up-counting mode and to be fed by the
     * pre-scale timer */
    tim->CTRL = TIMER_CTRL_CLKSEL_TIMEROUF;
    tim->TOP = 0xffff;
    tim->CNT = 0;
    /* clear all CC interrupt flags and enable their interrupts */
    tim->IFC = (TIMER_IFC_CC0 | TIMER_IFC_CC1 | TIMER_IFC_CC2);
    tim->IEN = (TIMER_IEN_CC0 | TIMER_IEN_CC1 | TIMER_IEN_CC2);
    /* activate global timer interrupt */
    NVIC_EnableIRQ(timer_config[dev].irqn);
    /* start both timers */
    tim->CMD = TIMER_CMD_START;
    pre->CMD = TIMER_CMD_START;
    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    unsigned int now = timer_read(dev);
    timer_set_absolute(dev, channel, now + timeout);
    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    TIMER_TypeDef *tim;

    if (channel < 0 || channel >= CC_CHANNELS) {
        return -1;
    }

    tim = timer_config[dev].timer;
    tim->CC[channel].CCV = (uint16_t)value;
    tim->CC[channel].CTRL = TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    if (channel < 0 || channel >= CC_CHANNELS) {
        return -1;
    }

    timer_config[dev].timer->CC[channel].CTRL = _TIMER_CC_CTRL_MODE_OFF;
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    return (unsigned int)timer_config[dev].timer->CNT;
}

void timer_stop(tim_t dev)
{
    timer_config[dev].timer->CMD = TIMER_CMD_STOP;
}

void timer_start(tim_t dev)
{
    timer_config[dev].timer->CMD = TIMER_CMD_START;
}

void timer_reset(tim_t dev)
{
    timer_config[dev].timer->CNT = 0;
}

#ifdef TIMER_0_ISR
void TIMER_0_ISR(void)
{
    TIMER_TypeDef *tim = timer_config[0].timer;
    for (int i = 0; i < CC_CHANNELS; i++) {
        if (tim->IF & (TIMER_IF_CC0 << i)) {
            tim->CC[i].CTRL = _TIMER_CC_CTRL_MODE_OFF;
            tim->IFC = (TIMER_IFC_CC0 << i);
            isr_ctx[0].cb(isr_ctx[0].arg, i);
        }
    }
    cortexm_isr_end();
}
#endif /* TIMER_0_EN */

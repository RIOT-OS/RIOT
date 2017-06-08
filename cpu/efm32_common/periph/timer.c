/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32_common
 *
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"

#include "periph/timer.h"
#include "periph_conf.h"

#include "em_cmu.h"
#include "em_timer.h"

/* guard file in case no TIMER device is defined */
#if TIMER_NUMOF

/**
 * @brief   This timer implementation has three available channels
 */
#define CC_CHANNELS      (3U)

/**
 * @brief   Timer state memory
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

int timer_init(tim_t dev, unsigned int us_per_tick, void (*callback)(int))
{
    TIMER_TypeDef *pre, *tim;

    /* test if given timer device is valid */
    if (dev >= TIMER_NUMOF) {
        return -1;
    }

    /* save callback */
    isr_ctx[dev].cb = callback;

    /* get timers */
    pre = timer_config[dev].prescaler.dev;
    tim = timer_config[dev].timer.dev;

    /* enable clocks */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(timer_config[dev].prescaler.cmu, true);
    CMU_ClockEnable(timer_config[dev].timer.cmu, true);

    /* stop both (in case they are running) */
    TIMER_Enable(tim, false);
    TIMER_Enable(pre, false);

    /* configure the pre-scale timer to drive the actual timer. For this we
     * configure it up-counting, driven by the HFPER clock and we set the TOP
     * register depending on the specified timer speed value */
    pre->CTRL = 0;
    pre->TOP = ((CMU_ClockFreqGet(cmuClock_TIMER0) / 1000000) - 1) * us_per_tick;
    pre->CNT = 0;
    pre->IEN = 0;

    /* configure the actual timer to up-counting mode and to be fed by the
     * pre-scale timer */
    tim->CTRL = TIMER_CTRL_CLKSEL_TIMEROUF;
    tim->TOP = 0xffff;
    tim->CNT = 0;

    /* enable interrupts */
    TIMER_IntClear(tim, TIMER_IFC_CC0 | TIMER_IFC_CC1 | TIMER_IFC_CC2);
    TIMER_IntEnable(tim, TIMER_IEN_CC0 | TIMER_IEN_CC1 | TIMER_IEN_CC2);

    NVIC_ClearPendingIRQ(timer_config[dev].irq);
    NVIC_EnableIRQ(timer_config[dev].irq);

    /* start both timers */
    TIMER_Enable(tim, true);
    TIMER_Enable(pre, true);

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

    tim = timer_config[dev].timer.dev;
    tim->CC[channel].CCV = (uint16_t)value;
    tim->CC[channel].CTRL = TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    if (channel < 0 || channel >= CC_CHANNELS) {
        return -1;
    }

    timer_config[dev].timer.dev->CC[channel].CTRL = _TIMER_CC_CTRL_MODE_OFF;
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    return (unsigned int) TIMER_CounterGet(timer_config[dev].timer.dev);
}

void timer_stop(tim_t dev)
{
    TIMER_Enable(timer_config[dev].timer.dev, false);
}

void timer_start(tim_t dev)
{
    TIMER_Enable(timer_config[dev].timer.dev, true);
}

void timer_irq_enable(tim_t dev)
{
    NVIC_EnableIRQ(timer_config[dev].irq);
}

void timer_irq_disable(tim_t dev)
{
    NVIC_DisableIRQ(timer_config[dev].irq);
}

void timer_reset(tim_t dev)
{
    TIMER_CounterSet(timer_config[dev].timer.dev, 0);
}

#ifdef TIMER_0_ISR
void TIMER_0_ISR(void)
{
    TIMER_TypeDef *tim = timer_config[0].timer.dev;

    for (int i = 0; i < CC_CHANNELS; i++) {
        if (tim->IF & (TIMER_IF_CC0 << i)) {
            tim->CC[i].CTRL = _TIMER_CC_CTRL_MODE_OFF;
            tim->IFC = (TIMER_IFC_CC0 << i);
            isr_ctx[0].cb(i);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /* TIMER_0_ISR */

#endif /* TIMER_NUMOF */

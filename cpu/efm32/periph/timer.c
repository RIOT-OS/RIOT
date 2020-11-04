/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @author      Thomas Stilwell <stilwellt@openlabs.co>
 * @}
 */

#include "cpu.h"
#include "log.h"
#include "assert.h"
#include "periph/timer.h"
#include "periph_conf.h"
#include "pm_layered.h"

#include "em_cmu.h"
#include "em_timer.h"
#include "em_timer_utils.h"
#include "em_letimer.h"

/**
 * @brief   These power modes will be blocked while the timer is running
 */
#ifndef EFM32_TIMER_PM_BLOCKER
#define EFM32_TIMER_PM_BLOCKER      1
#endif
#ifndef EFM32_LETIMER_PM_BLOCKER
#define EFM32_LETIMER_PM_BLOCKER    0
#endif

/**
 * @brief   Timer state memory
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

/**
 * @brief   Check whether device is a using a WTIMER device (32-bit)
 */
static inline bool _is_wtimer(timer_t dev)
{
#if defined(WTIMER_COUNT) && WTIMER_COUNT > 0
    return ((uint32_t) timer_config[dev].timer.dev) >= WTIMER0_BASE;
#else
    (void) dev;
    return false;
#endif
}

/**
 * @brief   Check whether dev is using a LETIMER device
 */
static inline bool _is_letimer(timer_t dev)
{
#if defined(LETIMER_COUNT) && LETIMER_COUNT > 0
    return ((uint32_t) timer_config[dev].timer.dev) == LETIMER0_BASE;
#else
    (void) dev;
    return false;
#endif
}

static void _letimer_init(tim_t dev, uint32_t freq)
{
    (void) freq;
#if LETIMER_COUNT
    assert(freq == 32768);

    LETIMER_TypeDef *tim = timer_config[dev].timer.dev;

    /* enable clocks */
    CMU_ClockEnable(timer_config[dev].timer.cmu, true);
    CMU_ClockEnable(cmuClock_CORELE, true);

    /* disable and clear interrupts */
    LETIMER_IntDisable(tim, LETIMER_IEN_COMP0 | LETIMER_IEN_COMP1);
    LETIMER_IntClear(tim, LETIMER_IFC_COMP0 | LETIMER_IFC_COMP1);

    /* initialize timer without starting it yet */
    LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;
    letimerInit.enable = false;
    LETIMER_Init(tim, &letimerInit);
#else
    (void) dev;
#endif
}

static void _timer_init(tim_t dev, uint32_t freq)
{
    TIMER_TypeDef *pre, *tim;

    /* get timers */
    pre = timer_config[dev].prescaler.dev;
    tim = timer_config[dev].timer.dev;

    /* enable clocks */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(timer_config[dev].prescaler.cmu, true);
    CMU_ClockEnable(timer_config[dev].timer.cmu, true);

    /* reset and initialize peripherals */
    TIMER_Init_TypeDef init_pre = TIMER_INIT_DEFAULT;
    TIMER_Init_TypeDef init_tim = TIMER_INIT_DEFAULT;

    init_pre.enable = false;
    init_pre.prescale = timerPrescale1;
    init_tim.enable = false;
    init_tim.clkSel = timerClkSelCascade;

    TIMER_Reset(tim);
    TIMER_Reset(pre);

    TIMER_Init(tim, &init_tim);
    TIMER_Init(pre, &init_pre);

    /* configure the prescaler top value */
    uint32_t freq_timer = CMU_ClockFreqGet(timer_config[dev].prescaler.cmu);
    uint32_t top = (
        freq_timer / TIMER_Prescaler2Div(init_pre.prescale) / freq) - 1;

    TIMER_TopSet(pre, top);

    /* note: when changing this, update timer_set_absolute()'s TopGet,
     * which assumes either 0xffffffff or 0xffff
     */
    TIMER_TopSet(tim, _is_wtimer(dev) ? 0xffffffff : 0xffff);

    /* enable interrupts for the channels */
    TIMER_IntClear(tim, TIMER_IFC_CC0 | TIMER_IFC_CC1 | TIMER_IFC_CC2);
    TIMER_IntEnable(tim, TIMER_IEN_CC0 | TIMER_IEN_CC1 | TIMER_IEN_CC2);
}

int timer_init(tim_t dev, uint32_t freq, timer_cb_t callback, void *arg)
{
    /* test if given timer device is valid */
    if (dev >= TIMER_NUMOF) {
        return -1;
    }

    /* save callback */
    isr_ctx[dev].cb = callback;
    isr_ctx[dev].arg = arg;

    if (_is_letimer(dev)) {
        _letimer_init(dev, freq);
    } else {
        _timer_init(dev, freq);
    }

    NVIC_ClearPendingIRQ(timer_config[dev].irq);
    NVIC_EnableIRQ(timer_config[dev].irq);

    timer_start(dev);

    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (!_is_letimer(dev)) {
        TIMER_TypeDef *tim = timer_config[dev].timer.dev;

        if (channel < 0 || channel >= timer_config[dev].channel_numof) {
            return -1;
        }

        if (TIMER_TopGet(tim) == 0xFFFF) {
            value &= 0xFFFF;
        }

        tim->CC[channel].CCV = (uint32_t) value;
        tim->CC[channel].CTRL = TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
    }
    else {
#if LETIMER_COUNT
        LETIMER_TypeDef *tim = timer_config[dev].timer.dev;

        /* LETIMER is countdown only, so we invert the value */
        value = 0xffff - value;
        LETIMER_CompareSet(tim, channel, value);

        switch (channel)
        {
            case 0:
                LETIMER_IntClear(tim, LETIMER_IFC_COMP0);
                LETIMER_IntEnable(tim, LETIMER_IEN_COMP0);
                break;
            case 1:
                LETIMER_IntClear(tim, LETIMER_IFC_COMP1);
                LETIMER_IntEnable(tim, LETIMER_IEN_COMP1);
                break;
            default:
                return -2;
                break;
        }
#endif
    }

    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    if (!_is_letimer(dev)) {
        TIMER_TypeDef *tim = timer_config[dev].timer.dev;
        tim->CC[channel].CTRL = _TIMER_CC_CTRL_MODE_OFF;
    }
    else {
#if LETIMER_COUNT
        LETIMER_TypeDef *tim = timer_config[dev].timer.dev;
        switch (channel)
        {
            case 0:
                LETIMER_IntDisable(tim, LETIMER_IEN_COMP0);
                LETIMER_IntClear(tim, LETIMER_IFC_COMP0);
                break;
            case 1:
                LETIMER_IntDisable(tim, LETIMER_IEN_COMP1);
                LETIMER_IntClear(tim, LETIMER_IFC_COMP1);
                break;
            default:
                return -1;
        }
#endif
    }
    return 0;
}

unsigned int timer_read(tim_t dev)
{
#if LETIMER_COUNT
    if (_is_letimer(dev)) {
        /* LETIMER is countdown only, so we invert the value */
        return (unsigned int) 0xffff
                    - LETIMER_CounterGet(timer_config[dev].timer.dev);
    }
#endif
    return (unsigned int) TIMER_CounterGet(timer_config[dev].timer.dev);
}

void timer_stop(tim_t dev)
{
    if (_is_letimer(dev)) {
#if LETIMER_COUNT
        LETIMER_TypeDef *tim = timer_config[dev].timer.dev;
        if (tim->STATUS & LETIMER_STATUS_RUNNING) {
            pm_unblock(EFM32_LETIMER_PM_BLOCKER);
        }
        LETIMER_Enable(timer_config[dev].timer.dev, false);
#endif
    }
    else {
        TIMER_TypeDef *tim = timer_config[dev].timer.dev;
        if (tim->STATUS & TIMER_STATUS_RUNNING) {
            pm_unblock(EFM32_TIMER_PM_BLOCKER);
        }
        TIMER_Enable(timer_config[dev].timer.dev, false);
        TIMER_Enable(timer_config[dev].prescaler.dev, false);
    }
}

void timer_start(tim_t dev)
{
    if (_is_letimer(dev)) {
#if LETIMER_COUNT
        LETIMER_TypeDef *tim = timer_config[dev].timer.dev;
        if (!(tim->STATUS & LETIMER_STATUS_RUNNING)) {
            pm_block(EFM32_LETIMER_PM_BLOCKER);
        }
        LETIMER_Enable(timer_config[dev].timer.dev, true);
#endif
    }
    else {
        TIMER_TypeDef *tim = timer_config[dev].timer.dev;
        if (!(tim->STATUS & TIMER_STATUS_RUNNING)) {
            pm_block(EFM32_TIMER_PM_BLOCKER);
        }
        TIMER_Enable(timer_config[dev].timer.dev, true);
        TIMER_Enable(timer_config[dev].prescaler.dev, true);
    }
}

static void _timer_isr(tim_t dev)
{
    if (_is_letimer(dev)) {
#if LETIMER_COUNT
        LETIMER_TypeDef *tim = timer_config[dev].timer.dev;

        for (int i = 0; i < timer_config[dev].channel_numof; i++) {
            if (tim->IF & (LETIMER_IF_COMP0 << i))
            {
                LETIMER_IntDisable(tim, LETIMER_IEN_COMP0 << i);
                LETIMER_IntClear(tim, LETIMER_IFC_COMP0 << i);
                isr_ctx[dev].cb(isr_ctx[dev].arg, i);
            }
        }
#endif
    }
    else {
        TIMER_TypeDef *tim = timer_config[dev].timer.dev;

        for (int i = 0; i < timer_config[dev].channel_numof; i++) {
            if (tim->IF & (TIMER_IF_CC0 << i)) {
                tim->CC[i].CTRL = _TIMER_CC_CTRL_MODE_OFF;
                tim->IFC = (TIMER_IFC_CC0 << i);
                isr_ctx[dev].cb(isr_ctx[dev].arg, i);
            }
        }
    }
    cortexm_isr_end();
}

#ifdef TIMER_0_ISR
void TIMER_0_ISR(void)
{
    _timer_isr(0);
}
#endif /* TIMER_0_ISR */

#ifdef TIMER_1_ISR
void TIMER_1_ISR(void)
{
    _timer_isr(1);
}
#endif /* TIMER_1_ISR */

#ifdef TIMER_2_ISR
void TIMER_2_ISR(void)
{
    _timer_isr(2);
}
#endif /* TIMER_2_ISR */

#ifdef TIMER_3_ISR
void TIMER_3_ISR(void)
{
    _timer_isr(3);
}
#endif /* TIMER_3_ISR */

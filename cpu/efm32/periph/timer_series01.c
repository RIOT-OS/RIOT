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

#include "assert.h"
#include "atomic_utils.h"
#include "cpu.h"
#include "periph/timer.h"
#include "periph_conf.h"
#include "pm_layered.h"

#include "em_cmu.h"
#include "em_timer.h"
#include "em_timer_utils.h"
#include "em_letimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   These power modes will be blocked while the timer is running
 */
#ifndef EFM32_TIMER_PM_BLOCKER
#define EFM32_TIMER_PM_BLOCKER      1
#endif
#ifndef EFM32_LETIMER_PM_BLOCKER
#define EFM32_LETIMER_PM_BLOCKER    0
#endif

#define CHAN_INVALID                0xff

/**
 * @brief   Timer state memory
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

/* Bitmask storing which channel are periodic and which not. Only use
 * getter/setter functions to access */
static uint8_t is_periodic_mask[TIMER_NUMOF];

/* Number of the channel that modified the top value, or CHAN_INVALID.
 * Only one channel is allowed to use `TIM_FLAG_RESET_ON_MATCH` per
 * timer (this is enforced by this driver) here. */
static uint8_t chan_num_that_modified_top_value[TIMER_NUMOF];

/* Check if the given channel on the given timer is periodic */
static bool is_periodic(tim_t dev, int chan)
{
    /* We have one channel bitmask per timer, currently `uint8_t` per timer.
     * Make sure that this is large enough to store all channels. For current
     * hardware this is safe, but let's rather have this failing noisy. */
    assume((unsigned)chan < 8 * sizeof(is_periodic_mask[0]));
    assume((unsigned)dev < TIMER_NUMOF);

    return is_periodic_mask[dev] & (1U << chan);
}

/* Mark the given channel on the given timer as periodic */
static void mark_periodic(tim_t dev, int chan)
{
    assume((unsigned)chan < 8);
    assume((unsigned)dev < TIMER_NUMOF);

    atomic_fetch_or_u8(&is_periodic_mask[dev], 1U << chan);
}

/* Mark the given channel on the given timer as on-shot timer */
static void mark_one_shot(tim_t dev, int chan)
{
    assume((unsigned)chan < 8);
    assume((unsigned)dev < TIMER_NUMOF);

    atomic_fetch_and_u8(&is_periodic_mask[dev], ~(1U << chan));
}

/* Return the number of the channel that modified the top value of the given
 * timer, or -1 if the top value is MAX */
static int get_chan_which_modified_top_value(tim_t dev)
{
    assume((unsigned)dev < TIMER_NUMOF);
    if (chan_num_that_modified_top_value[dev] == CHAN_INVALID) {
        return -1;
    }

    return chan_num_that_modified_top_value[dev];
}

/* Mark the top value of the given timer as modified by the given channel */
static void set_chan_which_modified_top_value(tim_t dev, int channel)
{
    assume((unsigned)dev < TIMER_NUMOF);
    chan_num_that_modified_top_value[dev] = channel;
}

/* Check if the top value of the given timer is modified */
static bool is_top_value_modified(tim_t dev)
{
    assume((unsigned)dev < TIMER_NUMOF);
    return chan_num_that_modified_top_value[dev] != 0;
}

/* Mark the top value of the given timer as unmodified */
static void clear_top_value_modified(tim_t dev)
{
    assume((unsigned)dev < TIMER_NUMOF);
    chan_num_that_modified_top_value[dev] = CHAN_INVALID;
}

/**
 * @brief   Check whether device is a using a WTIMER device (32-bit)
 */
static inline bool _is_wtimer(tim_t dev)
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
static inline bool _is_letimer(tim_t dev)
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

    clear_top_value_modified(dev);
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

        /* if given timer is 16 bit (== !_is_wtimer()), drop leading bits
         * so that timer can actually be triggered */
        if (!_is_wtimer(dev)) {
            value &= 0xFFFF;
        }

        tim->CC[channel].CCV = (uint32_t) value;
        tim->CC[channel].CTRL = TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
        mark_one_shot(dev, channel);
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

int timer_set_periodic(tim_t dev, int channel, unsigned int value, uint8_t flags)
{
    if (_is_letimer(dev)) {
        /* periodic timer not implemented on le timers */
        return -1;
    }

    if (channel < 0 || channel >= timer_config[dev].channel_numof) {
        return -1;
    }

    /* if given timer is 16 bit (== !_is_wtimer()), drop leading bits
     * so that timer can actually be triggered */
    if (!_is_wtimer(dev)) {
        value &= 0xFFFF;
    }

    TIMER_TypeDef *tim = timer_config[dev].timer.dev;

    if ((flags & TIM_FLAG_RESET_ON_MATCH)
            && is_top_value_modified(dev)
            && (get_chan_which_modified_top_value(dev) != channel)) {
        DEBUG("[tim %u] cannot use TIM_FLAG_RESET_ON_MATCH for chan %d: "
              "chan %d already set top value\n",
              (unsigned)dev, channel, get_chan_which_modified_top_value(dev));
        return -1;
    }

    unsigned state = irq_disable();
    if (flags & TIM_FLAG_SET_STOPPED) {
        TIMER_Enable(timer_config[dev].timer.dev, false);
        TIMER_Enable(timer_config[dev].prescaler.dev, false);
    }

    if (flags & TIM_FLAG_RESET_ON_SET) {
        TIMER_CounterSet(tim, 0);
    }

    if (flags & TIM_FLAG_RESET_ON_MATCH) {
        TIMER_TopSet(tim, value);
        set_chan_which_modified_top_value(dev, channel);
        DEBUG("[tim %u] New top value = %u\n", (unsigned)dev, value);
    }

    tim->CC[channel].CCV = (uint32_t) value;
    tim->CC[channel].CTRL = TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
    mark_periodic(dev, channel);
    irq_restore(state);

    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    if (!_is_letimer(dev)) {
        unsigned state = irq_disable();
        TIMER_TypeDef *tim = timer_config[dev].timer.dev;
        tim->CC[channel].CTRL = _TIMER_CC_CTRL_MODE_OFF;

        if (get_chan_which_modified_top_value(dev) == channel) {
            TIMER_TopSet(tim, _is_wtimer(dev) ? 0xffffffff : 0xffff);
            clear_top_value_modified(dev);
            DEBUG("[tim %u] Reset top value\n", (unsigned)dev);
        }
        irq_restore(state);
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
                if (!is_periodic(dev, i)) {
                    tim->CC[i].CTRL = _TIMER_CC_CTRL_MODE_OFF;
                }
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

/*
 * SPDX-FileCopyrightText: 2022 SSV Software Systems GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Juergen Fitschen <me@jue.yt>
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
#ifndef TIMER_PM_BLOCKER
#define TIMER_PM_BLOCKER      EFM32_PM_MODE_EM2
#endif
#ifndef LETIMER_PM_BLOCKER
#define LETIMER_PM_BLOCKER    EFM32_PM_MODE_EM3
#endif

/**
 * @brief   Timer state memory
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

/**
 * @brief   Check whether dev is using a LETIMER device
 */
static inline bool _is_letimer(tim_t dev)
{
#if defined(LETIMER_COUNT) && (LETIMER_COUNT > 0)
    return ((uint32_t) timer_config[dev].dev) == LETIMER0_BASE;
#else
    (void) dev;
    return false;
#endif
}

#define LETIMER_CH_VALID(x) (x < 2)

static inline void _letimer_init(tim_t dev, uint32_t freq)
{
    (void) freq;
    assert(freq == CMU_ClockFreqGet(timer_config[dev].cmu));

    LETIMER_TypeDef *tim = timer_config[dev].dev;

    /* disable and clear interrupts */
    LETIMER_IntDisable(tim, LETIMER_IEN_COMP0 | LETIMER_IEN_COMP1);
    LETIMER_IntClear(tim, LETIMER_IF_COMP0 | LETIMER_IF_COMP1);

    /* initialize timer without starting it yet */
    LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
    init.enable = false;
    init.topValue = LETIMER_MAX_VALUE;
    LETIMER_Init(tim, &init);
}

static inline void _timer_init(tim_t dev, uint32_t freq)
{
    /* bring peripheral into known state */
    TIMER_Reset(timer_config[dev].dev);

    /* get input clock of the timer */
    uint32_t clk_freq = CMU_ClockFreqGet(timer_config[dev].cmu);

    /* initialize timer */
    TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;
    init.enable = false;
    init.prescale = (clk_freq / freq) - 1;
    /* check if any rounding errors occurred */
    assert(clk_freq / (init.prescale + 1) == freq);
    TIMER_Init(timer_config[dev].dev, &init);

    /* set top variable */
    TIMER_TopSet(timer_config[dev].dev, TIMER_MAX_VALUE);

    /* initialize CC channels */
    TIMER_InitCC_TypeDef init_cc = TIMER_INITCC_DEFAULT;
    init_cc.mode = timerCCModeCompare;
    for (size_t i = 0; TIMER_CH_VALID(i); i++) {
        TIMER_InitCC(timer_config[dev].dev, i, &init_cc);
    }
}

int timer_init(tim_t dev, uint32_t freq, timer_cb_t callback, void *arg)
{
    assert(dev < TIMER_NUMOF);

    /* enable clocks */
    CMU_ClockEnable(timer_config[dev].cmu, true);

    /* init underlying hardware */
    _is_letimer(dev) ? _letimer_init(dev, freq) : _timer_init(dev, freq);

    /* save callback */
    isr_ctx[dev].cb = callback;
    isr_ctx[dev].arg = arg;

    /* setup Cortex-M IRQ line */
    NVIC_ClearPendingIRQ(timer_config[dev].irq);
    NVIC_EnableIRQ(timer_config[dev].irq);

    timer_start(dev);

    return 0;
}

static inline int _letimer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (!LETIMER_CH_VALID(channel)) {
        return -1;
    }

    LETIMER_TypeDef *tim = timer_config[dev].dev;

    /* LETIMER is countdown only, so we invert the value */
    value = LETIMER_MAX_VALUE - value;
    LETIMER_CompareSet(tim, channel, value);

    uint32_t irq_bit = (LETIMER_IF_COMP0 << channel);
    LETIMER_IntClear(tim, irq_bit);
    LETIMER_IntEnable(tim, irq_bit);

    return 0;
}

static inline int _timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (!TIMER_CH_VALID(channel)) {
        return -1;
    }

    TIMER_TypeDef *tim = timer_config[dev].dev;
    uint32_t irq_bit = (TIMER_IF_CC0 << channel);

    /* make sure to clear previously set irqs */
    TIMER_IntClear(tim, irq_bit);

    /* set compare value */
    TIMER_CompareSet(tim, channel, value);

    /* turn on IRQs */
    TIMER_IntEnable(tim, irq_bit);

    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    return _is_letimer(dev) ? _letimer_set_absolute(dev, channel, value) : _timer_set_absolute(dev, channel, value);
}

static inline int _letimer_clear(tim_t dev, int channel)
{
    if (!LETIMER_CH_VALID(channel)) {
        return -1;
    }

    LETIMER_TypeDef *tim = timer_config[dev].dev;
    uint32_t irq_bit = (LETIMER_IF_COMP0 << channel);
    LETIMER_IntDisable(tim, irq_bit);
    LETIMER_IntClear(tim, irq_bit);

    return 0;
}

static inline int _timer_clear(tim_t dev, int channel)
{
    if (!TIMER_CH_VALID(channel)) {
        return -1;
    }

    /* turn off output compare IRQ */
    TIMER_IntDisable(timer_config[dev].dev, (TIMER_IF_CC0 << channel));

    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    return _is_letimer(dev) ? _letimer_clear(dev, channel) : _timer_clear(dev, channel);
}

static inline unsigned int _letimer_read(tim_t dev)
{
    LETIMER_TypeDef *tim = timer_config[dev].dev;

    /* LETIMER is countdown only, so we invert the value */
    return (unsigned int) LETIMER_MAX_VALUE - LETIMER_CounterGet(tim);
}

static inline unsigned int _timer_read(tim_t dev)
{
    TIMER_TypeDef *tim = timer_config[dev].dev;

    return (unsigned int) TIMER_CounterGet(tim);
}

unsigned int timer_read(tim_t dev)
{
    return _is_letimer(dev) ? _letimer_read(dev) : _timer_read(dev);
}

static inline bool _letimer_is_running(LETIMER_TypeDef *tim)
{
    /* make sure registers are up-to-date and
     * we can rely on the STATS register */
    LETIMER_SyncWait(tim);

    return tim->STATUS & LETIMER_STATUS_RUNNING;
}

static inline bool _timer_is_running(TIMER_TypeDef *tim)
{
    /* make sure registers are up-to-date and
     * we can rely on the STATS register */
    TIMER_SyncWait(tim);

    return tim->STATUS & TIMER_STATUS_RUNNING;
}

static inline void _letimer_stop(tim_t dev)
{
    LETIMER_TypeDef *tim = timer_config[dev].dev;

    if (_letimer_is_running(tim)) {
        pm_unblock(LETIMER_PM_BLOCKER);
    }

    LETIMER_Enable(tim, false);
}

static inline void _timer_stop(tim_t dev)
{
    TIMER_TypeDef *tim = timer_config[dev].dev;

    if (_timer_is_running(tim)) {
        pm_unblock(TIMER_PM_BLOCKER);
    }

    TIMER_Enable(tim, false);
}

void timer_stop(tim_t dev)
{
    _is_letimer(dev) ? _letimer_stop(dev) : _timer_stop(dev);
}

static inline void _letimer_start(tim_t dev)
{
    LETIMER_TypeDef *tim = timer_config[dev].dev;

    if (!_letimer_is_running(tim)) {
        pm_block(LETIMER_PM_BLOCKER);
    }

    LETIMER_Enable(tim, true);
}

static inline void _timer_start(tim_t dev)
{
    TIMER_TypeDef *tim = timer_config[dev].dev;

    if (!_timer_is_running(tim)) {
        pm_block(TIMER_PM_BLOCKER);
    }

    TIMER_Enable(tim, true);
}

void timer_start(tim_t dev)
{
    _is_letimer(dev) ? _letimer_start(dev) : _timer_start(dev);
}

static inline void _letimer_isr(tim_t dev)
{
    LETIMER_TypeDef *tim = timer_config[dev].dev;

    for (int i = 0; TIMER_CH_VALID(i); i++) {
        if (tim->IF & (LETIMER_IF_COMP0 << i))
        {
            LETIMER_IntDisable(tim, LETIMER_IEN_COMP0 << i);
            LETIMER_IntClear(tim, LETIMER_IF_COMP0 << i);
            isr_ctx[dev].cb(isr_ctx[dev].arg, i);
        }
    }
}

static inline void _timer_isr(tim_t dev)
{
    TIMER_TypeDef *tim = timer_config[dev].dev;

    for (size_t i = 0; TIMER_CH_VALID(i); i++) {
        uint32_t irq_bit = (TIMER_IF_CC0 << i);

        if (TIMER_IntGetEnabled(tim) & irq_bit) {
            TIMER_IntDisable(tim, irq_bit);
            TIMER_IntClear(tim, irq_bit);
            isr_ctx[dev].cb(isr_ctx[dev].arg, i);
        }
    }
}

static void _isr(tim_t dev)
{
    _is_letimer(dev) ? _letimer_isr(dev) : _timer_isr(dev);

    cortexm_isr_end();
}

#ifdef TIMER_0_ISR
void TIMER_0_ISR(void)
{
    _isr(0);
}
#endif /* TIMER_0_ISR */

#ifdef TIMER_1_ISR
void TIMER_1_ISR(void)
{
    _isr(1);
}
#endif /* TIMER_1_ISR */

#ifdef TIMER_2_ISR
void TIMER_2_ISR(void)
{
    _isr(2);
}
#endif /* TIMER_2_ISR */

#ifdef TIMER_3_ISR
void TIMER_3_ISR(void)
{
    _isr(3);
}
#endif /* TIMER_3_ISR */

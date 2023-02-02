/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_countdown
 * @{
 *
 * @file        countdown.c
 * @brief       Low-level countdown timer implementation
 *
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <assert.h>
#include "board.h"
#include "periph_conf.h"
#include "periph/countdown.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* default to using TCC if nothing else is defined */
#if defined(REV_TCC)
#define TIMER_USE_TCC 1
#endif
/* use TC if no TCC is available */
#if defined(REV_TC)
#define TIMER_USE_TC  1
#endif

/* dummy defines to not litter the code with ifdefs if no TCC is available */
#ifndef REV_TCC
typedef TcCount8 Tcc;
#define TCC_CTRLA_ENABLE TC_CTRLA_ENABLE
#define TCC_SYNCBUSY_CC0 TC_SYNCBUSY_CC0
#ifdef TC_SYNCBUSY_MASK
#define TCC_SYNCBUSY_MASK TC_SYNCBUSY_MASK
#endif
#endif /* !REV_TCC */

static inline Tc *_tc(const countdown_conf_t *cfg)
{
#ifdef REV_TC
    return cfg->tim.dev.tc;
#else
    (void)cfg;
    return NULL;
#endif
}

static inline Tcc *_tcc(const countdown_conf_t *cfg)
{
#ifdef REV_TCC
    return cfg->tim.dev.tcc;
#else
    (void)cfg;
    return NULL;
#endif
}

static inline void poweron(const tc_tcc_cfg_t *tim, uint8_t gclk_src)
{
    sam0_gclk_enable(gclk_src);

#ifdef MCLK
    GCLK->PCHCTRL[tim->gclk_id].reg = GCLK_PCHCTRL_GEN(gclk_src)
                                    | GCLK_PCHCTRL_CHEN;
    *tim->mclk |= tim->mclk_mask;
#else
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN
                      | GCLK_CLKCTRL_GEN(gclk_src)
                      | GCLK_CLKCTRL_ID(tim->gclk_id);
    PM->APBCMASK.reg |= tim->pm_mask;
#endif
}

static inline void poweroff(const tc_tcc_cfg_t *tim)
{
#ifdef MCLK
    GCLK->PCHCTRL[tim->gclk_id].reg = 0;
    *tim->mclk &= ~tim->mclk_mask;
#else
    PM->APBCMASK.reg &= ~tim->pm_mask;
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN(SAM0_GCLK_DISABLED)
                      | GCLK_CLKCTRL_ID(tim->gclk_id);
#endif
}

static inline void _check_defines(void)
{
#ifdef TCC_CTRLA_PRESCALER_DIV1024_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV1024_Val == TC_CTRLA_PRESCALER_DIV1024_Val,
                  "TCC and TC prescaler not equal");
#endif
#ifdef TCC_CTRLA_PRESCALER_DIV256_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV256_Val == TC_CTRLA_PRESCALER_DIV256_Val,
                  "TCC and TC prescaler not equal");
#endif
#ifdef TCC_CTRLA_PRESCALER_DIV64_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV64_Val == TC_CTRLA_PRESCALER_DIV64_Val,
                  "TCC and TC prescaler not equal");
#endif
#ifdef TCC_CTRLA_PRESCALER_DIV16_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV16_Val == TC_CTRLA_PRESCALER_DIV16_Val,
                  "TCC and TC prescaler not equal");
#endif
#ifdef TCC_CTRLA_PRESCALER_DIV8_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV8_Val == TC_CTRLA_PRESCALER_DIV8_Val,
                  "TCC and TC prescaler not equal");
#endif
#ifdef TCC_CTRLA_PRESCALER_DIV4_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV4_Val == TC_CTRLA_PRESCALER_DIV4_Val,
                  "TCC and TC prescaler not equal");
#endif
#ifdef TCC_CTRLA_PRESCALER_DIV2_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV2_Val == TC_CTRLA_PRESCALER_DIV2_Val,
                  "TCC and TC prescaler not equal");
#endif
#ifdef TCC_CTRLA_PRESCALER_DIV1_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV1_Val == TC_CTRLA_PRESCALER_DIV1_Val,
                  "TCC and TC prescaler not equal");
#endif
}

static uint8_t _get_prescaler(unsigned int target, int *scale)
{
    _check_defines();

    if (target >= 512) {
        *scale = 1024;
        return TC_CTRLA_PRESCALER_DIV1024_Val;
    }
    if (target >= 128) {
        *scale = 256;
        return TC_CTRLA_PRESCALER_DIV256_Val;
    }
    if (target >= 32) {
        *scale = 64;
        return TC_CTRLA_PRESCALER_DIV64_Val;
    }
    if (target >= 12) {
        *scale = 16;
        return TC_CTRLA_PRESCALER_DIV16_Val;
    }
    if (target >= 6) {
        *scale = 8;
        return TC_CTRLA_PRESCALER_DIV8_Val;
    }
    if (target >= 3) {
        *scale = 4;
        return TC_CTRLA_PRESCALER_DIV4_Val;
    }
    if (target >= 2) {
        *scale = 2;
        return TC_CTRLA_PRESCALER_DIV2_Val;
    }

    *scale = 1;
    return TC_CTRLA_PRESCALER_DIV1_Val;
}

static inline uint32_t _pre_config(const countdown_conf_t *cfg, uint32_t mask,
                                   uint32_t max_range, uint8_t flags, uint8_t *prescaler)
{
    const uint32_t f_src = sam0_gclk_freq(cfg->gclk_src);
    uint32_t max_range_us = ((uint64_t)US_PER_SEC * mask) / f_src;
    unsigned div = countdown_unit_mul_us(max_range, flags) / max_range_us;
    int scale = 0;

    *prescaler = _get_prescaler(div, &scale);

    DEBUG("countdown clock divider: %lu Hz / %d -> %lu Hz\n", f_src, scale, f_src / scale);

    if (scale == 0) {
        return 0;
    }

    poweron(&cfg->tim, cfg->gclk_src);

    return f_src / scale;
}

#ifdef COUNTDOWN_TC_NUMOF

/**
 * @brief Interrupt callbacks
 * @{
 */
static countdown_cb_t irq_cb_tc[COUNTDOWN_TC_NUMOF];
static void *irq_arg_tc[COUNTDOWN_TC_NUMOF];
/** @} */

static inline void _tc_syncbusy(Tc *tc)
{
#ifdef TC_STATUS_SYNCBUSY
    while (tc->COUNT16.STATUS.bit.SYNCBUSY) {}
#else
    while (tc->COUNT16.SYNCBUSY.reg) {}
#endif
}

static uint32_t _init_tc(const void *ctx, uint32_t max_range, unsigned flags, uint32_t *mask)
{
    const countdown_conf_t *cfg = ctx;
    *mask = TC_COUNT16_COUNT_MASK;

    Tc *tc = _tc(ctx);
    DEBUG("Tc[%u] init\n", index_of(countdown_config_tc, ctx));

    uint8_t prescaler;
    const uint32_t f_out = _pre_config(cfg, *mask , max_range, flags, &prescaler);

    if (!f_out) {
        return 0;
    }

    /* reset TC module */
    tc->COUNT16.CTRLA.bit.SWRST = 1;
    while (tc->COUNT16.CTRLA.bit.SWRST) {}

    /* configure the TC device */
    tc->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16
                          | TC_CTRLA_PRESCALER(prescaler)
                          | TC_CTRLA_PRESCSYNC_RESYNC
#ifdef TC_CTRLA_WAVEGEN_MFRQ
                          | TC_CTRLA_WAVEGEN_MFRQ
#endif
#ifdef TC_CTRLA_ONDEMAND
                          | TC_CTRLA_ONDEMAND
#endif
                          ;
#ifndef TC_CTRLA_WAVEGEN_MFRQ
    /* MFRQ -> MC0 will be TOP value */
    tc->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;
#endif

    /* only enable overflow interrupt */
    tc->COUNT16.INTENSET.reg = TC_INTENSET_OVF;

    NVIC_EnableIRQ(cfg->tim.irq);

    return f_out;
}

static void _set_tc(const void *ctx, uint32_t alarm, bool one_shot)
{
    Tc *tc = _tc(ctx);

    DEBUG("Tc[%u] alarm: %lu\n", index_of(countdown_config_tc, ctx), alarm);


    /* set must disable the timer */
    tc->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
    tc->COUNT16.CC[0].reg = alarm;

    _tc_syncbusy(tc);

    if (one_shot) {
        tc->COUNT16.CTRLBSET.reg = TC_CTRLBSET_ONESHOT;
    } else {
        tc->COUNT16.CTRLBCLR.reg = TC_CTRLBCLR_ONESHOT;
    }
}

static void _start_tc(const void *ctx)
{
    Tc *tc = _tc(ctx);
    DEBUG("Tc[%u] start\n", index_of(countdown_config_tc, ctx));

    tc->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
}

static void _cancel_tc(const void *ctx)
{
    Tc *tc = _tc(ctx);
    DEBUG("Tc[%u] cancel\n", index_of(countdown_config_tc, ctx));

    tc->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
    _tc_syncbusy(tc);

    tc->COUNT16.COUNT.reg = 0;
}

static void _set_cb_tc(const void *ctx, countdown_cb_t cb, void *arg)
{
    unsigned idx = index_of(countdown_config_tc, ctx);

    unsigned state = irq_disable();
    irq_cb_tc[idx] = cb;
    irq_arg_tc[idx] = arg;
    irq_restore(state);
}

static void _get_cb_tc(const void *ctx, countdown_cb_t *cb, void **arg)
{
    unsigned idx = index_of(countdown_config_tc, ctx);

    *cb = irq_cb_tc[idx];
    *arg = irq_arg_tc[idx];
}

static inline void _isr_tc(unsigned idx)
{
    Tc *tc = _tc(&countdown_config_tc[idx]);

    uint32_t status = tc->COUNT16.INTFLAG.reg;
    tc->COUNT16.INTFLAG.reg = status;

    DEBUG_PUTS("IRQ Tc");

    if (status & TC_INTFLAG_OVF) {
        irq_cb_tc[idx](irq_arg_tc[idx]);
    }
}

const countdown_desc_t _driver_timer_tc = {
    .init   = _init_tc,
    .set_cb = _set_cb_tc,
    .get_cb = _get_cb_tc,
    .set    = _set_tc,
    .start  = _start_tc,
    .cancel = _cancel_tc,
};

XFA_CONST(countdown_timers_xfa, 1)
const countdown_dev_t countdown_timers_tc[COUNTDOWN_TC_NUMOF] = {
#ifdef COUNTDOWN_TC_0_ISR
    {
        .driver = &_driver_timer_tc,
        .ctx    = &countdown_config_tc[0],
    },
#endif
#ifdef COUNTDOWN_TC_1_ISR
    {
        .driver = &_driver_timer_tc,
        .ctx    = &countdown_config_tc[1],
    },
#endif
#ifdef COUNTDOWN_TC_2_ISR
    {
        .driver = &_driver_timer_tc,
        .ctx    = &countdown_config_tc[2],
    },
#endif
#ifdef COUNTDOWN_TC_3_ISR
    {
        .driver = &_driver_timer_tc,
        .ctx    = &countdown_config_tc[3],
    },
#endif
#ifdef COUNTDOWN_TC_4_ISR
    {
        .driver = &_driver_timer_tc,
        .ctx    = &countdown_config_tc[4],
    },
#endif
#ifdef COUNTDOWN_TC_5_ISR
    {
        .driver = &_driver_timer_tc,
        .ctx    = &countdown_config_tc[5],
    },
#endif
#ifdef COUNTDOWN_TC_6_ISR
    {
        .driver = &_driver_timer_tc,
        .ctx    = &countdown_config_tc[6],
    },
#endif
#ifdef COUNTDOWN_TC_7_ISR
    {
        .driver = &_driver_timer_tc,
        .ctx    = &countdown_config_tc[7],
    },
#endif
};

XFA(countdown_cfg_xfa, 1)
countdown_cfg_t countdown_cfg_tc[COUNTDOWN_TC_NUMOF];
#endif /* COUNTDOWN_TC_NUMOF */

#ifdef COUNTDOWN_TCC_NUMOF
/**
 * @brief Interrupt callbacks
 * @{
 */
static countdown_cb_t irq_cb_tcc[COUNTDOWN_TCC_NUMOF];
static void *irq_arg_tcc[COUNTDOWN_TCC_NUMOF];
/** @} */

static inline void _tcc_syncbusy(Tcc *tcc)
{
    while (tcc->SYNCBUSY.reg) {}
}

static uint32_t _init_tcc(const void *ctx, uint32_t max_range, unsigned flags, uint32_t *mask)
{
    const countdown_conf_t *cfg = ctx;
    *mask = TC_COUNT16_COUNT_MASK;

    Tcc *tcc = _tcc(ctx);
    DEBUG("Tcc[%u] init\n", index_of(countdown_config_tcc, ctx));

    uint8_t prescaler;
    const uint32_t f_out = _pre_config(cfg, *mask , max_range, flags, &prescaler);

    if (!f_out) {
        return 0;
    }

    /* reset TCC module */
    tcc->CTRLA.reg = TCC_CTRLA_SWRST;
    _tcc_syncbusy(tcc);
    while (tcc->SYNCBUSY.reg) {}

    /* configure the TCC device */
    tcc->CTRLA.reg = TCC_CTRLA_PRESCSYNC_RESYNC
                   | TCC_CTRLA_PRESCALER(prescaler);

    /* MFRQ -> MC0 will be TOP value */
    tcc->WAVE.reg = TCC_WAVE_WAVEGEN_MFRQ;

    /* only enable overflow interrupt */
    tcc->INTENSET.reg = TCC_INTENSET_OVF;

    NVIC_EnableIRQ(cfg->tim.irq);

    return f_out;
}

static void __set_tcc(const void *ctx, uint32_t alarm, bool one_shot)
{
    Tcc *tcc = _tcc(ctx);
    DEBUG("Tcc[%u] alarm: %lu\n", index_of(countdown_config_tcc, ctx), alarm);

    /* set must disable the timer */
    tcc->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
    tcc->CC[0].reg = alarm;

    _tcc_syncbusy(tcc);

    /* writing to CTRLBSET while SYNCBUSY causes hard fault on SAM D2x */
    if (one_shot) {
        tcc->CTRLBSET.reg = TCC_CTRLBSET_ONESHOT;
    } else {
        tcc->CTRLBCLR.reg = TCC_CTRLBCLR_ONESHOT;
    }
}

static void _start_tcc(const void *ctx)
{
    Tcc *tcc = _tcc(ctx);
    DEBUG("Tcc[%u] start\n", index_of(countdown_config_tcc, ctx));

    tcc->CTRLA.reg |= TCC_CTRLA_ENABLE;
}

static void _cancel_tcc(const void *ctx)
{
    Tcc *tcc = _tcc(ctx);
    DEBUG("Tcc[%u] cancel\n", index_of(countdown_config_tcc, ctx));

    tcc->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
    _tcc_syncbusy(tcc);

    /* writing to COUNT while SYNCBUSY causes hard fault on SAM D2x */
    tcc->COUNT.reg = 0;
}

static void _set_cb_tcc(const void *ctx, countdown_cb_t cb, void *arg)
{
    unsigned idx = index_of(countdown_config_tcc, ctx);

    unsigned state = irq_disable();
    irq_cb_tcc[idx] = cb;
    irq_arg_tcc[idx] = arg;
    irq_restore(state);
}

static void _get_cb_tcc(const void *ctx, countdown_cb_t *cb, void **arg)
{
    unsigned idx = index_of(countdown_config_tcc, ctx);

    *cb = irq_cb_tcc[idx];
    *arg = irq_arg_tcc[idx];
}

static inline void _isr_tcc(unsigned idx)
{
    Tcc *tcc = _tcc(&countdown_config_tcc[idx]);

    uint32_t status = tcc->INTFLAG.reg;
    tcc->INTFLAG.reg = status;

    DEBUG_PUTS("IRQ Tcc");

    if (status & TCC_INTFLAG_OVF) {
        irq_cb_tcc[idx](irq_arg_tcc[idx]);
    }
}

const countdown_desc_t _driver_timer_tcc = {
    .init   = __init_tcc,
    .set_cb = __set_cb_tcc,
    .get_cb = __get_cb_tcc,
    .set    = __set_tcc,
    .start  = __start_tcc,
    .cancel = __cancel_tcc,
};

XFA_CONST(countdown_timers_xfa, 1)
const countdown_dev_t countdown_timers_tcc[COUNTDOWN_TCC_NUMOF] = {
#ifdef COUNTDOWN_TCC_0_ISR
    {
        .driver = &_driver_timer_tcc,
        .ctx    = &countdown_config_tcc[0],
    },
#endif
#ifdef COUNTDOWN_TCC_1_ISR
    {
        .driver = &_driver_timer_tcc,
        .ctx    = &countdown_config_tcc[1],
    },
#endif
#ifdef COUNTDOWN_TCC_2_ISR
    {
        .driver = &_driver_timer_tcc,
        .ctx    = &countdown_config_tcc[2],
    },
#endif
#ifdef COUNTDOWN_TCC_3_ISR
    {
        .driver = &_driver_timer_tcc,
        .ctx    = &countdown_config_tcc[3],
    },
#endif
#ifdef COUNTDOWN_TCC_4_ISR
    {
        .driver = &_driver_timer_tcc,
        .ctx    = &countdown_config_tcc[4],
    },
#endif
};

XFA(countdown_cfg_xfa, 1)
countdown_cfg_t countdown_cfg_tcc[COUNTDOWN_TCC_NUMOF];
#endif /* COUNTDOWN_TCC_NUMOF */

#ifdef COUNTDOWN_TC_0_ISR
void COUNTDOWN_TC_0_ISR(void)
{
    _isr_tc(0);
    cortexm_isr_end();
}
#endif
#ifdef COUNTDOWN_TC_1_ISR
void COUNTDOWN_TC_1_ISR(void)
{
    _isr_tc(1);
    cortexm_isr_end();
}
#endif
#ifdef COUNTDOWN_TC_2_ISR
void COUNTDOWN_TC_2_ISR(void)
{
    _isr_tc(2);
    cortexm_isr_end();
}
#endif
#ifdef COUNTDOWN_TC_3_ISR
void COUNTDOWN_TC_3_ISR(void)
{
    _isr_tc(3);
    cortexm_isr_end();
}
#endif
#ifdef COUNTDOWN_TC_4_ISR
void COUNTDOWN_TC_4_ISR(void)
{
    _isr_tc(4);
    cortexm_isr_end();
}
#endif
#ifdef COUNTDOWN_TC_5_ISR
void COUNTDOWN_TC_5_ISR(void)
{
    _isr_tc(5);
    cortexm_isr_end();
}
#endif
#ifdef COUNTDOWN_TC_6_ISR
void COUNTDOWN_TC_6_ISR(void)
{
    _isr_tc(6);
    cortexm_isr_end();
}
#endif
#ifdef COUNTDOWN_TC_7_ISR
void COUNTDOWN_TC_7_ISR(void)
{
    _isr_tc(7);
    cortexm_isr_end();
}
#endif

#ifdef COUNTDOWN_TCC_0_ISR
void COUNTDOWN_TCC_0_ISR(void)
{
    _isr_tcc(0);
    cortexm_isr_end();
}
#endif
#ifdef COUNTDOWN_TCC_1_ISR
void COUNTDOWN_TCC_1_ISR(void)
{
    _isr_tcc(1);
    cortexm_isr_end();
}
#endif
#ifdef COUNTDOWN_TCC_2_ISR
void COUNTDOWN_TCC_2_ISR(void)
{
    _isr_tcc(2);
    cortexm_isr_end();
}
#endif
#ifdef COUNTDOWN_TCC_3_ISR
void COUNTDOWN_TCC_3_ISR(void)
{
    _isr_tcc(3);
    cortexm_isr_end();
}
#endif
#ifdef COUNTDOWN_TCC_4_ISR
void COUNTDOWN_TCC_4_ISR(void)
{
    _isr_tcc(4);
    cortexm_isr_end();
}
#endif

/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_freqm
 * @{
 *
 * @file        freqm.c
 * @brief       Frequency meter driver implementation
 *
 * @author      Urs Gompper <urs.gompper@ml-pa.com>
 *
 * @}
 */

#include "periph/freqm.h"

/* TODO:   Remove defines when Microchip vendor files (which include these
 *         defines) get updated.
 */
/* FREQM_GCLK_ID_REF is defined in newer versions of vendor header files */
#ifndef FREQM_GCLK_ID_REF
#define FREQM_GCLK_ID_REF (FREQM_GCLK_ID_MSR + 1)
#endif

/* Channel Enable Mask */
#define GCLK_PCHCTRL_CHEN_Msk   (_U_(0x1) << GCLK_PCHCTRL_CHEN_Pos)
/* Enable Mask */
#define FREQM_CTRLA_ENABLE_Msk  (_U_(0x1) << FREQM_CTRLA_ENABLE_Pos)
/* Start Measurement Mask */
#define FREQM_CTRLB_START_Msk   (_U_(0x1) << FREQM_CTRLB_START_Pos)
/* Measurement Done Interrupt Enable Mask */
#define FREQM_INTENSET_DONE_Msk (_U_(0x1) << FREQM_INTENSET_DONE_Pos)
/* Measurement Done Mask */
#define FREQM_INTFLAG_DONE_Msk  (_U_(0x1) << FREQM_INTFLAG_DONE_Pos)
/* FREQM Status Mask */
#define FREQM_STATUS_BUSY_Msk   (_U_(0x1) << FREQM_STATUS_BUSY_Pos)
/* Sticky Count Value Overflow Mask */
#define FREQM_STATUS_OVF_Msk    (_U_(0x1) << FREQM_STATUS_OVF_Pos)

/* check if pin has peripheral function GCLK */
static int _freqm_pin(gpio_t pin)
{
    for (unsigned i = 0; i < ARRAY_SIZE(gclk_io_pins); ++i) {
        if (gclk_io_pins[i] == pin) {
            return i;
        }
    }

    return -1;
}

static void _gclk_connect(uint8_t id, uint8_t src, uint32_t flags)
{
    GCLK->GENCTRL[id].reg = GCLK_GENCTRL_SRC(src) | GCLK_GENCTRL_GENEN | flags | GCLK_GENCTRL_IDC;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(id)) {}
}

static int _freqm_gpio_init(gpio_t msr_gpio_src, uint8_t *gclk_io_id)
{
    /* Check if selected pin has peripheral function GCLK */
    int index = _freqm_pin(msr_gpio_src);

    /* Fail assertion if pin has no peripheral function GCLK */
    assert(index > 0);

    /* Lookup which GCLK_IO[x] must be used */
    *gclk_io_id = gclk_io_ids[index];
    /* GCLK_IO[0] and GCLK_IO[1] can't be used here. They are associated with
       GCLKGEN[0] and GCLKGEN[1] respectively. These in turn are used by
       SAM0_GCLK_MAIN and SAM0_GCLK_32KHZ respectively */
    assert(*gclk_io_id > 1);

    /* Initialize GPIO as input */
    gpio_init(msr_gpio_src, GPIO_IN);
    /* Enable peripheral function GCLK/IO on GPIO */
    gpio_init_mux(msr_gpio_src, GPIO_MUX_M);
    /* Connect GCLK_IO[*gclk_io_id] with input pin */
    _gclk_connect(*gclk_io_id, GCLK_SOURCE_GCLKIN, 0);

    return 0;
}

static void _freqm_clock_init(uint8_t pin, uint8_t gclk_src)
{
    /* Selection of the Generator and write Lock for FREQM_MSR */
    GCLK->PCHCTRL[FREQM_GCLK_ID_MSR].reg = GCLK_PCHCTRL_GEN(pin) | GCLK_PCHCTRL_CHEN_Msk;
    /* Wait for synchronization */
    while ((GCLK->PCHCTRL[FREQM_GCLK_ID_MSR].reg & GCLK_PCHCTRL_CHEN_Msk) !=
           GCLK_PCHCTRL_CHEN_Msk) {}

    /* Selection of the Generator and write Lock for FREQM_REF */
    GCLK->PCHCTRL[FREQM_GCLK_ID_REF].reg = GCLK_PCHCTRL_GEN(gclk_src) | GCLK_PCHCTRL_CHEN_Msk;
    /* Wait for synchronization */
    while ((GCLK->PCHCTRL[FREQM_GCLK_ID_REF].reg & GCLK_PCHCTRL_CHEN_Msk) !=
           GCLK_PCHCTRL_CHEN_Msk) {}
}

static struct {
    freqm_cb_t callback;
    void *context;
    freqm_t idx;
    uint8_t period_cnt;
} freqm_obj;

struct _sync_ctx {
    mutex_t lock;   /**< Mutex for blocking till measurement is done */
    uint32_t hz;    /**< Measured frequency in Hz */
    bool overflow;  /**< Overflow in FREQM counter */
};

/**
 * @brief   Mutex for locking the FREQM device
 */
static mutex_t msr_lock = MUTEX_INIT;

static void _freqm_enable(uint8_t refnum)
{
    mutex_lock(&msr_lock);

    /* Save refnum for frequency calculation */
    freqm_obj.period_cnt = refnum;

    FREQM->CFGA.reg = (uint16_t)(FREQM_CFGA_REFNUM(refnum));

    /* Enable DONE Interrupt */
    FREQM->INTENSET.reg = FREQM_INTENSET_DONE_Msk;

    /* Enable FREQM */
    FREQM->CTRLA.reg = FREQM_CTRLA_ENABLE_Msk;

    /* Wait for Sync */
    while ((FREQM->SYNCBUSY.reg) != 0U) {}
}

static void _freqm_disable(void)
{
    /* Disable DONE Interrupt */
    FREQM->INTENCLR.reg = FREQM_INTENCLR_MASK;
    /* Disable FREQM */
    FREQM->CTRLA.reg &=  ~FREQM_CTRLA_ENABLE_Msk;
    /* Wait for Sync */
    while ((FREQM->SYNCBUSY.reg) != 0U) {}

    mutex_unlock(&msr_lock);
}

bool _freqm_get_measurement(uint32_t *result)
{
    const freqm_config_t *config = &freqm_config[freqm_obj.idx];

    /* Calculate measured frequency */
    uint64_t result_tmp = FREQM->VALUE.reg * (uint64_t)(sam0_gclk_freq(config->gclk_src));

    result_tmp = result_tmp / freqm_obj.period_cnt;
    *result = (uint32_t)result_tmp;

    /* Read overflow status */
    bool overflow_condition = ((int)FREQM->STATUS.reg & FREQM_STATUS_OVF_Msk);

    /* Clear overflow status */
    FREQM->STATUS.reg = FREQM_STATUS_OVF_Msk;

    return overflow_condition;
}

uint32_t _us_to_ref_clock_counts(uint32_t period_us, uint8_t clock_id)
{
    uint64_t clk_cnt = (uint64_t)period_us * sam0_gclk_freq(clock_id) / US_PER_SEC;

    if (clk_cnt > UINT8_MAX) {
        return UINT8_MAX;
    }
    else if (clk_cnt == 0) {
        return 1;
    }
    else {
        return clk_cnt;
    }
}

static void _sync_cb(uint32_t res, bool overflow, void *_ctx)
{
    struct _sync_ctx *ctx = _ctx;

    ctx->hz = res;
    ctx->overflow = overflow;
    mutex_unlock(&ctx->lock);
}

int freqm_frequency_get(freqm_t idx, uint32_t *result, uint32_t period_us)
{
    struct _sync_ctx ctx = { .lock = MUTEX_INIT_LOCKED };

    /* Invoke non-blocking FREQM measure function */
    freqm_frequency_get_async(idx, _sync_cb, &ctx, period_us);

    /* Block until measurement is done */
    mutex_lock(&ctx.lock);

    *result = ctx.hz;
    return ctx.overflow ? -EOVERFLOW : 0;
}

void freqm_frequency_get_async(freqm_t idx, freqm_cb_t freqm_cb, void *context, uint32_t period_us)
{
    const freqm_config_t *config = &freqm_config[idx];

    uint8_t refnum = _us_to_ref_clock_counts(period_us, config->gclk_src);

    _freqm_enable(refnum);

    /* Register callback function */
    freqm_obj.callback = freqm_cb;
    freqm_obj.context = context;
    freqm_obj.idx = idx;

    /* Clear the Done Interrupt flag */
    FREQM->INTFLAG.reg = FREQM_INTFLAG_DONE_Msk;

    /* Start measurement */
    FREQM->CTRLB.reg = FREQM_CTRLB_START_Msk;
}

void irq_freqm(void)
{
    /* Clear the Done Interrupt flag */
    FREQM->INTFLAG.reg = FREQM_INTFLAG_DONE_Msk;

    uint32_t result = 0;

    bool overflow_condition = _freqm_get_measurement(&result);

    /* Invoke the callback function */
    freqm_obj.callback(result, overflow_condition, freqm_obj.context);

    _freqm_disable();
}

void freqm_init(freqm_t idx)
{
    uint8_t gclk_io_id = 0;
    const freqm_config_t *config = &freqm_config[idx];

    /* Sanity check configuration */
    assert(config->gclk_src <= GCLK_GEN_NUM_MSB);

    _freqm_gpio_init(config->pin, &gclk_io_id);
    _freqm_clock_init(gclk_io_id, config->gclk_src);

    /* Enable interrupt */
    NVIC_EnableIRQ(FREQM_IRQn);
}

/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of the peripheral PDM interface using the DFSDM
 *              module
 *
 * @author      Ben Wehrberger <ben.wehrberger@haw-hamburg.de>
 *
 * @}
 */

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

#include "board.h"
#include "container.h"
#include "imath.h"
#include "macros/utils.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "periph/gpio.h"
#include "periph/pdm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Sinc4 or Sinc5 for audio (AN5027 table 14), Sinc4 allows larger ratio, so
 * lower sample rates. PDM_RATIO_MAX depends on it, change both together */
#define SINC_ORDER 4

/* max decimation ratio (FOSR) for Sinc4 (RM0351 24.4.8) */
#define PDM_RATIO_MAX 215

/* lower bound of typical decimation range for PDM mics (AN5027 1.4.3) */
#define PDM_RATIO_MIN 48

/* clock range of the microphone, depends on the microphone used, so there is no
 * default */
#if !defined(PDM_MIC_CLK_MIN_HZ) || !defined(PDM_MIC_CLK_MAX_HZ)
#  error "PDM: Define PDM_MIC_CLK_MIN_HZ and PDM_MIC_CLK_MAX_HZ for the microphone used."
#endif

/* CKOUT divider range of the DFSDM module
 * - min: DFSDM clock must be at least 4x the PDM clock (RM0351 24.4.3)
 * - max: divider max 256 (RM0351 24.7.1) */
#define PDM_MODULE_DIV_MIN 4
#define PDM_MODULE_DIV_MAX 256

/* divider range of the mic, rounded so the clock stays inside its range */
#define PDM_MIC_DIV_MIN ((CLOCK_APB2 + PDM_MIC_CLK_MAX_HZ - 1) / PDM_MIC_CLK_MAX_HZ)
#define PDM_MIC_DIV_MAX (CLOCK_APB2 / PDM_MIC_CLK_MIN_HZ)

/* divider range used by the driver: overlap of module and mic range */
#define PDM_CLK_DIV_MIN MAX(PDM_MODULE_DIV_MIN, PDM_MIC_DIV_MIN)
#define PDM_CLK_DIV_MAX MIN(PDM_MODULE_DIV_MAX, PDM_MIC_DIV_MAX)

#if PDM_CLK_DIV_MIN > PDM_CLK_DIV_MAX
#  error "PDM: No usable PDM clock, check PDM_MIC_CLK_MIN_HZ and PDM_MIC_CLK_MAX_HZ."
#endif

#define PDM_SAMPLE_RATE_MIN (CLOCK_APB2 / (PDM_CLK_DIV_MAX * PDM_RATIO_MAX))
#define PDM_SAMPLE_RATE_MAX (CLOCK_APB2 / (PDM_CLK_DIV_MIN * PDM_RATIO_MIN))

/* max of 24 bit signed RDATA field */
#define PDM_RDATA_MAX ((1 << 23) - 1)

/* the samples buffer is a double buffer */
static int16_t _pdm_buf[PDM_BUF_SIZE * 2] = { 0 };

static pdm_isr_ctx_t _isr_ctx;
static bool _pdm_running = false;

static inline DFSDM_Filter_TypeDef *_filter(unsigned filter)
{
    uintptr_t size = DFSDM1_Filter1_BASE - DFSDM1_Filter0_BASE;

    return (DFSDM_Filter_TypeDef *)(DFSDM1_Filter0_BASE + filter * size);
}

static inline DFSDM_Channel_TypeDef *_channel(unsigned chan)
{
    uintptr_t size = DFSDM1_Channel1_BASE - DFSDM1_Channel0_BASE;

    return (DFSDM_Channel_TypeDef *)(DFSDM1_Channel0_BASE + chan * size);
}

/* called on half and full transfer */
static void _dma_cb(void *arg)
{
    (void)arg;

    /* more than half left: DMA wrapped and is back in the first half, so second
     * half is ready */
    uint8_t half = dma_get_remaining(pdm_config.dma) > PDM_BUF_SIZE ? 1 : 0;
    _isr_ctx.cb(_isr_ctx.arg, &_pdm_buf[PDM_BUF_SIZE * half]);
}

static void _start_dma(void)
{
    dma_acquire(pdm_config.dma);

    /* sample is 24 bit left aligned in FLTRDATAR, API wants 16 bit, so read
     * only upper half word */
    void *periph_addr = (uint8_t *)&_filter(pdm_config.filter)->FLTRDATAR + 2;
    dma_setup(pdm_config.dma,
              pdm_config.dma_chan,
              periph_addr,
              DMA_PERIPH_TO_MEM,
              DMA_DATA_WIDTH_HALF_WORD,
              false);
    dma_prepare(pdm_config.dma, _pdm_buf, ARRAY_SIZE(_pdm_buf), true);
    dma_enable_loop(pdm_config.dma);
    dma_set_cb(pdm_config.dma, _dma_cb, NULL);
    dma_start(pdm_config.dma);
}

static void _stop_dma(void)
{
    dma_stop(pdm_config.dma);
    dma_disable_loop(pdm_config.dma);
    dma_release(pdm_config.dma);
}

static void _enable_dfsdm(void)
{
    /* global enable, only in channel 0 */
    DFSDM1_Channel0->CHCFGR1 |= DFSDM_CHCFGR1_DFSDMEN;

    _channel(pdm_config.chan)->CHCFGR1 |= DFSDM_CHCFGR1_CHEN;
    _filter(pdm_config.filter)->FLTCR1 |= DFSDM_FLTCR1_DFEN;
    _filter(pdm_config.filter)->FLTCR1 |= DFSDM_FLTCR1_RSWSTART;
}

static void _disable_dfsdm(void)
{
    _filter(pdm_config.filter)->FLTCR1 &= ~DFSDM_FLTCR1_DFEN;
    _channel(pdm_config.chan)->CHCFGR1 &= ~DFSDM_CHCFGR1_CHEN;
    DFSDM1_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_DFSDMEN;
}

static uint16_t _rate_to_ratio(uint32_t rate, uint16_t div)
{
    uint32_t pdm_clk_hz = CLOCK_APB2 / div;
    uint32_t ratio = (pdm_clk_hz + (rate / 2)) / rate;

    if (ratio < PDM_RATIO_MIN) {
        ratio = PDM_RATIO_MIN;
    }
    if (ratio > PDM_RATIO_MAX) {
        ratio = PDM_RATIO_MAX;
    }
    return ratio;
}

/* filter output at full scale input is ratio^SINC_ORDER (RM0351 table 163)
 * return smallest right shift to fit it into 24 bit result (RDATA) */
static uint8_t _ratio_to_rshift(uint16_t ratio)
{
    uint32_t full_scale = powi(ratio, SINC_ORDER);
    uint8_t rshift = 0;

    while (full_scale > PDM_RDATA_MAX) {
        rshift++;
        full_scale >>= 1;
    }
    return rshift;
}

static uint32_t _set_best_pdm_rate(uint32_t desired_rate)
{
    if (desired_rate < PDM_SAMPLE_RATE_MIN) {
        desired_rate = PDM_SAMPLE_RATE_MIN;
    }
    if (desired_rate > PDM_SAMPLE_RATE_MAX) {
        desired_rate = PDM_SAMPLE_RATE_MAX;
    }

    uint16_t best_div = PDM_CLK_DIV_MIN;
    uint32_t smallest_diff = UINT32_MAX;

    /* find div and ratio for rate nearest to desired_rate */
    for (uint16_t div = PDM_CLK_DIV_MIN; div <= PDM_CLK_DIV_MAX; div++) {
        uint16_t ratio = _rate_to_ratio(desired_rate, div);
        uint32_t rate = CLOCK_APB2 / (div * ratio);
        uint32_t diff = ABS((int32_t)desired_rate - (int32_t)rate);

        if (diff < smallest_diff) {
            smallest_diff = diff;
            best_div = div;
        }
    }

    uint16_t ratio = _rate_to_ratio(desired_rate, best_div);
    DFSDM_Filter_TypeDef *filter = _filter(pdm_config.filter);

    /* CKOUT = CLOCK_APB2 / (CKOUTDIV + 1) */
    DFSDM1_Channel0->CHCFGR1 |= (best_div - 1) << DFSDM_CHCFGR1_CKOUTDIV_Pos;

    /* sample rate = CKOUT / (FOSR + 1) */
    filter->FLTFCR |= ((ratio - 1) << DFSDM_FLTFCR_FOSR_Pos) |
                      ((uint32_t)SINC_ORDER << DFSDM_FLTFCR_FORD_Pos);

    uint8_t rshift = _ratio_to_rshift(ratio);
    _channel(pdm_config.chan)->CHCFGR2 |= rshift << DFSDM_CHCFGR2_DTRBS_Pos;

    uint32_t rate = CLOCK_APB2 / (best_div * ratio);
    DEBUG("[PDM] sample rate = %" PRIu32 " Hz, div %u, ratio %u, rshift %u\n",
          rate, best_div, ratio, rshift);

    return rate;
}

int32_t pdm_init(pdm_mode_t mode, uint32_t rate, int8_t gain,
                 pdm_data_cb_t cb, void *arg)
{
    switch (mode) {
    case PDM_MODE_MONO:
        break;
    case PDM_MODE_STEREO:
        DEBUG("[PDM] init: stereo not supported\n");
        return -ENOTSUP;
    default:
        DEBUG("[PDM] init: mode not supported\n");
        return -ENOTSUP;
    }

    /* DFSDM has no gain stage, so gain is ignored */
    if (gain != 0) {
        DEBUG("[PDM] init: gain not supported by DFSDM\n");
    }

    /* init resets the DFSDM, so stop a running recording first */
    pdm_stop();

    /* use PCLK2 as DFSDM clock source */
    RCC->CCIPR &= ~RCC_CCIPR_DFSDM1SEL;
    periph_clk_en(APB2, RCC_APB2ENR_DFSDM1EN);

    /* reset registers */
    RCC->APB2RSTR |= RCC_APB2RSTR_DFSDM1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_DFSDM1RST;

    gpio_init(pdm_config.din_pin, GPIO_IN);
    gpio_init_af(pdm_config.din_pin, pdm_config.din_af);
    gpio_init(pdm_config.clk_pin, GPIO_OUT);
    gpio_init_af(pdm_config.clk_pin, pdm_config.clk_af);

    DFSDM_Channel_TypeDef *chan = _channel(pdm_config.chan);
    DFSDM_Filter_TypeDef *filter = _filter(pdm_config.filter);

    /* sample data on falling edge of own CKOUT */
    chan->CHCFGR1 |= DFSDM_CHCFGR1_SITP_0 | DFSDM_CHCFGR1_SPICKSEL_0;

    /* read channel continuously, FAST keeps filter state between samples,
     * request DMA for each result */
    filter->FLTCR1 |= (pdm_config.chan << DFSDM_FLTCR1_RCH_Pos) |
                      DFSDM_FLTCR1_RCONT |
                      DFSDM_FLTCR1_FAST |
                      DFSDM_FLTCR1_RDMAEN;

    _isr_ctx.cb = cb;
    _isr_ctx.arg = arg;

    uint32_t real_rate = _set_best_pdm_rate(rate);

    /* registers keep their values, clock only runs while recording */
    periph_clk_dis(APB2, RCC_APB2ENR_DFSDM1EN);

    return (int32_t)real_rate;
}

void pdm_start(void)
{
    if (_pdm_running) {
        return;
    }

    periph_clk_en(APB2, RCC_APB2ENR_DFSDM1EN);
    _start_dma();
    _enable_dfsdm();
    _pdm_running = true;
}

void pdm_stop(void)
{
    if (!_pdm_running) {
        return;
    }

    _disable_dfsdm();
    _stop_dma();
    periph_clk_dis(APB2, RCC_APB2ENR_DFSDM1EN);
    _pdm_running = false;
}

void pdm_deinit(void)
{
    /* pdm_stop already releases clock and DMA, nothing more to do */
    pdm_stop();
}

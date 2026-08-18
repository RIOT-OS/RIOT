/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-FileCopyrightText: 2024-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf52
 * @{
 *
 * @file
 * @brief       Implementation of the peripheral PDM interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Ben Wehrberger <ben.wehrberger@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>

#include "cpu.h"
#include "nrf_clock.h"
#include "periph/gpio.h"
#include "periph/pdm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define ABS_DIFF(x, y)          (((x) < (y)) ? ((y) - (x)) : ((x) - (y)))

#define PDM_SRC_CLOCK_HZ 32000000

/* PDM clock = PDM_SRC_CLOCK_HZ / divisor. Keeps the clock inside a typical
 * PDM MEMS mic clock range of [1.0, 3.25] MHz */
#define PDM_CLK_DIV_MAX 32
#define PDM_CLK_DIV_MIN 10

/* Derived from Nordics own PDMCLKCTRL presets (lower 22 bits are always zero)
 * not otherwise documented, confirmed against measured hardware behavior. */
#define PDM_CLK_POS 22

/*
 * nRF5340 PS formula: divisor = 2^32 / PDMCLKCTRL, where the register value
 * PDMCLKCTRL = bitfield << PDM_CLK_POS. Substituting:
 * divisor = 2^32 / (bitfield << 22) = 2^10 / bitfield
 * so bitfield = 1024 / divisor.
 *
 * Not documented for the nRF52XXX, applied here by analogy and confirmed
 * by tests for the nRF52840.
 */
#define PDM_DIV_BITFIELD_CONST (1U << (32 - PDM_CLK_POS))

#define PDM_RATIO_HIGH  80
#define PDM_RATIO_LOW   64

/* Achievable sample rate range, from divisor [10,32] x ratio {64,80} */
#define PDM_SAMPLE_RATE_MIN (PDM_SRC_CLOCK_HZ / (PDM_CLK_DIV_MAX * PDM_RATIO_HIGH))
#define PDM_SAMPLE_RATE_MAX (PDM_SRC_CLOCK_HZ / (PDM_CLK_DIV_MIN * PDM_RATIO_LOW))

/* The samples buffer is a double buffer */
static int16_t _pdm_buf[PDM_BUF_SIZE * 2] = { 0 };
static pdm_isr_ctx_t _isr_ctx;
static uint8_t _pdm_current_buf = 0;
static uint8_t _pdm_next_buf = 0;

static uint8_t _rate_to_divisor(uint32_t rate, uint8_t ratio)
{
    assert(ratio == 64 || ratio == 80);
    assert(rate >= PDM_SAMPLE_RATE_MIN);
    assert(rate <= PDM_SAMPLE_RATE_MAX);

    uint32_t pdm_clk_hz = rate * ratio;
    uint8_t divisor = (PDM_SRC_CLOCK_HZ + pdm_clk_hz / 2) / pdm_clk_hz;

    if (divisor < PDM_CLK_DIV_MIN) {
        divisor = PDM_CLK_DIV_MIN;
    }
    if (divisor > PDM_CLK_DIV_MAX) {
        divisor = PDM_CLK_DIV_MAX;
    }
    return divisor;
}

static uint32_t _set_best_pdm_rate(uint32_t rate)
{
    if (rate < PDM_SAMPLE_RATE_MIN) {
        rate = PDM_SAMPLE_RATE_MIN;
    }
    if (rate > PDM_SAMPLE_RATE_MAX) {
        rate = PDM_SAMPLE_RATE_MAX;
    }

    /* Calculate the divisor for ratio 80 given the desired sample rate. */
    uint8_t divisor_80 = _rate_to_divisor(rate, PDM_RATIO_HIGH);
    uint32_t real_rate_80 = PDM_SRC_CLOCK_HZ / (divisor_80 * PDM_RATIO_HIGH);
    uint32_t abs_diff_80 = ABS_DIFF(real_rate_80, rate);

    /* Do the same for ratio 64. */
    uint8_t divisor_64 = _rate_to_divisor(rate, PDM_RATIO_LOW);
    uint32_t real_rate_64 = PDM_SRC_CLOCK_HZ / (divisor_64 * PDM_RATIO_LOW);
    uint32_t abs_diff_64 = ABS_DIFF(real_rate_64, rate);

    /* Choose the ratio which gets closest to the desired sample rate. */
    if (abs_diff_80 <= abs_diff_64) {
        DEBUG("[PDM] sample rate = %" PRIu32 " Hz, ratio 80\n", real_rate_80);
        NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
        NRF_PDM->PDMCLKCTRL = ((PDM_DIV_BITFIELD_CONST / divisor_80) << PDM_CLK_POS);
        return real_rate_80;
    }
    else {
        DEBUG("[PDM] sample rate = %" PRIu32 " Hz, ratio 64\n", real_rate_64);
        NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio64 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
        NRF_PDM->PDMCLKCTRL = ((PDM_DIV_BITFIELD_CONST / divisor_64) << PDM_CLK_POS);
        return real_rate_64;
    }
}

int32_t pdm_init(pdm_mode_t mode, uint32_t rate, int8_t gain,
                 pdm_data_cb_t cb, void *arg)
{
    clock_hfxo_request();

    /* Configure sampling rate */
    uint32_t real_rate = _set_best_pdm_rate(rate);

    /* Configure mode (Mono or Stereo) */
    switch (mode) {
    case PDM_MODE_MONO:
        NRF_PDM->MODE = PDM_MODE_OPERATION_Mono;
        break;
    case PDM_MODE_STEREO:
        NRF_PDM->MODE = PDM_MODE_OPERATION_Stereo;
        break;
    default:
        DEBUG("[pdm] init: mode not supported\n");
        return -ENOTSUP;
    }

    /* Configure gain */
    if (gain > PDM_GAIN_MAX) {
        gain = PDM_GAIN_MAX;
    }

    if (gain < PDM_GAIN_MIN) {
        gain = PDM_GAIN_MIN;
    }

    /* Register uses 0.5 dB steps, with PDM_GAIN_MIN mapped to 0 */
    NRF_PDM->GAINR = (gain - PDM_GAIN_MIN) * 2;
    NRF_PDM->GAINL = (gain - PDM_GAIN_MIN) * 2;

    /* Configure CLK and DIN pins */
    gpio_init(pdm_config.clk_pin, GPIO_OUT);
    gpio_clear(pdm_config.clk_pin);
    gpio_init(pdm_config.din_pin, GPIO_IN);

    NRF_PDM->PSEL.CLK = pdm_config.clk_pin;
    NRF_PDM->PSEL.DIN = pdm_config.din_pin;

    /* clear pending events */
    NRF_PDM->EVENTS_STARTED = 0;
    NRF_PDM->EVENTS_STOPPED = 0;
    NRF_PDM->EVENTS_END = 0;

    /* Enable end/started/stopped events */
    NRF_PDM->INTENSET = ((PDM_INTEN_END_Enabled << PDM_INTEN_END_Pos) |
                         (PDM_INTEN_STARTED_Enabled << PDM_INTEN_STARTED_Pos) |
                         (PDM_INTEN_STOPPED_Enabled << PDM_INTEN_STOPPED_Pos));

    /* Configure Length of DMA RAM allocation in number of samples */
    NRF_PDM->SAMPLE.MAXCNT = (PDM_BUF_SIZE);

    _isr_ctx.cb = cb;
    _isr_ctx.arg = arg;

    /* enable interrupt */
    NVIC_EnableIRQ(PDM_IRQn);

    /* Enable PDM */
    NRF_PDM->ENABLE = (PDM_ENABLE_ENABLE_Enabled << PDM_ENABLE_ENABLE_Pos);

    return (int32_t)real_rate;
}

void pdm_start(void)
{
    _pdm_next_buf = 0;
    _pdm_current_buf = 0;
    NRF_PDM->SAMPLE.PTR = (uint32_t)_pdm_buf;
    DEBUG("[PDM] MAXCNT: %" PRIu32 "\n", NRF_PDM->SAMPLE.MAXCNT);

    NRF_PDM->TASKS_START = 1;
}

void pdm_stop(void)
{
    NRF_PDM->TASKS_STOP = 1;
}

void isr_pdm(void)
{
    /* new buffer started filling, prepare next buffer starting position */
    if (NRF_PDM->EVENTS_STARTED == 1) {
        NRF_PDM->EVENTS_STARTED = 0;

        _pdm_next_buf ^= 1;
        NRF_PDM->SAMPLE.PTR = (uint32_t)&_pdm_buf[_pdm_next_buf * (PDM_BUF_SIZE)];
    }

    /* PDM transfer has finished */
    if (NRF_PDM->EVENTS_STOPPED == 1) {
        NRF_PDM->EVENTS_STOPPED = 0;
    }

    /* requested number of samples written to RAM */
    if (NRF_PDM->EVENTS_END == 1) {
        NRF_PDM->EVENTS_END = 0;

        /* Process received samples frame */
        _isr_ctx.cb(_isr_ctx.arg, &_pdm_buf[_pdm_current_buf * (PDM_BUF_SIZE)]);

        /* Set next buffer */
        _pdm_current_buf ^= 1;
    }

    cortexm_isr_end();
}

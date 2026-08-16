/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf52
 * @{
 *
 * @file
 * @brief       Implementation of the peripheral PDM interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>

#include "container.h"
#include "cpu.h"
#include "nrf_clock.h"
#include "periph/gpio.h"
#include "periph/pdm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define ABS_DIFF(x, y)          (((x) < (y))? ((y) - (x)) : ((x) - (y)))

#define MAX_PDM_CLK_DIV 32
#define MIN_PDM_CLK_DIV 8
#define PDM_CLK_POS 22
#define MAX_PDM_CLK_BITFIELD (0x20000000 >> PDM_CLK_POS)
#define MIN_PDM_CLK_BITFIELD (0x08000000 >> PDM_CLK_POS)

#define PDM_SRC_CLOCK_HZ 32000000

#define PDM_RATIO_HIGH  80
#define PDM_RATIO_LOW   64

/* Achievable sample rate range, from divisor [8,32] x ratio {64,80} */
#define PDM_SAMPLE_RATE_MIN (PDM_SRC_CLOCK_HZ / (MAX_PDM_CLK_DIV * PDM_RATIO_HIGH))
#define PDM_SAMPLE_RATE_MAX (PDM_SRC_CLOCK_HZ / (MIN_PDM_CLK_DIV * PDM_RATIO_LOW))

/* The samples buffer is a double buffer */
static int16_t _pdm_buf[PDM_BUF_SIZE * 2] = { 0 };
static pdm_isr_ctx_t isr_ctx;
static uint8_t _pdm_current_buf = 0;
static uint8_t _pdm_next_buf = 0;

/*
 * Maps divisor (index + 8) to the PDMCLKCTRL bitfield (PDMCLKCTRL >> 22) that produces it
 * index i means: floor(1024 / LUT[i]) == i + 8
 * (from floor(2^32 / (LUT[i] << 22)) == i + 8, since 2^32 / 2^22 == 1024)
 *
 * Six divisors are the values Nordic names explicitly in the nRF52840 PS:
 *   0x08000000  PDM_CLK = 32 MHz / 32 = 1.000 MHz
 *   0x08400000  PDM_CLK = 32 MHz / 31 = 1.032 MHz
 *   0x08800000  PDM_CLK = 32 MHz / 30 = 1.067 MHz
 *   0x09800000  PDM_CLK = 32 MHz / 26 = 1.231 MHz
 *   0x0A000000  PDM_CLK = 32 MHz / 25 = 1.280 MHz
 *   0x0A800000  PDM_CLK = 32 MHz / 24 = 1.333 MHz
 * The rest are verified with the same formula, which the nRF52840 PS does not document
 * It is only confirmed for the nRF5340 PDM Interface and applied here by analogy.
 */
static const uint8_t DIV_TO_BITFIELD_LUT[] = {
    114,
    103,
    94,
    86,
    79,
    74,
    69,
    65,
    61,
    57,
    54,
    52,
    49,
    47,
    45,
    43,
    41,
    40,
    38,
    37,
    36,
    35,
    34,
    33,
    32,
};

static uint8_t _get_divisor(uint8_t bitfield)
{
    assert(bitfield >= MIN_PDM_CLK_BITFIELD);
    assert(bitfield <= MAX_PDM_CLK_BITFIELD);
    for (unsigned i = 0; i < ARRAY_SIZE(DIV_TO_BITFIELD_LUT); i++) {
        if (bitfield >= DIV_TO_BITFIELD_LUT[i]) {
            return i + 8;
        }
    }
    /* should never get here */
    assert(0);
    return 32;
}

static uint32_t _get_pdm_sample_rate(uint8_t bitfield, uint8_t ratio)
{
    assert(ratio != 0);

    return (uint32_t)PDM_SRC_CLOCK_HZ / _get_divisor(bitfield) / ratio;
}

static uint8_t _get_clk_bitfield(uint32_t rate, uint8_t ratio)
{
    assert(ratio != 0);
    assert(rate >= PDM_SAMPLE_RATE_MIN);
    assert(rate <= PDM_SAMPLE_RATE_MAX);

    /* We want to do some integer rounding to better approximate the desired
     * frequency. */
    uint8_t divisor = ((uint32_t)PDM_SRC_CLOCK_HZ + (rate * ratio) / 2) / (rate * ratio);

    /* Since the mapping of the bitfield is non-linear we will use the LUT
     * with some bounds... */
    if (divisor <= MIN_PDM_CLK_DIV) {
        return DIV_TO_BITFIELD_LUT[0];
    }
    if (divisor >= MAX_PDM_CLK_DIV) {
        return DIV_TO_BITFIELD_LUT[ARRAY_SIZE(DIV_TO_BITFIELD_LUT) - 1];
    }
    return DIV_TO_BITFIELD_LUT[divisor - MIN_PDM_CLK_DIV];
}

static uint32_t _set_best_pdm_rate(uint32_t rate)
{
    if (rate < PDM_SAMPLE_RATE_MIN) {
        rate = PDM_SAMPLE_RATE_MIN;
    }
    if (rate > PDM_SAMPLE_RATE_MAX) {
        rate = PDM_SAMPLE_RATE_MAX;
    }

    /* Calculate the bitfield for ratio 80 given the sample rate. */
    uint8_t bitfield_80 = _get_clk_bitfield(rate, PDM_RATIO_HIGH);
    uint32_t real_rate_80 = _get_pdm_sample_rate(bitfield_80, PDM_RATIO_HIGH);
    uint32_t abs_diff_80 = ABS_DIFF(real_rate_80, rate);

    /* Do the same for ratio 64. */
    uint8_t bitfield_64 = _get_clk_bitfield(rate, PDM_RATIO_LOW);
    uint32_t real_rate_64 = _get_pdm_sample_rate(bitfield_64, PDM_RATIO_LOW);
    uint32_t abs_diff_64 = ABS_DIFF(real_rate_64, rate);

    /* Choose the ratio which gets closest to the desired sample rate. */
    if (abs_diff_80 <= abs_diff_64) {
        DEBUG("[PDM] sample rate = %" PRIu32 " Hz, ratio 80\n", real_rate_80);
        NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
        NRF_PDM->PDMCLKCTRL = ((uint32_t)bitfield_80 << PDM_CLK_POS);
        return real_rate_80;
    }
    else {
        DEBUG("[PDM] sample rate = %" PRIu32 " Hz, ratio 64\n", real_rate_64);
        NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio64 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
        NRF_PDM->PDMCLKCTRL = ((uint32_t)bitfield_64 << PDM_CLK_POS);
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

    NRF_PDM->GAINR = (gain << 1) + 40;
    NRF_PDM->GAINL = (gain << 1) + 40;

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

    isr_ctx.cb = cb;
    isr_ctx.arg = arg;

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
        isr_ctx.cb(isr_ctx.arg, &_pdm_buf[_pdm_current_buf * (PDM_BUF_SIZE)]);

        /* Set next buffer */
        _pdm_current_buf ^= 1;
    }

    cortexm_isr_end();
}

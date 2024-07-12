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
#include "periph/gpio.h"
#include "periph/pdm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* The samples buffer is a double buffer */
int16_t _pdm_buf[PDM_BUF_SIZE * 2] = { 0 };
static uint8_t _pdm_current_buf = 0;
static pdm_isr_ctx_t isr_ctx;

#define MAX_PDM_CLK_DIV 32
#define MIN_PDM_CLK_DIV 8
#define PDM_CLK_POS 22
#define MAX_PDM_CLK_BITFIELD (0x20000000 >> PDM_CLK_POS)
#define MIN_PDM_CLK_BITFIELD (0x08000000 >> PDM_CLK_POS)

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

/* The clock speed that we can assume I guess? */
#define PDM_CLOCK_SPEED 32000000


uint8_t _get_divisor(uint8_t bitfield) {
    assert(bitfield >= MIN_PDM_CLK_BITFIELD);
    assert(bitfield <= MAX_PDM_CLK_BITFIELD);
    for (unsigned i = 0; i < ARRAY_SIZE(DIV_TO_BITFIELD_LUT); i++) {
        if (bitfield >= DIV_TO_BITFIELD_LUT[i] ) {
            return i + 8;
        }
    }
    /* should never get here */
    assert(0);
    return 32;
}

uint32_t _get_pdm_sample_rate(uint8_t divisor_tick, uint8_t ratio) {
    /* According to the datasheet, there is some sort of mask 22 bits shifted */
    /* Here are the examples from the datasheet... Needs a LUT */
    /* The divisor_tick is PDM_CLK >> 22. */
    /*  0x08000000 PDM_CLK = 32 MHz / 32 = 1.000 MHz
        0x08400000 PDM_CLK = 32 MHz / 31 = 1.032 MHz
        0x08800000 PDM_CLK = 32 MHz / 30 = 1.067 MHz
        0x09800000 PDM_CLK = 32 MHz / 26 = 1.231 MHz
        0x0A000000 PDM_CLK = 32 MHz / 25 = 1.280 MHz
        0x0A800000 PDM_CLK = 32 MHz / 24 = 1.333 MHz
    */
    assert(ratio != 0);

    return (uint32_t)PDM_CLOCK_SPEED / _get_divisor(divisor_tick) / ratio;
}

uint8_t _get_divisor_tick(uint32_t rate, uint8_t ratio) {
    assert(ratio != 0);
    /* Don't be dividing by zeros my friend*/
    if (rate == 0) {
        return DIV_TO_BITFIELD_LUT[ARRAY_SIZE(DIV_TO_BITFIELD_LUT) - 1];
    }
    /* We want to do some integer rounding to better approximate the desired */
    /* frequency. */
    uint8_t divisor = ((uint32_t)PDM_CLOCK_SPEED + (rate * ratio) / 2) / (rate * ratio);

    /* Since the mapping of the bitfield is non-linear we will use the LUT */
    /* with some bounds... */
    if (divisor <= MIN_PDM_CLK_DIV){
        return DIV_TO_BITFIELD_LUT[0];
    }
    if (divisor >= MAX_PDM_CLK_DIV) {
        return DIV_TO_BITFIELD_LUT[ARRAY_SIZE(DIV_TO_BITFIELD_LUT) - 1];
    }
    return DIV_TO_BITFIELD_LUT[divisor - MIN_PDM_CLK_DIV];
}

void _set_best_pdm_rate(uint32_t rate) {
    /* Calculate the divisor tick with with ratio 80 given the sample rate. */
    uint8_t divisor_tick_80;
    uint32_t real_rate_80;
    uint32_t abs_diff_80;
    divisor_tick_80 = _get_divisor_tick(rate, 80);
    real_rate_80 = _get_pdm_sample_rate(divisor_tick_80, 80);
    if (real_rate_80  < rate) {
        abs_diff_80 = -(real_rate_80 - rate);
    }
    else {
        abs_diff_80 = real_rate_80 - rate;
    }

    /* Now let's do the same for the 64 ratio. */
    uint8_t divisor_tick_64;
    uint32_t real_rate_64;
    uint32_t abs_diff_64;
    divisor_tick_64 = _get_divisor_tick(rate, 64);
    real_rate_64 = _get_pdm_sample_rate(divisor_tick_64, 64);
    if (real_rate_64 < rate) {
        abs_diff_64 = -(real_rate_64 - rate);
    }
    else {
        abs_diff_64 = real_rate_64 - rate;
    }

    /* One we have both, let's see which one is the best to use. */
    /* If we have more ratios, probably better to refactor this. */
    if (abs_diff_80 <= abs_diff_64) {
        DEBUG("[PDM] PDM_CLK = %lu Hz, ratio 80\n", real_rate_80);
        NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
        NRF_PDM->PDMCLKCTRL = ((uint32_t)divisor_tick_80 << PDM_CLK_POS);
    }
    else {
        DEBUG("[PDM] PDM_CLK = %lu Hz, ratio 64\n", real_rate_64);
        NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio64 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
        NRF_PDM->PDMCLKCTRL = ((uint32_t)divisor_tick_64 << PDM_CLK_POS);
    }
}

uint32_t get_pdm_sample_rate(void) {
    if (((NRF_PDM->RATIO & PDM_RATIO_RATIO_Msk) >> PDM_RATIO_RATIO_Pos) == PDM_RATIO_RATIO_Ratio80) {
        return _get_pdm_sample_rate(NRF_PDM->PDMCLKCTRL >> PDM_CLK_POS, 80);
    }
    if (((NRF_PDM->RATIO & PDM_RATIO_RATIO_Msk) >> PDM_RATIO_RATIO_Pos) == PDM_RATIO_RATIO_Ratio64) {
        return _get_pdm_sample_rate(NRF_PDM->PDMCLKCTRL >> PDM_CLK_POS, 64);
    }
    return 0;
}

int pdm_init(pdm_mode_t mode, uint32_t rate, int8_t gain, pdm_data_cb_t cb, void *arg)
{
    if (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
        NRF_CLOCK->TASKS_HFCLKSTART = 1;
        while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {}
    }

    /* Configure sampling rate */
    _set_best_pdm_rate(rate);

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

    return 0;
}

void pdm_start(void)
{
    NRF_PDM->SAMPLE.PTR = (uint32_t)_pdm_buf;
    DEBUG("[PDM] MAXCNT: %lu\n", NRF_PDM->SAMPLE.MAXCNT);

    NRF_PDM->TASKS_START = 1;
}

void pdm_stop(void)
{
    NRF_PDM->TASKS_STOP = 1;
}

void isr_pdm(void)
{
    /* PDM transfer has started */
    if (NRF_PDM->EVENTS_STARTED == 1) {
        NRF_PDM->EVENTS_STARTED = 0;
        uint8_t next_buf_pos = (_pdm_current_buf + 1) & 0x1;
        NRF_PDM->SAMPLE.PTR = (uint32_t)&_pdm_buf[next_buf_pos * (PDM_BUF_SIZE)];
    }

    /* PDM transfer has finished */
    if (NRF_PDM->EVENTS_STOPPED == 1) {
        NRF_PDM->EVENTS_STOPPED = 0;
        _pdm_current_buf = 0;
    }

    /* Address resolution procedure has completed*/
    if (NRF_PDM->EVENTS_END == 1) {
        NRF_PDM->EVENTS_END = 0;

        /* Process received samples frame */
        isr_ctx.cb(isr_ctx.arg, &_pdm_buf[_pdm_current_buf * (PDM_BUF_SIZE)]);

        /* Set next buffer */
        _pdm_current_buf = (_pdm_current_buf + 1) & 0x1;
    }

    cortexm_isr_end();
}

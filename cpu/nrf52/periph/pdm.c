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

#include "cpu.h"
#include "periph/gpio.h"
#include "periph/pdm.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* The samples buffer is a double buffer */
int16_t _pdm_buf[PDM_BUF_SIZE * 2] = { 0 };
static uint8_t _pdm_current_buf = 0;
static pdm_isr_ctx_t isr_ctx;

int pdm_init(pdm_mode_t mode, pdm_sample_rate_t rate, int8_t gain,
              pdm_data_cb_t cb, void *arg)
{
    if (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
        NRF_CLOCK->TASKS_HFCLKSTART = 1;
        while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {}
    }

    /* Configure sampling rate */
    switch (rate) {
        case PDM_SAMPLE_RATE_16KHZ:
#ifdef CPU_MODEL_NRF52840XXAA
            NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
            NRF_PDM->PDMCLKCTRL = PDM_PDMCLKCTRL_FREQ_1280K;
#else
            NRF_PDM->PDMCLKCTRL = PDM_PDMCLKCTRL_FREQ_Default; /* 1.033MHz */
#endif
            break;
        case PDM_SAMPLE_RATE_20KHZ:
            NRF_PDM->PDMCLKCTRL = 0x0A000000; /* CLK= 1.280 MHz (32 MHz / 25) => rate= 20000 Hz */
            break;
        case PDM_SAMPLE_RATE_41KHZ:
            NRF_PDM->PDMCLKCTRL = 0x15000000; /* CLK= 2.667 MHz (32 MHz / 12) => rate= 41667 Hz */
            break;
        case PDM_SAMPLE_RATE_50KHZ:
            NRF_PDM->PDMCLKCTRL = 0x19000000; /* CLK= 3.200 MHz (32 MHz / 10) => rate= 50000 Hz */
            break;
        case PDM_SAMPLE_RATE_60KHZ:
            NRF_PDM->PDMCLKCTRL = 0x20000000; /* CLK= 4.000 MHz (32 MHz / 8) => rate= 60000 Hz */
            break;
        default:
            DEBUG("[pdm] init: sample rate not supported\n");
            return -ENOTSUP;
    }

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

    /* Configure internal RAM buffer size, divide by 2 for stereo mode */
    NRF_PDM->SAMPLE.MAXCNT = (PDM_BUF_SIZE >> mode);

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
    if (NRF_PDM->EVENTS_STARTED == 1) {
        NRF_PDM->EVENTS_STARTED = 0;
        uint8_t next_buf_pos = (_pdm_current_buf + 1) & 0x1;
        NRF_PDM->SAMPLE.PTR = (uint32_t)&_pdm_buf[next_buf_pos * (PDM_BUF_SIZE >> 1)];
    }

    if (NRF_PDM->EVENTS_STOPPED == 1) {
        NRF_PDM->EVENTS_STOPPED = 0;
        _pdm_current_buf = 0;
    }

    if (NRF_PDM->EVENTS_END == 1) {
        NRF_PDM->EVENTS_END = 0;

        /* Process received samples frame */
        isr_ctx.cb(isr_ctx.arg, &_pdm_buf[_pdm_current_buf * (PDM_BUF_SIZE >> 1)]);

        /* Set next buffer */
        _pdm_current_buf = (_pdm_current_buf + 1) & 0x1;
    }

    cortexm_isr_end();
}

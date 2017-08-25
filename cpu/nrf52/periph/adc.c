/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf52
 * @{
 *
 * Any one of the available channels can be enabled for the ADC to operate in
 * one-shot mode. If more than one CH[n] is configured, the ADC enters scan mode.
 *
 * Example of RAM placement (RESULT.MAXCNT = 8), all channels enabled:
 *
 *                           31    16 15     0
 * RESULT.PTR = adc_val[0]   | CH[1] | CH[0] |
 * RESULT.PTR + 4            | CH[3] | CH[2] |
 * RESULT.PTR + 8            | CH[5] | CH[4] |
 * RESULT.PTR + 12           | CH[7] | CH[6] |
 *
 * Example of RAM placement (RESULT.MAXCNT = 4), channels 0, 3, 4 and 7 enabled:
 *
 *                           31    16 15     0
 * RESULT.PTR = adc_val[0]   | CH[3] | CH[0] |
 * RESULT.PTR + 4            | CH[7] | CH[4] |
 * RESULT.PTR + 8            |       |       |
 * RESULT.PTR + 12           |       |       |
 *
 * Scan mode and oversampling cannot be combined.
 * -> 8/10/12-bit resolution, 14-bit resolution only with oversampling
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"

#ifdef ADC_NUMOF

static int16_t adc_val[ADC_NUMOF];
static uint8_t adc_ch_enabled = 0;
static uint8_t adc_return_idx;

/**
 * @brief   Lock to prevent concurrency issues when used from different threads
 */
static mutex_t lock;

static inline void prep(void)
{
    mutex_lock(&lock);
    NRF_SAADC->ENABLE = 1;
}

static inline void done(void)
{
    NRF_SAADC->ENABLE = 0;
    mutex_unlock(&lock);
}

int adc_init(adc_t line)
{
    if (line >= ADC_NUMOF) {
        return -1;
    }

    if ((adc_ch_enabled & (1 << line)) == 0) {
        /* prepare device */
        prep();

        /* set the number of enabled channels and the data pointer */
        NRF_SAADC->RESULT.MAXCNT += 1;
        NRF_SAADC->RESULT.PTR = (uint32_t)&adc_val;

        /* set ADC channel and use VDD (+5V) as reference */
        NRF_SAADC->CH[line].PSELP = line + 1;
        NRF_SAADC->CH[line].CONFIG = (SAADC_CH_CONFIG_GAIN_Gain1_4 << SAADC_CH_CONFIG_RESN_Pos) |
                                  (SAADC_CH_CONFIG_REFSEL_VDD1_4 << SAADC_CH_CONFIG_REFSEL_Pos);

        /* calibrate the SAADC */
        NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
        NRF_SAADC->TASKS_CALIBRATEOFFSET = 1;
        while (NRF_SAADC->EVENTS_CALIBRATEDONE == 0) {}

        /* remember which ADC channel is enabled */
        adc_ch_enabled |= (1 << line);

        /* free device */
        done();
    }

    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
{
    assert(line < ADC_NUMOF);

    /* check if resolution is valid */
    if (res > 2) {
        return -1;
    }

    /* prepare device */
    prep();

    /* set resolution */
    NRF_SAADC->RESOLUTION = res;

    /* start the SAADC and wait for the started event */
    NRF_SAADC->EVENTS_STARTED = 0;
    NRF_SAADC->TASKS_START = 1;
    while (NRF_SAADC->EVENTS_STARTED == 0) {}

    /* start conversions and wait for conversions to be complete */
    for (uint8_t i = 0; i < NRF_SAADC->RESULT.MAXCNT; i++) {
        while (NRF_SAADC->STATUS == (SAADC_STATUS_STATUS_Busy << SAADC_STATUS_STATUS_Pos)) {}
        NRF_SAADC->EVENTS_DONE = 0;
        NRF_SAADC->TASKS_SAMPLE = 1;
        while (NRF_SAADC->EVENTS_DONE == 0) {}
    }

    /* stop the SAADC */
    NRF_SAADC->EVENTS_STOPPED = 0;
    NRF_SAADC->TASKS_STOP = 1;
    while (NRF_SAADC->EVENTS_STOPPED == 0) {}

    /* free device */
    done();

    /* return the ADC value for the given line */
    adc_return_idx = 0;
    for (uint8_t i = 0; i < line; i++) {
        if (adc_ch_enabled & (1 << i)) {
            adc_return_idx++;
        }
    }
    if (adc_val[adc_return_idx] < 0) {
        return 0;
    }
    return adc_val[adc_return_idx];
}

#else
typedef int dont_be_pedantic;
#endif /* ADC_CONFIG */

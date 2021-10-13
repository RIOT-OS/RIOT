/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Ludwig Knüpfer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf51
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"

/**
 * @brief   Lock to prevent concurrency issues when used from different threads
 */
static mutex_t lock;

static inline void prep(void)
{
    mutex_lock(&lock);
    NRF_ADC->POWER = 1;
    NRF_ADC->ENABLE = 1;
}

static inline void done(void)
{
    NRF_ADC->ENABLE = 0;
    NRF_ADC->POWER = 0;
    mutex_unlock(&lock);
}

int adc_init(adc_t line)
{
    if (line >= ADC_NUMOF) {
        return -1;
    }
    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    int val;

    /* check if resolution is valid */
    if (res > 2) {
        return -1;
    }

    /* prepare device */
    prep();

    /* set resolution, line, and use 1/3 input and ref voltage scaling */
    NRF_ADC->CONFIG = ((ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling << 5) |
                       (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << 2) |
                       (1 << (adc_config[line] + 8)) |
                       res);
    /* start conversion */
    NRF_ADC->TASKS_START = 1;
    /* wait for conversion to be complete */
    while (NRF_ADC->BUSY == 1) {}
    /* get result */
    val = (int)NRF_ADC->RESULT;

    /* free device */
    done();

    return val;
}

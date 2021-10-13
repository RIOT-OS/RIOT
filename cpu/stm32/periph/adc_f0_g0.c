/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"

/**
 * @brief   Allocate lock for the ADC device
 *
 * All STM32F0 & STM32G0 CPUs we support so far only come with a single ADC device.
 */
static mutex_t lock = MUTEX_INIT;

static inline void prep(void)
{
    mutex_lock(&lock);
#ifdef RCC_APB2ENR_ADCEN
    periph_clk_en(APB2, RCC_APB2ENR_ADCEN);
#endif
#ifdef RCC_APBENR2_ADCEN
    periph_clk_en(APB12, RCC_APBENR2_ADCEN);
#endif
}

static inline void done(void)
{
#ifdef RCC_APB2ENR_ADCEN
    periph_clk_dis(APB2, RCC_APB2ENR_ADCEN);
#endif
#ifdef RCC_APBENR2_ADCEN
    periph_clk_dis(APB12, RCC_APBENR2_ADCEN);
#endif
    mutex_unlock(&lock);
}

int adc_init(adc_t line)
{
    /* make sure the given line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* lock and power on the device */
    prep();
    /* configure the pin */
    gpio_init_analog(adc_config[line].pin);
    /* reset configuration */
    ADC1->CFGR2 = 0;
    /* enable device */
    ADC1->CR = ADC_CR_ADEN;
    /* configure sampling time to save value */
    ADC1->SMPR = 0x3;       /* 28.5 ADC clock cycles */
    /* power off an release device for now */
    done();

    return 0;
}

int32_t adc_sample(adc_t line,  adc_res_t res)
{
    int sample;

    /* check if resolution is applicable */
    if (res > 0xf0) {
        return -1;
    }

    /* lock and power on the ADC device  */
    prep();

    /* set resolution and channel */
    ADC1->CFGR1 = res;
    ADC1->CHSELR = (1 << adc_config[line].chan);
    /* start conversion and wait for results */
    ADC1->CR |= ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC)) {}
    /* read result */
    sample = (int)ADC1->DR;

    /* unlock and power off device again */
    done();

    return sample;
}

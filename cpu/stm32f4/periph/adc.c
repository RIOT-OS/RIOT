/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f4
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
#include "periph/adc.h"
#include "periph_conf.h"

static inline ADC_TypeDef *dev(adc_t line)
{
    return adc_config[line].dev;
}

static inline void poweron(adc_t line)
{
    RCC->APB2ENR |= (1 << adc_config[line].rcc);
}

static inline void poweroff(adc_t line)
{
    RCC->APB2ENR &= ~(1 << adc_config[line].rcc);
}

int adc_init(adc_t line)
{
    /* check if the linenel is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }
    /* configure the pin */
    gpio_init_analog(adc_config[line].pin);
    /* power on the device for the duration of configuration */
    poweron(line);
    /* set sequence length to 1 conversion and enable the ADC device */
    dev(line)->SQR1 = 0;
    dev(line)->CR2 = ADC_CR2_ADON;
    /* set clock prescaler */
    ADC->CCR = (3 << 16);       /* ADC clock = 10,5MHz */
    /* power off the device again */
    poweroff(line);
    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
{
    int sample;

    /* check if resolution is applicable */
    if (res == ADC_RES_14BIT || res == ADC_RES_16BIT) {
        return -2;
    }
    /* power device on for sampling */
    poweron(line);
    /* wait for any ongoing conversions to finish */
    while (!(dev(line)->SR & ADC_SR_STRT));
    /* set resolution and conversion channel */
    dev(line)->CR1 = res;
    dev(line)->SQR3 = adc_config[line].chan;
    /* start conversion and wait for results */
    dev(line)->CR2 |= ADC_CR2_SWSTART;
    while (!(dev(line)->SR & ADC_SR_EOC));
    /* finally read sample and reset the STRT bit in the status register */
    sample = (int)dev(line)->DR;
    dev(line)->SR &= ~ADC_SR_STRT;
    /* power off device again */
    poweroff(line);
    return sample;
}

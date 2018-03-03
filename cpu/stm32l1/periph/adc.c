/*
 * Copyright (C) 2016 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l1
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Francisco Molina <francisco.molina@inria.cl>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"

/**
 * @brief   Maximum allowed ADC clock speed
 */
#define MAX_ADC_SPEED           (12000000U)

/**
 * @brief   Load the ADC configuration
 */
static const adc_conf_t adc_config[] = ADC_CONFIG;

/**
 * @brief   Allocate locks for all three available ADC device
 *
 * All STM32l1 CPU's have single ADC device
 */
static mutex_t lock = MUTEX_INIT;

static inline void prep(void)
{
    mutex_lock(&lock);
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}

static inline void done(void)
{
    RCC->APB2ENR &= ~(RCC_APB2ENR_ADC1EN);
    mutex_unlock(&lock);
}

int adc_init(adc_t line)
{
    uint32_t clk_div = 2;

    /* check if the line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* lock and power-on the device */
    prep();

    /* configure the pin */
    if ((adc_config[line].pin != GPIO_UNDEF))
        gpio_init_analog(adc_config[line].pin);

    /* set clock prescaler to get the maximal possible ADC clock value */
    for (clk_div = 2; clk_div < 8; clk_div += 2) {
        if ((CLOCK_CORECLOCK / clk_div) <= MAX_ADC_SPEED) {
            break;
        }
    }

    ADC->CCR = ((clk_div / 2) - 1) << 16;

    /* check if this channel is an internal ADC channel, if so
     * enable the internal temperature and Vref */
    if (adc_config[line].chan == 16 || adc_config[line].chan == 17) {
        ADC->CCR |= ADC_CCR_TSVREFE;
    }

    /* enable the ADC module */
    ADC1->CR2 = ADC_CR2_ADON;
    /* turn off during idle phase*/
    ADC1->CR1 = ADC_CR1_PDI;

    /* free the device again */
    done();

    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
{
    int sample;

    /* check if resolution is applicable */
    if ( (res != ADC_RES_6BIT) &&
         (res != ADC_RES_8BIT) &&
         (res != ADC_RES_10BIT) &&
         (res != ADC_RES_12BIT)) {
        return -1;
    }

    /* lock and power on the ADC device  */
    prep();

    /* set resolution, conversion channel and single read */
    ADC1->CR1 |= res & ADC_CR1_RES;
    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR5 = adc_config[line].chan;

    /* wait for regulat channel to be ready*/
    while (!(ADC1->SR & ADC_SR_RCNR)) {}
    /* start conversion and wait for results */
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_EOC)) {}
    /* finally read sample and reset the STRT bit in the status register */
    sample = (int)ADC1->DR;
    ADC1 -> SR &= ~ADC_SR_STRT;

    /* power off and unlock device again */
    done();

    return sample;
}

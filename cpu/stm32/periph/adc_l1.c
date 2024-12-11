/*
 * Copyright (C) 2016 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
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
 * @brief   ADC clock settings
 *
 * NB: with ADC_CLOCK_HIGH, Vdda should be 2.4V min
 *
 */
#define ADC_CLOCK_HIGH      (0)
#define ADC_CLOCK_MEDIUM    (ADC_CCR_ADCPRE_0)
#define ADC_CLOCK_LOW       (ADC_CCR_ADCPRE_1)

/**
 * @brief   ADC sample time, cycles
 */
#define ADC_SAMPLE_TIME_4C    (0)
#define ADC_SAMPLE_TIME_9C    (1)
#define ADC_SAMPLE_TIME_16C   (2)
#define ADC_SAMPLE_TIME_24C   (3)
#define ADC_SAMPLE_TIME_48C   (4)
#define ADC_SAMPLE_TIME_96C   (5)
#define ADC_SAMPLE_TIME_192C  (6)
#define ADC_SAMPLE_TIME_384C  (7)

/**
 * @brief   Allocate locks for all three available ADC device
 *
 * All STM32l1 CPU's have single ADC device
 */
static mutex_t lock = MUTEX_INIT;

static inline void prep(void)
{
    mutex_lock(&lock);

    /* ADC clock is always HSI clock */
    if (!(RCC->CR & RCC_CR_HSION)) {
        RCC->CR |= RCC_CR_HSION;
        /* Wait for HSI to become ready */
        while (!(RCC->CR & RCC_CR_HSION)) {}
    }

    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}

static inline void done(void)
{
    RCC->APB2ENR &= ~(RCC_APB2ENR_ADC1EN);
    mutex_unlock(&lock);
}

static void adc_set_sample_time(uint8_t time)
{
    uint8_t i;
    uint32_t reg32 = 0;

    for (i = 0; i <= 9; i++) {
        reg32 |= (time << (i * 3));
    }

#if defined(CPU_MODEL_STM32L152RE)
    ADC1->SMPR0 = reg32;
#endif
    ADC1->SMPR1 = reg32;
    ADC1->SMPR2 = reg32;
    ADC1->SMPR3 = reg32;
}

int adc_init(adc_t line)
{
    /* check if the line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* lock and power-on the device */
    prep();

    /* configure the pin */
    if ((adc_config[line].pin != GPIO_UNDEF))
        gpio_init_analog(adc_config[line].pin);

    /* set ADC clock prescaler */
    ADC->CCR &= ~ADC_CCR_ADCPRE;
    ADC->CCR |= ADC_CLOCK_MEDIUM;

    /* Set sample time */
    /* Min 4us needed for temperature sensor measurements */
    switch (ADC->CCR & ADC_CCR_ADCPRE) {
        case ADC_CLOCK_LOW:
            /* 4 MHz ADC clock -> 16 cycles */
            adc_set_sample_time(ADC_SAMPLE_TIME_16C);
            break;
        case ADC_CLOCK_MEDIUM:
            /* 8 MHz ADC clock -> 48 cycles */
            adc_set_sample_time(ADC_SAMPLE_TIME_48C);
            break;
        default:
            /* 16 MHz ADC clock -> 96 cycles */
            adc_set_sample_time(ADC_SAMPLE_TIME_96C);
    }

    /* check if this channel is an internal ADC channel, if so
     * enable the internal temperature and Vref */
    if (adc_config[line].chan == 16 || adc_config[line].chan == 17) {
        ADC->CCR |= ADC_CCR_TSVREFE;
    }

    /* turn off during idle phase*/
    ADC1->CR1 = ADC_CR1_PDI;

    /* free the device again */
    done();

    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    int sample;

    /* check if resolution is applicable */
    if ((res & ADC_CR1_RES) != res) {
        return -1;
    }

    /* lock and power on the ADC device  */
    prep();

    /* mask and set resolution, conversion channel and single read */
    ADC1->CR1 = (ADC1->CR1 & ~ADC_CR1_RES) | (res & ADC_CR1_RES);
    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR5 = adc_config[line].chan;

    /* only set ADON when ADONS bit is cleared (ADC not ready) */
    if (!(ADC1->SR & ADC_SR_ADONS)) {
        ADC1->CR2 |= ADC_CR2_ADON;
    }

    /* wait for regular channel to be ready*/
    while (!(ADC1->SR & ADC_SR_RCNR)) {}
    /* start conversion and wait for results */
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_EOC)) {}
    /* finally read sample and reset the STRT bit in the status register */
    sample = (int)ADC1->DR;
    ADC1 -> SR &= ~ADC_SR_STRT;

    /* wait for ADC to become ready before disabling it */
    while (!(ADC1->SR & ADC_SR_ADONS)) {}
    ADC1->CR2 &= ~ADC_CR2_ADON;

    /* power off and unlock device again */
    done();

    return sample;
}

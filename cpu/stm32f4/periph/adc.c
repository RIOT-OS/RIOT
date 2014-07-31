/*
 * Copyright (C) 2014 Freie Universität Berlin
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

/* guard in case that no ADC device is defined */
#if ADC_NUMOF

typedef struct {
    int max_value;
} adc_config_t;

adc_config_t adc_config[ADC_NUMOF];

int adc_init(adc_t dev, adc_precision_t precision)
{
    ADC_TypeDef *adc = 0;

    adc_poweron(dev);

    switch (dev) {
#if ADC_0_EN
        case ADC_0:
            adc = ADC_0_DEV;
            ADC_0_PORT_CLKEN();
            ADC_0_PORT->MODER |= (3 << (ADC_0_CH0_PIN * 2) | 3 << (ADC_0_CH1_PIN * 2));
            break;
#endif
#if ADC_1_EN
        case ADC_1:
            adc = ADC_1_DEV;
            ADC_1_PORT_CLKEN();
            ADC_1_PORT->MODER |= (3 << (ADC_1_CH0_PIN * 2) | 3 << (ADC_1_CH1_PIN * 2));
            break;
#endif
        default:
            return -1;
    }

    /* reset control registers */
    adc->CR1 = 0;
    adc->CR2 = 0;
    adc->SQR1 = 0;

    /* set precision */

    switch (precision) {
        case ADC_RES_6BIT:
            adc->CR1 |= ADC_CR1_RES_0 | ADC_CR1_RES_1;
            adc_config[dev].max_value = 0x3f;
            break;
        case ADC_RES_8BIT:
            adc->CR1 |= ADC_CR1_RES_1;
            adc_config[dev].max_value = 0xff;
            break;
        case ADC_RES_10BIT:
            adc->CR1 |= ADC_CR1_RES_0;
            adc_config[dev].max_value = 0x3ff;
            break;
        case ADC_RES_12BIT:
            adc_config[dev].max_value = 0xfff;
            break;
        case ADC_RES_14BIT:
        case ADC_RES_16BIT:
            adc_poweroff(dev);
            return -1;
            break;
    }

    /* set clock prescaler */
    ADC->CCR = (3 << 16);       /* ADC clock = 10,5MHz */

    /* enable the ADC module */
    adc->CR2 |= ADC_CR2_ADON;

    return 0;
}

int adc_sample(adc_t dev, int channel)
{
    ADC_TypeDef *adc = 0;

    switch (dev) {
#if ADC_0_EN
        case ADC_0:
            adc = ADC_0_DEV;
            switch (channel) {
                case 0:
                    adc->SQR3 = ADC_0_CH0 & 0x1f;
                    break;
                case 1:
                    adc->SQR3 = ADC_0_CH1 & 0x1f;
                    break;
                default:
                    return -1;
            }
            break;
#endif
#if ADC_1_EN
        case ADC_1:
            adc = ADC_1_DEV;
            switch (channel) {
                case 0:
                    adc->SQR3 = ADC_1_CH0 & 0x1f;
                    break;
                case 1:
                    adc->SQR3 = ADC_1_CH1 & 0x1f;
                    break;
                default:
                    return -1;
            }
            break;
#endif
    }

    /* start single conversion */
    adc->CR2 |= ADC_CR2_SWSTART;
    /* wait until conversion is complete */
    while (!(adc->SR & ADC_SR_EOC));
    /* read and return result */
    return (int)adc->DR;
}

void adc_poweron(adc_t dev)
{
    switch (dev) {
#if ADC_0_EN
        case ADC_0:
            ADC_0_CLKEN();
            break;
#endif
#if ADC_1_EN
        case ADC_1:
            ADC_1_CLKEN();
            break;
#endif
    }
}

void adc_poweroff(adc_t dev)
{
    switch (dev) {
#if ADC_0_EN
        case ADC_0:
            ADC_0_CLKDIS();
            break;
#endif
#if ADC_1_EN
        case ADC_1:
            ADC_1_CLKDIS();
            break;
#endif
    }
}

int adc_map(adc_t dev, int value, int min, int max)
{
    return (int)adc_mapf(dev, value, (float)min, (float)max);
}

float adc_mapf(adc_t dev, int value, float min, float max)
{
    return ((max - min) / ((float)adc_config[dev].max_value)) * value;
}

#endif /* ADC_NUMOF */

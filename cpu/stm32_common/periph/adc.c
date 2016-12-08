/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 *               2016 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @{
 *
 * @file
 * @brief       ADC peripheral driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/adc.h"

#if defined(ADC_NUMOF) && !defined(CPU_FAM_STM32F0)

#define BASEADDR                (ADC1_BASE)
#define DEVOFFSET               (8)

#if defined(CPU_FAM_STM32L1)
#define WRITE_LINE(x)           (dev(line)->SQR5 = x)
#else
#define WRITE_LINE(x)           (dev(line)->SQR3 = x)
#endif

/**
 * @brief   Maximum allowed ADC clock speed
 */
#define MAX_ADC_SPEED           (12000000U)

/**
 * @brief   Allocate one mutex for each ADC unit
 */
static mutex_t locks[ADC_DEV_NUMOF] = { MUTEX_INIT };

static inline ADC_TypeDef *dev(adc_t line)
{
    return (ADC_TypeDef *)(BASEADDR + (adc_config[line].dev * DEVOFFSET));
}

static inline void prep(adc_t line)
{
    mutex_lock(&locks[adc_config[line].dev]);
    periph_clk_en(APB2, (RCC_APB2ENR_ADC1EN << adc_config[line].dev));
}

static inline void done(adc_t line)
{
    periph_clk_dis(APB2, (RCC_APB2ENR_ADC1EN << adc_config[line].dev));
    mutex_unlock(&locks[adc_config[line].dev]);
}

int adc_init(adc_t line)
{
    uint32_t clk_div;

    /* check if the line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* lock and power-on the device */
    prep(line);

    /* configure the pin (if one is defined) */
    if (adc_config[line].pin != GPIO_UNDEF) {
        gpio_init_analog(adc_config[line].pin);
    }
    /* set clock prescaler to get the maximal possible ADC clock value */
    for (clk_div = 2; clk_div < 8; clk_div += 2) {
        if ((CLOCK_CORECLOCK / clk_div) <= MAX_ADC_SPEED) {
            break;
        }
    }
    ADC->CCR = ((clk_div / 2) - 1) << 16;

    /* set sequence length to 1 conversion and enable the ADC device */
    dev(line)->SQR1 = 0;
    dev(line)->CR2 = ADC_CR2_ADON;

    /* check if this channel is an internal ADC channel, if so
     * enable the internal temperature and Vref */
    if (adc_config[line].chan == 16 || adc_config[line].chan == 17) {
        /* check if the internal channels are configured to use ADC1 */
        if (dev(line) != ADC1) {
            return -1;
        }

        ADC->CCR |= ADC_CCR_TSVREFE;
    }

    /* free the device again */
    done(line);
    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
{
    int sample;

    assert(line < ADC_NUMOF);

    /* check if resolution is applicable */
    if (res & ~(ADC_RES_6BIT)) {
        return -1;
    }

    /* lock and power on the ADC device  */
    prep(line);

    /* set resolution and conversion channel */
    dev(line)->CR1 = res;
    WRITE_LINE(adc_config[line].chan);

#ifdef CPU_FAM_STM32L1
    /* wait for regular channel to be ready */
    while (!(dev(line)->SR & ADC_SR_RCNR)) {}
#endif

    /* start conversion and wait for results */
    dev(line)->CR2 |= ADC_CR2_SWSTART;
    while (!(dev(line)->SR & ADC_SR_EOC)) {}
    /* finally read sample and reset the STRT bit in the status register */
    sample = (int)dev(line)->DR;

    /* power off and unlock device again */
    done(line);

    return sample;
}

#endif /* ADC_NUMOF */

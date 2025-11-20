/*
 * SPDX-FileCopyrightText: 2016 Engineering-Spirit
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "compiler_hints.h"
#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"
#include "periph/vbat.h"

/**
 * @brief   Maximum allowed ADC clock speed
 */
#ifndef ADC_CLK_MAX
#define ADC_CLK_MAX             MHZ(12)
#endif

/**
 * @brief   Default VBAT undefined value
 */
#ifndef VBAT_ADC
#define VBAT_ADC    ADC_UNDEF
#endif

/**
 * @brief   Allocate locks for all three available ADC devices
 */
static mutex_t locks[] = {
#if ADC_DEVS > 1
    MUTEX_INIT,
#endif
#if ADC_DEVS > 2
    MUTEX_INIT,
#endif
    MUTEX_INIT
};

static inline ADC_TypeDef *dev(adc_t line)
{
    return (ADC_TypeDef *)(ADC1_BASE + (adc_config[line].dev << 8));
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
    uint32_t clk_div = 2;

    /* check if the line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* lock and power-on the device */
    prep(line);

    /* configure the pin */
    if (adc_config[line].pin != GPIO_UNDEF) {
        gpio_init_analog(adc_config[line].pin);
    }
    /* set clock prescaler to get the maximal possible ADC clock value */
    for (clk_div = 2; clk_div < 8; clk_div += 2) {
        if ((periph_apb_clk(APB2) / clk_div) <= ADC_CLK_MAX) {
            break;
        }
    }
    assume((periph_apb_clk(APB2) / clk_div) <= ADC_CLK_MAX);
    ADC->CCR = ((clk_div / 2) - 1) << 16;

    if (IS_USED(MODULE_PERIPH_VBAT)) {
        /* Set the sampling rate for the VBat channel to 112 cycles. It reads
        * correct with 84 cycles already, so this adds some margin. */
        ADC1->SMPR1 = (ADC1->SMPR1 & ~ADC_SMPR1_SMP18) | \
                      (ADC_SMPR1_SMP18_2 | ADC_SMPR1_SMP18_0);
    }

    /* enable the ADC module */
    dev(line)->CR2 = ADC_CR2_ADON;

    /* free the device again */
    done(line);
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
    prep(line);
    /* check if this is the VBAT line or another internal ADC channel */
    if (IS_USED(MODULE_PERIPH_VBAT) && line == VBAT_ADC) {
        vbat_enable();
    }
    else if (dev(line) == ADC1) {
        if (adc_config[line].chan == 16 || adc_config[line].chan == 17) {
            ADC->CCR |= ADC_CCR_TSVREFE;
        }
    }
    /* set resolution and conversion channel */
    dev(line)->CR1 = res;
    dev(line)->SQR3 = adc_config[line].chan;
    /* start conversion and wait for results */
    dev(line)->CR2 |= ADC_CR2_SWSTART;
    while (!(dev(line)->SR & ADC_SR_EOC)) {}
    /* finally read sample and reset the STRT bit in the status register */
    sample = (int)dev(line)->DR;
    /* check if this is the VBAT line or another internal ADC channel */
    if (IS_USED(MODULE_PERIPH_VBAT) && line == VBAT_ADC) {
        vbat_disable();
    }
    else if (dev(line) == ADC1) {
        if (adc_config[line].chan == 16 || adc_config[line].chan == 17) {
            ADC->CCR &= ~ADC_CCR_TSVREFE;
        }
    }
    /* power off and unlock device again */
    done(line);

    return sample;
}

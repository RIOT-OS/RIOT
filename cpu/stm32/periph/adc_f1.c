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
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "compiler_hints.h"
#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"

/**
 * @brief   Maximum allowed ADC clock speed
 */
#ifndef ADC_CLK_MAX
#define ADC_CLK_MAX             MHZ(14)
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

    /* enable the ADC module */
    dev(line)->CR2 |= ADC_CR2_ADON;

    /* check if this channel is an internal ADC channel, if so
     * enable the internal temperature and Vref */
    if (adc_config[line].chan == 16 || adc_config[line].chan == 17) {
        dev(line)->CR2 |= ADC_CR2_TSVREFE;
    }
}

static inline void done(adc_t line)
{
    /* disable the internal temperature and Vref */
    dev(line)->CR2 &= ~ADC_CR2_TSVREFE;

    /* disable the ADC module */
    dev(line)->CR2 &= ~ADC_CR2_ADON;

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
    RCC->CFGR &= ~(RCC_CFGR_ADCPRE);
    RCC->CFGR |= ((clk_div / 2) - 1) << 14;

    /* resets the selected ADC calibration registers */
    dev(line)->CR2 |= ADC_CR2_RSTCAL;
    /* check the status of RSTCAL bit */
    while (dev(line)->CR2 & ADC_CR2_RSTCAL) {}

    /* enable the selected ADC calibration process */
    dev(line)->CR2 |= ADC_CR2_CAL;
    /* wait for the calibration to have finished */
    while (dev(line)->CR2 & ADC_CR2_CAL) {}

    /* set all channels to maximum (239.5) cycles for best accuracy */
    dev(line)->SMPR1 |= 0x00ffffff;
    dev(line)->SMPR2 |= 0x3fffffff;
    /* we want to sample one channel */
    dev(line)->SQR1 = ADC_SQR1_L_0;
    /* start sampling from software */
    dev(line)->CR2 |= ADC_CR2_EXTTRIG | ADC_CR2_EXTSEL;

    /* check if the internal channels are configured to use ADC1 */
    if (adc_config[line].chan == 16 || adc_config[line].chan == 17) {
        if (dev(line) != ADC1) {
            return -3;
        }
    }

    /* free the device again */
    done(line);
    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    int sample;

    /* check if the linenel is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* check if resolution is applicable */
    if (res != ADC_RES_12BIT) {
        return -2;
    }

    /* lock and power on the ADC device  */
    prep(line);

    /* set conversion channel */
    dev(line)->SQR3 = adc_config[line].chan;
    /* start conversion and wait for results */
    dev(line)->CR2 |= ADC_CR2_SWSTART;
    while (!(dev(line)->SR & ADC_SR_EOC)) {}
    /* finally read sample and reset the STRT bit in the status register */
    sample = (int)dev(line)->DR;

    /* power off and unlock device again */
    done(line);

    return sample;
}

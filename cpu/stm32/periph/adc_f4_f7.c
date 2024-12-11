/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "compiler_hints.h"
#include "cpu.h"
#include "irq.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph/vbat.h"
#include "periph_conf.h"

/**
 * @brief   Maximum allowed ADC clock speed
 */
#ifndef ADC_CLK_MAX
#define ADC_CLK_MAX             MHZ(12)
#endif

/**
 * @brief   Maximum sampling time for each channel (480 cycles)
 *          T_CONV[µs] = (RESOLUTION[bits] + SMP[cycles]) / CLOCK_SPEED[MHz]
 */
#define MAX_ADC_SMP             (7u)

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
    /* set sequence length to 1 conversion and enable the ADC device */
    dev(line)->SQR1 = 0;
    dev(line)->CR2 = ADC_CR2_ADON;
    /* set clock prescaler to get the maximal possible ADC clock value */
    for (clk_div = 2; clk_div < 8; clk_div += 2) {
        if ((periph_apb_clk(APB2) / clk_div) <= ADC_CLK_MAX) {
            break;
        }
    }
    assume((periph_apb_clk(APB2) / clk_div) <= ADC_CLK_MAX);
    ADC->CCR = ((clk_div / 2) - 1) << 16;
    /* set sampling time to the maximum */
    unsigned irq_state = irq_disable();
    if (adc_config[line].chan >= 10) {
        uint32_t smpr1 = dev(line)->SMPR1;
        smpr1 &= ~(MAX_ADC_SMP << (3 * (adc_config[line].chan - 10)));
        smpr1 |= MAX_ADC_SMP << (3 * (adc_config[line].chan - 10));
        dev(line)->SMPR1 = smpr1;
    }
    else {
        uint32_t smpr2 = dev(line)->SMPR2;
        smpr2 &= ~(MAX_ADC_SMP << (3 * adc_config[line].chan));
        smpr2 |= MAX_ADC_SMP << (3 * adc_config[line].chan);
        dev(line)->SMPR2 = smpr2;
    }
    irq_restore(irq_state);
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
    /* check if this channel is an internal ADC channel */
    if (IS_USED(MODULE_PERIPH_VBAT) && line == VBAT_ADC) {
        vbat_enable();
    }
    /* set resolution and conversion channel */
    dev(line)->CR1 = res;
    dev(line)->SQR3 = adc_config[line].chan;
    /* start conversion and wait for results */
    dev(line)->CR2 |= ADC_CR2_SWSTART;
    while (!(dev(line)->SR & ADC_SR_EOC)) {}
    /* finally read sample and reset the STRT bit in the status register */
    sample = (int)dev(line)->DR;
    /* check if this channel was an internal ADC channel */
    if (IS_USED(MODULE_PERIPH_VBAT) && line == VBAT_ADC) {
        vbat_disable();
    }
    /* power off and unlock device again */
    done(line);

    return sample;
}

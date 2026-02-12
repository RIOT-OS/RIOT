/*
 * SPDX-FileCopyrightText: 2020 LAAS-CNRS
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
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"
#include "periph/cpu_dma.h"
#include "ztimer.h"
#include "periph/vbat.h"

#define ADC_INSTANCE    ADC12_COMMON

/**
 * @brief   Default VBAT undefined value
 */
#ifndef VBAT_ADC
#define VBAT_ADC    ADC_UNDEF
#endif

/**
 * @brief   Allocate locks for all available ADC devices
 */
static mutex_t locks[ADC_DEVS];

static inline ADC_TypeDef *dev(adc_t line)
{
    switch (adc_config[line].dev) {
#ifdef ADC1_BASE
        case 0:
            return (ADC_TypeDef *)(ADC1_BASE);
            break;
#endif
#ifdef ADC2_BASE
        case 1:
            return (ADC_TypeDef *)(ADC2_BASE);
            break;
#endif
#ifdef ADC3_BASE
        case 2:
            return (ADC_TypeDef *)(ADC3_BASE);
            break;
#endif
#ifdef ADC4_BASE
        case 3:
            return (ADC_TypeDef *)(ADC4_BASE);
            break;
#endif
    }

    /* should never reach here */
    assert(false);
    return NULL;
}

static inline void prep(adc_t line)
{
    mutex_lock(&locks[adc_config[line].dev]);
    /* Enable the clock here only if it will be disabled by done, else just
     * enable it once in adc_init() */
#if defined(RCC_AHBENR_ADC1EN)
    periph_clk_en(AHB, RCC_AHBENR_ADC1EN);
#endif
}

static inline void done(adc_t line)
{
    /* On some STM32F3 ADC are grouped by paire (ADC12EN or ADC34EN) so
     * don't disable the clock as the other device may still use it. */
#if defined(RCC_AHBENR_ADC1EN)
    periph_clk_dis(AHB, RCC_AHBENR_ADC1EN);
#endif
    mutex_unlock(&locks[adc_config[line].dev]);
}

int adc_init(adc_t line)
{
    /* Check if the line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    RCC->D3CCIPR |= RCC_D3CCIPR_ADCSEL_1;

    /* Lock device and enable its peripheral clock */
    prep(line);
    /* On some STM32F3 ADC are grouped by paire (ADC12EN or ADC34EN) so
     * enable the clock only once here. */
    if (adc_config[line].dev <= 1) {
        periph_clk_en(AHB1, RCC_AHB1ENR_ADC12EN);
    }
    if (adc_config[line].dev >= 2) {
        periph_clk_en(AHB4, RCC_AHB4ENR_ADC3EN);
    }

    ADC_INSTANCE->CCR = ADC_CCR_CKMODE_1 | ADC_CCR_CKMODE_0;
    ADC3_COMMON->CCR = ADC_CCR_CKMODE_1 | ADC_CCR_CKMODE_0 |
                       ADC_CCR_VREFEN;

    /* Configure the pin */
    if (adc_config[line].pin != GPIO_UNDEF) {
        gpio_init_analog(adc_config[line].pin);
    }

    /* Init ADC line only if it wasn't already initialized */
    if (!(dev(line)->CR & ADC_CR_ADEN)) {
        /* take ADC out of deep sleep */
        dev(line)->CR &= ~(ADC_CR_DEEPPWD);
        /* Enable ADC internal voltage regulator and wait for startup period */
        dev(line)->CR |= ADC_CR_ADVREGEN;
#if IS_USED(MODULE_ZTIMER_USEC) && 0
        ztimer_sleep(ZTIMER_USEC, ADC_T_ADCVREG_STUP_US);
#else
        /* to avoid using ZTIMER_USEC unless already included round up the
         * internal voltage regulator start up to 1ms */
        ztimer_sleep(ZTIMER_MSEC, 1);
#endif

        if (false) {
            /* Configure calibration for differential inputs */
            dev(line)->CR |= ADC_CR_ADCALDIF;
        }
        else {
            /* Configure calibration for single ended inputs */
            dev(line)->CR &= ~ADC_CR_ADCALDIF;
        }

        /* enable linearity cal, and turn on boost supply */
        if (adc_config[line].dev < 2)
        {
            dev(line)->CR |= ADC_CR_ADCALLIN | ADC_CR_BOOST;
        }

        /* Start automatic calibration and wait for it to complete */
        dev(line)->CR |= ADC_CR_ADCAL;
        while (dev(line)->CR & ADC_CR_ADCAL) {}

        /* Clear ADRDY by writing it */
        dev(line)->ISR |= ADC_ISR_ADRDY;

        /* Enable ADC and wait for it to be ready */
        dev(line)->CR |= ADC_CR_ADEN;
        while ((dev(line)->ISR & ADC_ISR_ADRDY) == 0) {}

        /* Set sequence length to 1 conversion */
        dev(line)->SQR1 |= (0 & ADC_SQR1_L);
    }

    /* Enable the ADC channel's analog switch */
    #if defined(ADC_VER_V5_V90)
    if (adc_config[line].dev < 2)
    {
        dev(line)->PCSEL_RES0 |= ADC_PCSEL_PCSEL_0 << adc_config[line].chan;
    }
    #else
    dev(line)->PCSEL |= ADC_PCSEL_PCSEL_0 << adc_config[line].chan;
    #endif

    /* sample time to use
     * ADC1/2 : (1/34.375) uS * (7.5+16.5) samples = 0.698182 uSec/conversion
     * ADC3   : (1/68.75) uS * (12.5+47.5) samples = 0.872727 uSec/conversion
     */
    unsigned smp = (adc_config[line].dev < 2) ? 3 : 4;

    /* Slow internal channels need long sample time */
    if (adc_config[line].pin == GPIO_UNDEF) {
        smp = 7;
    }

    /* Configure sampling time for the given channel */
    if (adc_config[line].chan < 10) {
        dev(line)->SMPR1 |= (smp << (adc_config[line].chan * 3));
    }
    else {
        dev(line)->SMPR2 |= (smp << ((adc_config[line].chan - 10) * 3));
    }

    /* Power off and unlock device again */
    done(line);

    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    int sample;
    /* check if resolution is applicable */
    if (res == ADC_RES_6BIT) {
        return -1;
    }
    /* Lock and power on the ADC device */
    prep(line);

    /* check if this is the VBAT line */
    if (IS_USED(MODULE_PERIPH_VBAT) && line == VBAT_ADC) {
        vbat_enable();
    }

    /* Set resolution */
    if (adc_config[line].dev <= 2)
    {
        dev(line)->CFGR &= ~ADC_CFGR_RES;
        dev(line)->CFGR |= res;
    }
    else
    {
        dev(line)->CFGR &= ~ADC_CFGR_RES;
        switch (res)
        {
            case ADC_RES_12BIT:
            break;

            case ADC_RES_10BIT:
            dev(line)->CFGR |= 0x01 << ADC_CFGR_RES_Pos;
            break;

            case ADC_RES_8BIT:
            dev(line)->CFGR |= 0x02 << ADC_CFGR_RES_Pos;
            break;

            case ADC_RES_6BIT:
            dev(line)->CFGR |= 0x03 << ADC_CFGR_RES_Pos;
            break;

            default:
            break;
        }
    }

    /* Specify channel for regular conversion */
    dev(line)->SQR1 = adc_config[line].chan << ADC_SQR1_SQ1_Pos;

    /* Start conversion and wait for it to complete */
    dev(line)->ISR |= ADC_ISR_EOC;
    dev(line)->CR |= ADC_CR_ADSTART;
    while (!(dev(line)->ISR & ADC_ISR_EOC)) {}

    /* Read the sample */
    sample = (int)dev(line)->DR;

    /* check if this is the VBAT line */
    if (IS_USED(MODULE_PERIPH_VBAT) && line == VBAT_ADC) {
        vbat_disable();
    }

    /* Power off and unlock device again */
    done(line);

    return sample;
}

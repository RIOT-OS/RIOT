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

#include "busy_wait.h"
#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"
#include "periph/vbat.h"

#define ADC_SMP_MIN_VAL     (0x2) /*< Sampling time for slow channels
                                      (0x2 = 4.5 ADC clock cycles) */
#define ADC_SMP_VBAT_VAL    (0x5) /*< Sampling time when the VBat channel
                                      is read (0x5 = 61.5 ADC clock cycles) */

/* The sampling time width is 3 bit */
#define ADC_SMP_BIT_WIDTH    (3)

/* The sampling time can be specified for each channel over SMPR1 and SMPR2.
   This specifies the first channel that goes to SMPR2 instead of SMPR1. */
#define ADC_SMPR2_FIRST_CHAN (10)

#ifdef ADC1_COMMON
#define ADC_INSTANCE    ADC1_COMMON
#else
#define ADC_INSTANCE    ADC12_COMMON
#endif

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
#ifdef ADC2_BASE
        case 1:
            return (ADC_TypeDef *)(ADC2_BASE);
            break;
#endif
#ifdef ADC34_COMMON
        case 2:
            return (ADC_TypeDef *)(ADC3_BASE);
            break;
        case 3:
            return (ADC_TypeDef *)(ADC4_BASE);
            break;
#endif
        default:
            return (ADC_TypeDef *)(ADC1_BASE);
            break;
    }
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

    /* Lock device and enable its peripheral clock */
    prep(line);
/* On some STM32F3 ADC are grouped by paire (ADC12EN or ADC34EN) so
 * enable the clock only once here. */
#if defined(RCC_AHBENR_ADC12EN)
    if (adc_config[line].dev <= 1) {
        periph_clk_en(AHB, RCC_AHBENR_ADC12EN);
    }
#endif
#if defined(RCC_AHBENR_ADC34EN)
    if (adc_config[line].dev >= 2) {
        periph_clk_en(AHB, RCC_AHBENR_ADC34EN);
    }
#endif

    /* Setting ADC clock to HCLK/1 is only allowed if AHB clock
     * prescaler is 1 */
    if (!(RCC->CFGR & RCC_CFGR_HPRE_3)) {
        /* set ADC clock to HCLK/1 */
        if (adc_config[line].dev <= 1) {
            ADC_INSTANCE->CCR |= ADC_CCR_CKMODE_0;
        }
#ifdef ADC34_COMMON
        if (adc_config[line].dev >= 2) {
            ADC34_COMMON->CCR |= ADC_CCR_CKMODE_0;
        }
#endif
    }
    else {
        /* set ADC clock to HCLK/2 otherwise */
        if (adc_config[line].dev <= 1) {
            ADC_INSTANCE->CCR |= ADC_CCR_CKMODE_1;
        }
#ifdef ADC34_COMMON
        if (adc_config[line].dev >= 2) {
            ADC34_COMMON->CCR |= ADC_CCR_CKMODE_1;
        }
#endif
    }

    /* Configure the pin */
    if (adc_config[line].pin != GPIO_UNDEF) {
        gpio_init_analog(adc_config[line].pin);
    }
    /* Init ADC line only if it wasn't already initialized */
    if (!(dev(line)->CR & ADC_CR_ADEN)) {
        /* Enable ADC internal voltage regulator and wait for startup period */
        dev(line)->CR |= ADC_CR_ADVREGEN;
        busy_wait_us(ADC_T_ADCVREG_STUP_US * 2);

        if (dev(line)->DIFSEL & (1 << adc_config[line].chan)) {
            /* Configure calibration for differential inputs */
            dev(line)->CR |= ADC_CR_ADCALDIF;
        }
        else {
            /* Configure calibration for single ended inputs */
            dev(line)->CR &= ~ADC_CR_ADCALDIF;
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

    /* determine the right sampling time */
    uint32_t smp_time = ADC_SMP_MIN_VAL;
    if (IS_USED(MODULE_PERIPH_VBAT) && line == VBAT_ADC) {
        smp_time = ADC_SMP_VBAT_VAL;
    }

    /* Configure sampling time for the given channel */
    if (adc_config[line].chan < 10) {
        dev(line)->SMPR1 = (smp_time << (adc_config[line].chan
                                        * ADC_SMP_BIT_WIDTH));
    }
    else {
        dev(line)->SMPR2 = (smp_time << ((adc_config[line].chan
                                        - ADC_SMPR2_FIRST_CHAN)
                                        * ADC_SMP_BIT_WIDTH));
    }

    /* Power off and unlock device again */
    done(line);

    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    int sample;

    /* Check if resolution is applicable */
    if ((res & ADC_CFGR_RES) != res) {
        return -1;
    }

    /* Lock and power on the ADC device */
    prep(line);

    /* check if this is the VBAT line */
    if (IS_USED(MODULE_PERIPH_VBAT) && line == VBAT_ADC) {
        vbat_enable();
    }

    /* Set resolution */
    dev(line)->CFGR &= ~ADC_CFGR_RES;
    dev(line)->CFGR |= res;

    /* Specify channel for regular conversion */
    dev(line)->SQR1 = adc_config[line].chan << ADC_SQR1_SQ1_Pos;

    /* Start conversion and wait for it to complete */
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

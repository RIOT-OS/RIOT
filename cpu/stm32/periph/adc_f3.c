/*
 * Copyright (C) 2020 LAAS-CNRS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Hugues Larrive <hugues.larrive@laas.fr>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"
#include "xtimer.h"

#define SMP_SLOW    (0x2) /*< Sampling time for slow channels
                                (0x2 = 4.5 ADC clock cycles) */

/**
 * @brief   Allocate locks for all available ADC devices
 */
static mutex_t locks[ADC_DEVS];

static inline ADC_TypeDef *dev(adc_t line)
{
    return (ADC_TypeDef *)(ADC1_BASE + (adc_config[line].dev << 8));
}

static inline void prep(adc_t line)
{
    mutex_lock(&locks[adc_config[line].dev]);
/* Enable the clock here only if it will be disabled by done, else just
 * enable it once in adc_init() */
#if defined(RCC_AHBENR_ADC1EN)
    periph_clk_en(AHB, (RCC_AHBENR_ADC1EN << adc_config[line].dev));
#endif
}

static inline void done(adc_t line)
{
/* On some STM32F3 ADC are grouped by paire (ADC12EN or ADC34EN) so
 * don't disable the clock as the other device may still use it. */
#if defined(RCC_AHBENR_ADC1EN)
    periph_clk_dis(AHB, (RCC_AHBENR_ADC1EN << adc_config[line].dev));
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
#if !defined(RCC_AHBENR_ADC1EN)
    periph_clk_en(AHB, (RCC_AHBENR_ADC12EN << adc_config[line].dev));
#endif

    /* Setting ADC clock to HCLK/1 is only allowed if AHB clock
     * prescaler is 1 */
    if (!(RCC->CFGR & RCC_CFGR_HPRE_3)) {
        /* set ADC clock to HCLK/1 */
        ADC12_COMMON->CCR |= ADC_CCR_CKMODE_0;
    }
    else {
        /* set ADC clock to HCLK/2 otherwise */
        ADC12_COMMON->CCR |= ADC_CCR_CKMODE_1;
    }

    /* Configure the pin */
    gpio_init_analog(adc_config[line].pin);

    /* Init ADC line only if it wasn't already initialized */
    if (!(dev(line)->CR & ADC_CR_ADEN)) {
        /* Enable ADC internal voltage regulator and wait for startup period */
        dev(line)->CR |= ADC_CR_ADVREGEN;
        xtimer_usleep(ADC_T_ADCVREG_STUP_US);

        /* Configure calibration for single ended input */
        dev(line)->CR &= ~ADC_CR_ADCALDIF;

        /* Start automatic calibration and wait for it to complete */
        dev(line)->CR |= ADC_CR_ADCAL;
        while (dev(line)->CR  & ADC_CR_ADCAL) {}

        /* Clear ADRDY by writing it */
        dev(line)->ISR |= ADC_ISR_ADRDY;

        /* Enable ADC and wait for it to be ready */
        dev(line)->CR |= ADC_CR_ADEN;
        while ((dev(line)->ISR & ADC_ISR_ADRDY) == 0) {}

        /* Set sequence length to 1 conversion */
        dev(line)->SQR1 |= (0 & ADC_SQR1_L);
    }

    /* Configure sampling time for the given channel (6 to 18) */
    dev(line)->SMPR1 = (SMP_SLOW << ADC_SMPR1_SMP6_Pos)
                        | (SMP_SLOW << ADC_SMPR1_SMP7_Pos)
                        | (SMP_SLOW << ADC_SMPR1_SMP8_Pos)
                        | (SMP_SLOW << ADC_SMPR1_SMP9_Pos);
    dev(line)->SMPR2 = (SMP_SLOW << ADC_SMPR2_SMP10_Pos)
                        | (SMP_SLOW << ADC_SMPR2_SMP11_Pos)
                        | (SMP_SLOW << ADC_SMPR2_SMP12_Pos)
                        | (SMP_SLOW << ADC_SMPR2_SMP13_Pos)
                        | (SMP_SLOW << ADC_SMPR2_SMP14_Pos)
                        | (SMP_SLOW << ADC_SMPR2_SMP15_Pos)
                        | (SMP_SLOW << ADC_SMPR2_SMP16_Pos)
                        | (SMP_SLOW << ADC_SMPR2_SMP17_Pos)
                        | (SMP_SLOW << ADC_SMPR2_SMP18_Pos);

    /* Power off and unlock device again */
    done(line);

    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    int sample;

    /* Check if resolution is applicable */
    if (res & 0x3) {
        return -1;
    }

    /* Lock and power on the ADC device */
    prep(line);

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

    /* Power off and unlock device again */
    done(line);

    return sample;
}

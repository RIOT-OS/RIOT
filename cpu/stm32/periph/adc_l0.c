/*
 * SPDX-FileCopyrightText: 2014-2017 Freie Universität Berlin
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
 * @author      Aurélien Fillau <aurelien.fillau@we-sens.com>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"

/**
 * @brief   Allocate locks for all three available ADC device
 *
 * All STM32L0 CPUs we support so far only come with a single ADC device.
 */
static mutex_t lock = MUTEX_INIT;

static inline void prep(void)
{
    mutex_lock(&lock);
    periph_clk_en(APB2, RCC_APB2ENR_ADCEN);
}

static inline void done(void)
{
    periph_clk_dis(APB2, RCC_APB2ENR_ADCEN);
    mutex_unlock(&lock);
}

static void _enable_adc(void)
{
    if ((ADC1->CR & ADC_CR_ADEN) != 0) {
        ADC1->CR |= ADC_CR_ADDIS;
        while(ADC1->CR & ADC_CR_ADEN) {} /* Wait for ADC disabled */
    }

    if ((ADC1->CR & ADC_CR_ADEN) == 0) {
        /* Then, start a calibration */
        ADC1->CR |= ADC_CR_ADCAL;
        while(ADC1->CR & ADC_CR_ADCAL) {} /* Wait for the end of calibration */
    }

    /* Clear flag */
    ADC1->ISR |= ADC_ISR_ADRDY;

    /* enable device */
    ADC1->CR = ADC_CR_ADVREGEN | ADC_CR_ADEN;

    /* Wait for ADC to be ready */
    while (!(ADC1->ISR & ADC_ISR_ADRDY)) {}
}

static void _disable_adc(void)
{
    /* Disable ADC */
    if ((ADC1->CR & ADC_CR_ADEN) != 0) {
        ADC1->CR |= ADC_CR_ADDIS;
        while(ADC1->CR & ADC_CR_ADEN) {} /* Wait for ADC disabled */
        /* Disable Voltage regulator */
        ADC1->CR = 0;
        ADC1->ISR = 0;
    }
}

int adc_init(adc_t line)
{
    /* make sure the given line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* lock and power on the device */
    prep();

    if ((adc_config[line].chan != 17) && (adc_config[line].chan != 18)) {
        /* configure the pin */
        gpio_init_analog(adc_config[line].pin);
    }

    /* no watchdog, no discontinuous mode, no auto off, single conv, no trigger,
     * right align, 12bits, no dma, no wait */
    ADC1->CFGR1 = 0;
    /* no oversampling: Watch out, MSB (CKMODE) MUST not be changed while on
     * (it is zero by default) */
    ADC1->CFGR2 = 0;
    /* activate VREF, and set prescaler to 4 (4Mhz clock)
     * activate also temp sensor, so that it will be ready for temp measure */
    ADC->CCR = ADC_CCR_VREFEN | ADC_CCR_TSEN | ADC_CCR_PRESC_1;
    /* Sampling time selection: 7 => 160 clocks => 40µs @ 4MHz
     * (must be 10+10 for ref start and sampling time) */
    ADC1->SMPR |= ADC_SMPR_SMP;
    /* clear previous flag */
    ADC1->ISR |= ADC_ISR_EOC;

    /* power off an release device for now */
    done();

    return 0;
}

int32_t adc_sample(adc_t line,  adc_res_t res)
{
    int sample;

    /* check if resolution is applicable */
    if ((res & ADC_CFGR1_RES) != res) {
        return -1;
    }

    /* lock and power on the ADC device  */
    prep();

    /* Reactivate VREFINT and temperature sensor if necessary */
    if (adc_config[line].chan == 17) {
        ADC->CCR |= ADC_CCR_VREFEN;
    }
    else if (adc_config[line].chan == 18) {
        ADC->CCR |= ADC_CCR_TSEN;
    }
    /* else nothing */

    /* set resolution and channel */
    ADC1->CFGR1 &= ~ADC_CFGR1_RES;
    ADC1->CFGR1 |= res & ADC_CFGR1_RES;
    ADC1->CHSELR = (1 << adc_config[line].chan);

    /* Enable ADC */
    _enable_adc();

    /* clear flag */
    ADC1->ISR |= ADC_ISR_EOC;

    /* start conversion and wait for results */
    ADC1->CR |= ADC_CR_ADSTART;

    while (!(ADC1->ISR & ADC_ISR_EOC)) {}

    /* read result */
    sample = (int)ADC1->DR;

    /* Disable ADC */
    _disable_adc();

    /* Deactivate VREFINT and temperature sensor to save power */
    ADC->CCR &= ~(ADC_CCR_VREFEN | ADC_CCR_TSEN);

    /* unlock and power off device again */
    done();

    return sample;
}

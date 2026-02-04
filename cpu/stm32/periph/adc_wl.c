/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 HAW-Hamburg
 * SPDX-FileCopyrightText: 2021 Inria
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
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "busy_wait.h"
#include "periph/adc.h"
#include "periph_conf.h"
#include "periph/vbat.h"

/**
 * @brief   Default VBAT undefined value
 */
#ifndef VBAT_ADC
#define VBAT_ADC    ADC_UNDEF
#endif

/* ADC register CR bits with HW property "rs":
 * Software can read as well as set this bit. We want to avoid writing a status
 * bit with a Read-Modify-Write cycle and accidentally setting other status
 * bits as well. Writing '0' has no effect on the bit value. */
#define ADC_CR_BITS_PROPERTY_RS (ADC_CR_ADCAL | ADC_CR_ADSTP | ADC_CR_ADSTART \
                                | ADC_CR_ADDIS | ADC_CR_ADEN)

/**
 * @brief   Allocate lock for the ADC device
 *
 * All STM32WL CPUs we support so far only come with a single ADC device.
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

static int _enable_adc(void)
{
    /* check if the ADC is not already enabled */
    if (ADC->CR & ADC_CR_ADEN) {
        return 0;
    }

    /* ensure the prerequisites are right */
    if (ADC->CR & (ADC_CR_ADCAL | ADC_CR_ADSTP | ADC_CR_ADSTART | ADC_CR_ADDIS)) {
        return -1;
    }

    /* clear ADRDY by writing to it */
    ADC->ISR |= ADC_ISR_ADRDY;

    /* enable the ADC and wait for the READY flag */
    ADC->CR = (ADC->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADEN;

    while (!(ADC->ISR & ADC_ISR_ADRDY)) {}

    return 0;
}

static int _disable_adc(void)
{
    /* check if disable is going on or ADC is disabled already */
    if ((ADC->CR & ADC_CR_ADDIS) || !(ADC->CR & ADC_CR_ADEN)) {
        while (ADC->CR & ADC_CR_ADDIS) {}
        return 0;
    }

    /* make sure no conversion is going on and stop it if it is */
    if (ADC->CR & ADC_CR_ADSTART) {
        ADC->CR = (ADC->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADSTP;

        while (ADC->CR & ADC_CR_ADSTP) {}
    }

    /* disable the ADC and wait until is is disabled*/
    ADC->CR = (ADC->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADDIS;
    while (!(ADC->CR & ADC_CR_ADEN)) {}

    /* clear the ADRDY bit by writing 1 to it */
    ADC->ISR |= ADC_ISR_ADRDY;

    return 0;
}

int adc_init(adc_t line)
{
    /* check if the line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* lock device and enable its peripheral clock */
    prep();

    /* configure the pin */
    if (adc_config[line].pin != GPIO_UNDEF) {
        gpio_init_analog(adc_config[line].pin);
    }

    /* init ADC line only if it wasn't already initialized. Check a register
     * set by the initialization which has a reset value of 0 */
    if (ADC->SMPR == 0) {

        /* set prescaler to 0 to let the ADC run with maximum speed */
        ADC_COMMON->CCR &= ~(ADC_CCR_PRESC);

        /* set ADC clock to PCLK/2 otherwise */
        ADC->CFGR2 &= ~(ADC_CFGR2_CKMODE_0 | ADC_CFGR2_CKMODE_1);
        ADC->CFGR2 |= ADC_CFGR2_CKMODE_0;

        /* enable ADC internal voltage regulator and wait for startup period */
        ADC->CR = (ADC->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADVREGEN;
        busy_wait(ADC_T_ADCVREG_STUP_US * 2);

        /* start automatic calibration and wait for it to complete */
        ADC->CR = (ADC->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADCAL;
        while (ADC->CR  & ADC_CR_ADCAL) {}

        /* set sequence length to 1 conversion */
        ADC->CFGR1 &= ~ADC_CFGR1_CONT;

        /* Set Sampling Time Register 1 to 3.5 ADC Cycles for all GPIO-Channels
         * and Sampling Time Register 2 to 39.5 ADC Cycles for VBat. Set the
         * VBat channel to use Sampling Time Register 2. */
        ADC->SMPR = ADC_SMPR_SMP1_0;
        if (IS_USED(MODULE_PERIPH_VBAT)) {
            ADC->SMPR |= ADC_SMPR_SMP2_2 | ADC_SMPR_SMP2_0 |
                         (1 << (adc_config[VBAT_ADC].chan+ ADC_SMPR_SMPSEL_Pos));
        }
    }

    /* free the device again */
    done();
    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    int sample;

    /* check if resolution is applicable */
    if ((res & ADC_CFGR1_RES) != res) {
        return -1;
    }

    /* lock and power on the ADC device  */
    prep();

    /* check if this is the VBAT line */
    if (IS_USED(MODULE_PERIPH_VBAT) && line == VBAT_ADC) {
        vbat_enable();
    }

    /* first clear resolution */
    ADC->CFGR1 &= ~ADC_CFGR1_RES;

    /* then set resolution to the required value*/
    ADC->CFGR1 |= res;

    /* specify channel for regular conversion */
    ADC->CHSELR = (1 << adc_config[line].chan);

    /* check if the ADC was enabled successfully */
    if (_enable_adc() == -1) {
        done();
        return -1;
    }

    /* start conversion and wait for it to complete */
    ADC->CR = (ADC->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADSTART;
    while (!(ADC->ISR & ADC_ISR_EOC)) {}

    /* read the sample */
    sample = (int)ADC->DR;

    /* check if this is the VBAT line */
    if (IS_USED(MODULE_PERIPH_VBAT) && line == VBAT_ADC) {
        vbat_disable();
    }

    /* disable, unlock and power off the device again */
    int ret = _disable_adc();
    done();

    if (ret == -1) {
        return -1;
    }
    else {
        return sample;
    }
}

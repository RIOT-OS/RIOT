/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
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
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph/vbat.h"

#include "busy_wait.h"

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
 * All STM32F0 & STM32G0 CPUs we support so far only come with a single ADC device.
 */
static mutex_t lock = MUTEX_INIT;

static inline void prep(void)
{
    mutex_lock(&lock);
#ifdef RCC_APB2ENR_ADCEN
    periph_clk_en(APB2, RCC_APB2ENR_ADCEN);
#endif
#ifdef RCC_APBENR2_ADCEN
    periph_clk_en(APB12, RCC_APBENR2_ADCEN);
#endif
}

static inline void done(void)
{
#ifdef RCC_APB2ENR_ADCEN
    periph_clk_dis(APB2, RCC_APB2ENR_ADCEN);
#endif
#ifdef RCC_APBENR2_ADCEN
    periph_clk_dis(APB12, RCC_APBENR2_ADCEN);
#endif
    mutex_unlock(&lock);
}

static int _enable_adc(void)
{
    /* check if the ADC is not already enabled */
    if (ADC1->CR & ADC_CR_ADEN) {
        return 0;
    }

    /* ensure the prerequisites are right */
    if (ADC1->CR & (ADC_CR_ADCAL | ADC_CR_ADSTP | ADC_CR_ADSTART | ADC_CR_ADDIS)) {
        return -1;
    }

    /* enable the ADC and wait for the READY flag */
    ADC1->CR = (ADC1->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADEN;

    while (!(ADC1->ISR & ADC_ISR_ADRDY)) {
        /* the calibration logic can reset the ADEN flag, so keep enabling it */
        if (!(ADC1->CR & ADC_CR_ADEN)) {
            ADC1->CR = (ADC1->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADEN;
        }
    }

    return 0;
}

static int _disable_adc(void)
{
    /* check if disable is going on or ADC is disabled already */
    if ((ADC1->CR & ADC_CR_ADDIS) || !(ADC1->CR & ADC_CR_ADEN)) {
        while (ADC1->CR & ADC_CR_ADDIS) {}
        return 0;
    }

    /* make sure no conversion is going on and stop it if it is */
    if (ADC1->CR & ADC_CR_ADSTART) {
        ADC1->CR = (ADC1->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADSTP;

        while (ADC1->CR & ADC_CR_ADSTP) {}
    }

    /* disable the ADC and wait until is is disabled*/
    ADC1->CR = (ADC1->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADDIS;
    while (!(ADC1->CR & ADC_CR_ADEN)) {}

    return 0;
}

int adc_init(adc_t line)
{
    /* make sure the given line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* lock and power on the device, but keep it disabled */
    prep();
    _disable_adc();

    /* configure the pin */
    if (adc_config[line].pin != GPIO_UNDEF) {
        gpio_init_analog(adc_config[line].pin);
    }

    /* init ADC only if it wasn't already initialized. Check a register
     * set by the initialization which has a reset value of 0 */
    if (ADC1->SMPR == 0) {

        /* reset configuration, including ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG | ADC_CFGR1_AUTOFF */
        ADC1->CFGR1 = 0;
        ADC1->CFGR2 = 0;

        /* Calibration procedure according to:
        * - RM0360 section 12.3.2 for the STM32F0
        * - RM0454 section 14.3.3 for the STM32G0
        * - RM0490 section 16.4.3 for the STM32C0 */

        /* only enable the ADC voltage regulator if the chip has one (STM32F0 does not) */
#if defined(ADC_CR_ADVREGEN)
        ADC1->CR = (ADC1->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADVREGEN;

        /* wait for t_ADCVREG_STUP = 20us with some headroom due to busy_wait_us being inaccurate */
        busy_wait_us(100);
#endif

        /* the STM32C0 requires an averaging of eight calibration values */
#if defined(CPU_FAM_STM32C0) || defined(CPU_FAM_STM32G0)
        uint32_t calfact = 0;

        for (uint32_t i = 8; i > 0; i--) {
            /* perform a calibration and wait for the flag to clear */
            ADC1->CR = (ADC1->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADCAL;
            while (ADC1->CR & ADC_CR_ADCAL) {}

            calfact += ADC1->CALFACT;
        }
        /* round up to the nearest integer */
        calfact = (calfact + 4) / 8;

        /* enable the ADC to write the calibration factor and wait before writing and disabling */
        if (_enable_adc() == -1) {
            return -1;
        }
        busy_wait_us(100);

        /* apply the calibration factor and mask it in case it is bigger than 0x7F */
        ADC1->CALFACT = calfact & ADC_CALFACT_CALFACT;

        _disable_adc();

        /* configure sampling time to a safe value */
        ADC1->SMPR = ADC_SMPR_SMP1_2 | ADC_SMPR_SMP1_0; /* 39.5 ADC clock cycles */
#else
        /* perform a calibration and wait for the flag to clear */
        ADC1->CR = (ADC1->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADCAL;
        while (ADC1->CR & ADC_CR_ADCAL) {}

        /* configure sampling time to safe value */
        ADC1->SMPR = ADC_SMPR_SMP_1 | ADC_SMPR1_SMPR_0; /* 28.5 ADC clock cycles */
#endif
    }

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

    /* check if this is the VBAT line */
    if (IS_USED(MODULE_PERIPH_VBAT) && line == VBAT_ADC) {
        vbat_enable();
    }

    /* set resolution and channel */
    ADC1->CFGR1 = res;
    ADC1->CHSELR = (1 << adc_config[line].chan);

    /* check if the ADC was enabled successfully */
    if (_enable_adc() == -1) {
        done();
        return -1;
    }

    /* start conversion and wait for results */
    ADC1->CR = (ADC1->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC)) {}

    /* read result */
    sample = (int)ADC1->DR;

    /* check if this is the VBAT line */
    if (IS_USED(MODULE_PERIPH_VBAT) && line == VBAT_ADC) {
        vbat_disable();
    }

    /* disable, unlock and power off device again */
    int ret = _disable_adc();
    done();

    if (ret == -1) {
        return -1;
    } else {
        return sample;
    }
}

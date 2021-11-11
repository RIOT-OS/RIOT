/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 * Copyright (C) 2018 HAW-Hamburg
 * Copyright (C) 2021 Inria
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
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"
#include "ztimer.h"

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

int adc_init(adc_t line)
{
    /* check if the line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* lock device and enable its peripheral clock */
    prep();

    /* configure the pin */
    gpio_init_analog(adc_config[line].pin);

    /* init ADC line only if it wasn't already initialized */
    if (!(ADC->CR & (ADC_CR_ADEN))) {

        /* set prescaler to 0 to let the ADC run with maximum speed */
        ADC_COMMON->CCR &= ~(ADC_CCR_PRESC);

        /* set ADC clock to PCLK/2 otherwise */
        ADC->CFGR2 &= ~(ADC_CFGR2_CKMODE_0 | ADC_CFGR2_CKMODE_1);
        ADC->CFGR2 |= ADC_CFGR2_CKMODE_0;

        /* enable ADC internal voltage regulator and wait for startup period */
        ADC->CR |= (ADC_CR_ADVREGEN);
#if IS_USED(MODULE_ZTIMER_USEC)
        ztimer_sleep(ZTIMER_USEC, ADC_T_ADCVREG_STUP_US);
#else
        /* to avoid using ZTIMER_USEC unless already included round up the
           internal voltage regulator start up to 1ms */
        ztimer_sleep(ZTIMER_MSEC, 1);
#endif

        /* ´start automatic calibration and wait for it to complete */
        ADC->CR |= ADC_CR_ADCAL;
        while (ADC->CR  & ADC_CR_ADCAL) {}

        /* clear ADRDY by writing it*/
        ADC->ISR |= (ADC_ISR_ADRDY);

        /* enable ADC and wait for it to be ready */
        ADC->CR |= (ADC_CR_ADEN);
        while ((ADC->ISR & ADC_ISR_ADRDY) == 0) {}

        /* set sequence length to 1 conversion */
        ADC->CFGR1 &= ~ADC_CFGR1_CONT;

        /* Sampling time of 3.5 ADC clocks for all channels*/
        ADC->SMPR = 0x0101;
    }

    /* free the device again */
    done();
    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    int sample;

    /* check if resolution is applicable */
    if (res & 0x3) {
        return -1;
    }

    /* lock and power on the ADC device  */
    prep();

    /* first clear resolution */
    ADC->CFGR1 &= ~ADC_CFGR1_RES;

    /* then set resolution to the required value*/
    ADC->CFGR1 |= res;

    /* specify channel for regular conversion */
    ADC->CHSELR = (1 << adc_config[line].chan);

    /* start conversion and wait for it to complete */
    ADC->CR |= ADC_CR_ADSTART;
    while (!(ADC->ISR & ADC_ISR_EOC)) {}

    /* read the sample */
    sample = (int)ADC->DR;

    /* free the device again */
    done();

    return sample;
}

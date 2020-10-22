/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>


#include "mutex.h"
#include "periph/adc.h"

#define RES_INVALID         (0x0f)
#define WP_KEY              (0x414443)
#define PMC_BIT             (1 << (ID_ADC - 32))

/* max ADC clock is 6.4MHz, we aim for 5MHz per default */
#ifndef ADC_CLOCK_TARGET
#define ADC_CLOCK_TARGET    (5000000)
#endif
#define PRESCALER           ((CLOCK_CORECLOCK / (2 * ADC_CLOCK_TARGET)) - 1)


static mutex_t lock = MUTEX_INIT;

static inline void use(void)
{
    mutex_lock(&lock);
    PMC->PMC_PCER1 = PMC_BIT;
}

static inline void done(void)
{
    PMC->PMC_PCDR1 = PMC_BIT;
    mutex_unlock(&lock);
}

int adc_init(adc_t line)
{
    assert(line < ADC_NUMOF);

    use();

    /* disable ADC write protection */
    ADC->ADC_WPMR = ADC_WPMR_WPKEY(WP_KEY);
    ADC->ADC_CHDR = 0x0000ffff;

    /* if line 15 is used, enable the temperature sensor */
    if (line == 15) {
        ADC->ADC_ACR |= ADC_ACR_TSON;
    }

    done();
    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    assert(line < ADC_NUMOF);

    /* check if resolution is applicable */
    if (res & RES_INVALID) {
        return -1;
    }

    use();

    /* set resolution */
    ADC->ADC_MR = (res | ADC_MR_PRESCAL(PRESCALER));
    /* enable channel */
    ADC->ADC_CHER = (1 << line);

    /* start conversion */
    ADC->ADC_CR = ADC_CR_START;
    /* wait for result */
    while (!(ADC->ADC_ISR & ADC_ISR_DRDY)) {}
    /* read result */
    int sample = (int)(ADC->ADC_LCDR & ADC_LCDR_LDATA_Msk);

    /* disable channel */
    ADC->ADC_CHDR = (1 << line);

    done();
    return sample;
}

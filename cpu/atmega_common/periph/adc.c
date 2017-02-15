/*
 * Copyright (C) 2016 Laurent Navet <laurent.navet@gmail.com>
 *               2017 HAW Hamburg, Dimitri Nahm
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_periph
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation for ATmega family
 *
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/adc.h"
#include "periph_conf.h"

#define ADC_MAX_CLK         (200000U)

static mutex_t lock = MUTEX_INIT;

static inline void prep(void)
{
    mutex_lock(&lock);
    /* Enable ADC */
    ADCSRA |= (1 << ADEN);
}

static inline void done(void)
{
    /* Disable ADC */
    ADCSRA &= ~(1 << ADEN);
    mutex_unlock(&lock);
}

int adc_init(adc_t line)
{
    uint32_t clk_div = 1;

    /* check if the line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    prep();

    /* Disable corresponding Digital input */
    #if defined (CPU_ATMEGA328P) || defined (CPU_ATMEGA1281)
        DIDR0 |= (1 << line);
    #elif defined (CPU_ATMEGA2560)
        if (line < 8)
            DIDR0 |= (1 << line);
        else
            DIDR2 |= (1 << (line - 8));
    #endif

    /* Set ADC-pin as input */
    #if defined (CPU_ATMEGA328P)
        DDRC &= ~(1 << line);
        PORTC &= ~(1 << line);
    #elif defined (CPU_ATMEGA2560)
        if (line < 8) {
            DDRF &= ~(1 << line);
            PORTF &= ~(1 << line);
        }
        else {
            DDRK &= ~(1 << (line-8));
            PORTK &= ~(1 << (line-8));
        }
    #elif defined (CPU_ATMEGA1281)
        DDRF &= ~(1 << line);
        PORTF &= ~(1 << line);
    #endif

    /* set clock prescaler to get the maximal possible ADC clock value */
    for (clk_div = 1; clk_div < 8; clk_div++) {
        if ((CLOCK_CORECLOCK / (1 << clk_div)) <= ADC_MAX_CLK) {
            break;
        }
    }
    ADCSRA |= clk_div;

    /* Ref Voltage is Vcc(5V) */
    ADMUX |= (1 << REFS0);

    done();

    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
{
    int sample = 0;

    /* check if resolution is applicable */
    assert(res == ADC_RES_10BIT);

    prep();

    /* set conversion channel */
    #if defined (CPU_ATMEGA328P) || defined (CPU_ATMEGA1281)
        ADMUX &= 0xf0;
        ADMUX |= line;
    #endif
    #ifdef CPU_ATMEGA2560
        if(line < 8) {
            ADCSRB &= ~(1 << MUX5);
            ADMUX &= 0xf0;
            ADMUX |= line;
        }
        else {
            ADCSRB |= (1 << MUX5);
            ADMUX &= 0xf0;
            ADMUX |= (line-8);
        }
    #endif

    /* Start a new conversion. By default, this conversion will
       be performed in single conversion mode. */
    ADCSRA |= (1 << ADSC);

    /* Wait until the conversion is complete */
    while(ADCSRA & (1 << ADSC)) {}

    /* Get conversion result */
    sample = ADC;

    /* Clear the ADIF flag */
    ADCSRA |= (1 << ADIF);

    done();

    return sample;
}

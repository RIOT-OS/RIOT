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
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"

#define ADC_MAX_CLK         (200000U)

static mutex_t lock = MUTEX_INIT;

static inline void _prep(void)
{
    mutex_lock(&lock);
    /* Enable ADC */
    ADCSRA |= (1 << ADEN);
}

static inline void _done(void)
{
    /* Disable ADC */
    ADCSRA &= ~(1 << ADEN);
    mutex_unlock(&lock);
}

int adc_init(adc_t line)
{
    /* check if the line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    _prep();

    /* Disable corresponding Digital input */
    if (line < 8) {
        DIDR0 |= (1 << line);
    }
#if defined(CPU_ATMEGA2560)
    else {
        DIDR2 |= (1 << (line - 8));
    }
#endif

    /* Set ADC-pin as input */
#if defined(CPU_ATMEGA328P)
    DDRC &= ~(1 << line);
    PORTC &= ~(1 << line);
#elif defined(CPU_ATMEGA1284P)
    DDRA &= ~(1 << line);
    PORTA &= ~(1 << line);
#elif defined(CPU_ATMEGA2560) || defined(CPU_ATMEGA1281)
    if (line < 8) {
        DDRF  &= ~(1 << line);
        PORTF &= ~(1 << line);
    }
#if defined(CPU_ATMEGA2560)
    else {
        DDRK  &= ~(1 << (line-8));
        PORTK &= ~(1 << (line-8));
    }
#endif /* CPU_ATMEGA2560 */
#endif /* CPU_ATMEGA328P */

    /* set clock prescaler to get the maximal possible ADC clock value */
    for (uint32_t clk_div = 1; clk_div < 8; ++clk_div) {
        if ((CLOCK_CORECLOCK / (1 << clk_div)) <= ADC_MAX_CLK) {
            ADCSRA |= clk_div;
            break;
        }
    }

    /* Ref Voltage is Vcc(5V) */
    ADMUX |= (1 << REFS0);

    _done();

    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    int sample = 0;

    /* check if resolution is applicable */
    if (res != ADC_RES_10BIT) {
        return -1;
    }

    _prep();

    /* set conversion channel */
#if defined(CPU_ATMEGA328P) || defined(CPU_ATMEGA1281) || defined(CPU_ATMEGA1284P) || defined(CPU_ATMEGA32U4)
    ADMUX &= 0xf0;
    ADMUX |= line;
#elif defined(CPU_ATMEGA2560) || defined(CPU_ATMEGA128RFA1) || defined(CPU_ATMEGA256RFR2)
    if (line < 8) {
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
    while (ADCSRA & (1 << ADSC)) {}

    /* Get conversion result */
    sample = ADC;

    /* Clear the ADIF flag */
    ADCSRA |= (1 << ADIF);

    _done();

    return sample;
}

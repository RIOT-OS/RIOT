/*
 * Copyright (C) 2016 Laurent Navet <laurent.navet@gmail.com>
 *               2017 HAW Hamburg, Dimitri Nahm
 *               2023 Hugues Larrive
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
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph/pm.h"
#include "periph_conf.h"

#define ADC_MAX_CLK         (200000U)

static mutex_t lock = MUTEX_INIT;

static inline void _prep(void)
{
    pm_block(3); /* Require clkADC */
    mutex_lock(&lock);
    /* Enable ADC */
    ADCSRA |= (1 << ADEN);
}

static inline void _done(void)
{
    /* Disable ADC */
    ADCSRA &= ~(1 << ADEN);
    mutex_unlock(&lock);
    pm_unblock(3);
}

int adc_init(adc_t line)
{
    /* check if the line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    _prep();

#if defined(DIDR0)
    /* Disable corresponding Digital input */
    if (line < 8) {
        DIDR0 |= (1 << line);
    }
#if defined(DIDR2)
    else {
        DIDR2 |= (1 << (line - 8));
    }
#endif
#endif

    /* Set ADC-pin as input */
#if !defined(PORTA) && defined(PC0)
    /* 328p and 8 do not have PORTA, on 32u4 pins are named differently
     * and it only have PORTC6 and PORTC7 */
    DDRC &= ~(1 << line);
    PORTC &= ~(1 << line);
#elif defined(PORTA) && !defined(DIDR2) /* 1284p do not have DIDR2 */
    DDRA &= ~(1 << line);
    PORTA &= ~(1 << line);
#elif defined(PORTF) /* 2560 and 1281 */
    if (line < 8) {
        DDRF  &= ~(1 << line);
        PORTF &= ~(1 << line);
    }
#if     defined(PORTK) /* 2560 */
    else {
        DDRK  &= ~(1 << (line - 8));
        PORTK &= ~(1 << (line - 8));
    }
#elif   defined(PORTF0) && !defined(PORTF2) && !defined(PORTF3)
        /* 32u4 do not have PORTF2 and PORTF3 */
    else if (line == 8) {
        DDRD  &= ~(1 << PORTD4);
        PORTD &= ~(1 << PORTD4);
    }
    else if (line < 11) {
        DDRD  &= ~(1 << (line - 3));
        PORTD &= ~(1 << (line - 3));
    }
    else {
        DDRB  &= ~(1 << (line - 7));
        PORTB &= ~(1 << (line - 7));
    }
#endif /* PORTK */
#endif /* PORTF */

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
#if defined(ADMUX)
#if !defined(MUX5)
    /* atmega8 ; 328p ; 1281 ; 1284p ; 32u4 */
    ADMUX &= 0xf0;
    ADMUX |= line;
#else /* 2560 ; 128rfa1 ; 256rfr2 */
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

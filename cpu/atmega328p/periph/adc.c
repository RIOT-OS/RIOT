/*
 * Copyright (C) 2016 Laurent Navet <laurent.navet@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_atmega328p
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation for atmega328p mcu
 *
 * @author      Laurent Navet <laurent.navet@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph/adc.h"
#include "periph_conf.h"

int adc_init(adc_t line)
{
    /* check if the line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* configure line as input */
    DDRD &= (1 << line);

    /* Disable corresponding Digital input */
    DIDR0 |= (1 << line);

    /* Enable ADC */
    ADCSRA |= (1 << ADEN);

    /* Enable ADC with Prescaling = /128 = 125KHz */
    ADCSRA=(1 << ADEN)|(1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);

    /* Ref Voltage is Vcc(5V) */
    ADMUX = (1 << REFS0);

    /* Select pin channel */
    ADMUX |= line;

    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
{
    int sample = 0;

    /* Start a new conversion. By default, this conversion will
       be performed in single conversion mode. */
    ADCSRA |= (1 << ADSC);

    /* Wait until the conversion is complete */
    while(ADCSRA & (1 << ADIF));

    /* Get conversion result */
    sample = ADCW;

    /* Clear the ADIF flag */
    ADCSRA |= ( 1 << ADIF );

    return sample;
}

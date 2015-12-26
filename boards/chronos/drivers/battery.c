/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup chronos
 * @{
 */

/**
 * @file
 * @brief       eZ430-chronos battery readout (via ADC)
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 */

#include <stdint.h>
#include <stdint.h>
#include <cc430f6137.h>

#include "cc430-adc.h"

uint32_t battery_get_voltage(void)
{
    uint32_t voltage;
    voltage = adc12_single_conversion(REFVSEL_1, ADC12SHT0_10, ADC12INCH_11);

    /* Ideally we have A11=0->AVCC=0V ... A11=4095(2^12-1)->AVCC=4V
     * --> (A11/4095)*4V=AVCC --> AVCC=(A11*4)/4095 */
    voltage = (voltage * 2 * 2 * 1000) / 4095;
    return voltage;
}

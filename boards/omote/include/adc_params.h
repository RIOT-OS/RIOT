/*
 * Copyright (C) 2016 Eistec AB
 * Copyright (C) 2020 Oppila Microsystems - http://www.oppila.in
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_omote
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped ADC in omote
 *
 */

#ifndef ADC_PARAMS_H
#define ADC_PARAMS_H

#include "board.h"
#include "saul/periph.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    ADC configuration
 */
static const  saul_adc_params_t saul_adc_params[] =
{
    {
        .name = "ADC1",
        .line = ADC_LINE(0),
        .res  = ADC_RES_12BIT,
    },
    {
        .name = "ADC2",
        .line = ADC_LINE(1),
        .res  = ADC_RES_12BIT,
    },
    {
        .name = "ADC3",
        .line = ADC_LINE(2),
        .res  = ADC_RES_12BIT,
    }
};

#ifdef __cplusplus
}
#endif

#endif /* ADC_PARAMS_H */
/** @} */

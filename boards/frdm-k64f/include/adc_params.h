/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_frdm-k64f
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped ADC
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef ADC_PARAMS_H
#define ADC_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    ADC configuration
 */
static const  saul_adc_params_t saul_adc_params[] =
{
    { .name = "PTB2",     .line = ADC_LINE(0, 0), .res  = ADC_RES_16BIT, },
    { .name = "PTB3",     .line = ADC_LINE(0, 1), .res  = ADC_RES_16BIT, },
    { .name = "PTB10",    .line = ADC_LINE(0, 2), .res  = ADC_RES_16BIT, },
    { .name = "PTB11",    .line = ADC_LINE(0, 3), .res  = ADC_RES_16BIT, },
    { .name = "PTC11",    .line = ADC_LINE(0, 4), .res  = ADC_RES_16BIT, },
    { .name = "PTC10",    .line = ADC_LINE(0, 5), .res  = ADC_RES_16BIT, },
    { .name = "ADC0_DP0", .line = ADC_LINE(0, 6), .res  = ADC_RES_16BIT, },
    { .name = "ADC0_DM0", .line = ADC_LINE(0, 7), .res  = ADC_RES_16BIT, },
    { .name = "ADC0_DP0-ADC0_DM0", .line = ADC_LINE(0, 8), .res  = ADC_RES_16BIT, },
    { .name = "ADC1_DP0", .line = ADC_LINE(0, 9), .res  = ADC_RES_16BIT, },
    { .name = "ADC1_DM0", .line = ADC_LINE(0, 10), .res  = ADC_RES_16BIT, },
    { .name = "ADC1_DP0-ADC1_DM0", .line = ADC_LINE(0, 11), .res  = ADC_RES_16BIT, },
    { .name = "ADC0_DP1", .line = ADC_LINE(0, 12), .res  = ADC_RES_16BIT, },
    { .name = "ADC0_DM1", .line = ADC_LINE(0, 13), .res  = ADC_RES_16BIT, },
    { .name = "ADC0_DP1-ADC0_DM1", .line = ADC_LINE(0, 14), .res  = ADC_RES_16BIT, },
    { .name = "ADC1_DP1", .line = ADC_LINE(0, 15), .res  = ADC_RES_16BIT, },
    { .name = "ADC1_DM1", .line = ADC_LINE(0, 16), .res  = ADC_RES_16BIT, },
    { .name = "ADC1_DP1-ADC1_DM1", .line = ADC_LINE(0, 17), .res  = ADC_RES_16BIT, },
    { .name = "coretemp", .line = ADC_LINE(0, 18), .res  = ADC_RES_16BIT, },
    { .name = "corebandgap", .line = ADC_LINE(0, 19), .res  = ADC_RES_16BIT, },
};

#ifdef __cplusplus
}
#endif

#endif /* ADC_PARAMS_H */
/** @} */

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
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    ADC configuration
 */
static const  saul_adc_params_t saul_adc_params[] =
{
    { .line = ADC_LINE(0),  .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(1),  .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(2),  .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(3),  .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(4),  .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(5),  .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(6),  .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(7),  .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(8),  .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(9),  .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(10), .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(11), .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(12), .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(13), .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(14), .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(15), .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(16), .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(17), .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(18), .res  = ADC_RES_16BIT, },
    { .line = ADC_LINE(19), .res  = ADC_RES_16BIT, },
};

/**
 * @brief   ADC information for SAUL registry
 */
static const saul_reg_info_t saul_adc_info[] =
{
    { .name = "PTB2" },
    { .name = "PTB3" },
    { .name = "PTB10" },
    { .name = "PTB11" },
    { .name = "PTC11" },
    { .name = "PTC10" },
    { .name = "ADC0_DP0" },
    { .name = "ADC0_DM0" },
    { .name = "ADC0_DP0-ADC0_DM0" },
    { .name = "ADC1_DP0" },
    { .name = "ADC1_DM0" },
    { .name = "ADC1_DP0-ADC1_DM0" },
    { .name = "ADC0_DP1" },
    { .name = "ADC0_DM1" },
    { .name = "ADC0_DP1-ADC0_DM1" },
    { .name = "ADC1_DP1" },
    { .name = "ADC1_DM1" },
    { .name = "ADC1_DP1-ADC1_DM1" },
    { .name = "coretemp" },
    { .name = "corebandgap" }
};

#ifdef __cplusplus
}
#endif

#endif /* ADC_PARAMS_H */
/** @} */

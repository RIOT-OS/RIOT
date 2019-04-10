/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_frdm-kw41z
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
    {
        .line = ADC_LINE(0),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(1),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(2),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(3),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(4),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(5),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(6),
        .res  = ADC_RES_16BIT,
    },
};

/**
 * @brief   ADC information for SAUL registry
 */
static const saul_reg_info_t saul_adc_info[] =
{
    { .name = "ADC0_DP-DM" },
    { .name = "ADC0_DP" },
    { .name = "PTB2" },
    { .name = "PTB3" },
    { .name = "coretemp" },
    { .name = "corebandgap" },
    { .name = "dcdcvbat" }
};

#ifdef __cplusplus
}
#endif

#endif /* ADC_PARAMS_H */
/** @} */

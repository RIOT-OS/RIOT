/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup   boards_frdm-k22f
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped ADC
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

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
    {
        .name = "ADC0_DP",
        .line = ADC_LINE(0),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "ADC0_DM",
        .line = ADC_LINE(1),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "ADC1_DP",
        .line = ADC_LINE(2),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "ADC1_DM",
        .line = ADC_LINE(3),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "A0",
        .line = ADC_LINE(4),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "A1",
        .line = ADC_LINE(5),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "A2",
        .line = ADC_LINE(6),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "A3",
        .line = ADC_LINE(7),
        .res  = ADC_RES_16BIT,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
